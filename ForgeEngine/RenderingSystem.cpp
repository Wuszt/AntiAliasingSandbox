#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "RenderingSystem.h"
#include "Model.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Transform.h"
#include "Core.h"
#include "Material.h"
#include <DirectXTex/DirectXTex.h>
#include "ShadersManager.h"
#include <d3d9types.h>
#include "Profiler.h"
#include "Core.h"

#include <sstream>

using namespace DirectX;
using namespace std;

RenderingSystem::RenderingSystem()
{
    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbPerObject);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    Core::GetD3Device()->CreateBuffer(&cbbd, nullptr, &m_cbPerObjectBuff);
}

RenderingSystem::~RenderingSystem()
{
    for (auto const& entry : m_models)
    {
        ReleaseModel(entry.second);
    }

    m_cbPerObjectBuff->Release();
}

void RenderingSystem::RenderRegisteredMeshRenderers(Camera* const& camera)
{
    for (MeshRenderer* const& renderer : m_meshRenderers)
    {
        m_cbPerObj.WVP = XMMatrixTranspose(renderer->GetOwner()->GetTransform()->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix());
        m_cbPerObj.W = XMMatrixTranspose(renderer->GetOwner()->GetTransform()->GetWorldMatrix());
        m_cbPerObj.PrevWVP = renderer->PrevWVP;
        renderer->PrevWVP = m_cbPerObj.WVP;

        Core::GetD3DeviceContext()->UpdateSubresource(m_cbPerObjectBuff, 0, nullptr, &m_cbPerObj, 0, 0);
        Core::GetD3DeviceContext()->VSSetConstantBuffers(static_cast<UINT>(VertexCBIndex::PerObject), 1, &m_cbPerObjectBuff);

        for (const Mesh* const& mesh : *renderer->m_meshes)
        {
            Core::GetD3DeviceContext()->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

            UINT offset = 0;
            Core::GetD3DeviceContext()->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &mesh->Stride, &offset);
            
            static ID3D11Buffer* materialBuff;
            materialBuff = mesh->Material->GetConstantBufferMaterialBuffer();
            Core::GetD3DeviceContext()->VSSetConstantBuffers(static_cast<UINT>(VertexCBIndex::Material), 1, &materialBuff);

            if (mesh->Material->Textures.size() > 0)
                Core::GetD3DeviceContext()->PSSetShaderResources(0, 1, &mesh->Material->Textures[0]);

            static const CachedShaders* cachedShaders;
            cachedShaders = mesh->Material->GetShaders();

            Core::GetD3DeviceContext()->VSSetShader(cachedShaders->GetVS().Shader, 0, 0);
            Core::GetD3DeviceContext()->PSSetShader(cachedShaders->GetPS().Shader, 0, 0);

            Core::GetD3DeviceContext()->IASetInputLayout(mesh->Material->GetInputLayout());

            Core::GetD3DeviceContext()->DrawIndexed(mesh->IndicesAmount, 0, 0);
        }
    }
}

void RenderingSystem::InitializeMeshRendererWithModelPath(MeshRenderer* const& meshRenderer, const std::string& modelPath, const std::string& shaderPath)
{
    auto alreadyCreated = m_models.find(modelPath);
    const Model* model = nullptr;


    if (alreadyCreated != m_models.end())
        model = alreadyCreated->second;
    else
    {
        model = LoadModelFromPath(modelPath, shaderPath);
        m_models.insert({ modelPath, model });
    }

    InitializeMeshRendererWithModel(meshRenderer, model, shaderPath);
}

void RenderingSystem::InitializeMeshRendererWithModel(MeshRenderer* const& meshRenderer, const Model* const& model, const std::string& shaderPath)
{
    meshRenderer->m_meshes = &(model->Meshes);
    m_meshRenderers.insert(meshRenderer);

    for (const Model* const& child : model->Children)
    {
        Object* obj = Core::InstantiateObject<Object>();
        obj->GetTransform()->SetParent(meshRenderer->GetOwner()->GetTransform());
        obj->GetTransform()->SetFromMatrix(child->TransformMatrix);
        obj->AddComponent<MeshRenderer>(child, shaderPath);

        if (child->Name.length() > 0)
            obj->Name = child->Name;
    }
}

const Model* RenderingSystem::LoadModelFromPath(const std::string& modelPath, const std::string& shaderPath)
{
    const Model* model;

    Assimp::Importer importer;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    unsigned int flags = aiProcess_Triangulate
        | aiProcess_ConvertToLeftHanded
        | aiProcess_FixInfacingNormals
        | aiProcess_FindInvalidData;

    const aiScene* pScene = importer.ReadFile(modelPath, flags);
    model = LoadModelFromNode(pScene, pScene->mRootNode, shaderPath);

    return model;
}

const Model* RenderingSystem::LoadModelFromNode(const aiScene* const& scene, const aiNode* const& node, const std::string& shaderPath)
{
    Model* model = new Model();
    model->TransformMatrix = GetMatrixFromAssimp(node->mTransformation);
    model->TransformMatrix = XMMatrixTranspose(model->TransformMatrix);

    model->Meshes = LoadMeshesFromNode(scene, node, shaderPath);
    model->Name = string(node->mName.C_Str());

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        if (node->mChildren[i]->mNumMeshes == 0 && node->mChildren[i]->mChildren == 0)
            continue;

        const Model* child = LoadModelFromNode(scene, node->mChildren[i], shaderPath);
        model->Children.push_back(child);
    }

    return model;
}

vector<const Mesh*> RenderingSystem::LoadMeshesFromNode(const aiScene* const& scene, const aiNode* const& node, const std::string& shaderPath)
{
    vector<const Mesh*> meshes;

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        Mesh* mesh = new Mesh;

        mesh->Material = new Material();

        vector<float> vertData;
        vector<DWORD> indices;

        aiMesh* meshData = scene->mMeshes[node->mMeshes[i]];

        aiMaterial* mat = scene->mMaterials[meshData->mMaterialIndex];

        //Opacity - ignoring transparency for now
        float opacity;
        if (AI_SUCCESS == mat->Get(AI_MATKEY_OPACITY, opacity))
        {
            if (opacity < 1.0f)
                continue;
        }
        //Finish opacity

        for (int i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
        {
            int amount = mat->GetTextureCount((aiTextureType)i);

            for (int a = 0; a < amount; ++a)
            {
                aiString path;
                mat->GetTexture((aiTextureType)i, a, &path);
                ID3D11ShaderResourceView* srv = GetResourceFromTexturePath(string(path.C_Str()));
                mesh->Material->Textures.push_back(srv);
                mesh->Material->ShaderPath = shaderPath;
            }

        }

        UINT offset = 0;

        aiColor3D tmpClr;
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, tmpClr))
        {
            mesh->Material->Diffuse = XMFLOAT3(tmpClr.r, tmpClr.g, tmpClr.b);
        }

        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, tmpClr))
        {
            mesh->Material->Specular = XMFLOAT3(tmpClr.r, tmpClr.g, tmpClr.b);
        }

        if (meshData->HasPositions())
        {
            mesh->Material->Layout.push_back(
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
            offset += 12;
        }


        if (meshData->HasNormals())
        {
            mesh->Material->Layout.push_back(
                { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
            offset += 12;
        }


        UINT texIndex = 0;
        while (meshData->HasTextureCoords(texIndex))
        {
            mesh->Material->Layout.push_back(
                { "TEXCOORD", texIndex, DXGI_FORMAT_R32G32_FLOAT,0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
            offset += 8;
            ++texIndex;
        }

        UINT clrIndex = 0;
        while (meshData->HasVertexColors(clrIndex))
        {
            mesh->Material->Layout.push_back(
                { "COLOR", clrIndex, DXGI_FORMAT_R32G32B32A32_FLOAT,0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
            offset += 16;

            ++clrIndex;
        }

        for (unsigned int x = 0; x < meshData->mNumVertices; ++x)
        {
            aiVector3D vert = meshData->mVertices[x];
            if (meshData->HasPositions())
            {
                vertData.push_back(vert.x);
                vertData.push_back(vert.y);
                vertData.push_back(vert.z);
            }

            if (meshData->HasNormals())
            {
                aiVector3D norm = meshData->mNormals[x];
                vertData.push_back(norm.x);
                vertData.push_back(norm.y);
                vertData.push_back(norm.z);
            }

            texIndex = 0;
            while (meshData->HasTextureCoords(texIndex))
            {
                aiVector3D uvs = meshData->mTextureCoords[texIndex][x];
                vertData.push_back(uvs.x);
                vertData.push_back(uvs.y);

                ++texIndex;
            }

            clrIndex = 0;
            while (meshData->HasVertexColors(clrIndex))
            {
                aiColor4D clr = meshData->mColors[clrIndex][x];
                vertData.push_back(clr.r);
                vertData.push_back(clr.g);
                vertData.push_back(clr.b);
                vertData.push_back(clr.a);

                ++clrIndex;
            }
        }

        mesh->Stride = offset;

        for (unsigned int f = 0; f < meshData->mNumFaces; ++f)
        {
            aiFace face = meshData->mFaces[f];
            for (unsigned int x = 0; x < face.mNumIndices; ++x)
            {
                indices.push_back(face.mIndices[x]);
            }
        }

        mesh->IndexBuffer = CreateIndexBuffer(indices);
        mesh->VertexBuffer = CreateVertexBuffer(vertData);
        mesh->IndicesAmount = (UINT)indices.size();

        meshes.push_back(mesh);
    }

    return meshes;
}

ID3D11Buffer* RenderingSystem::CreateVertexBuffer(const std::vector<float>& vertData)
{
    ID3D11Buffer* result;

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = 4 * (UINT)vertData.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData;

    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = vertData.data();
    HRESULT hr = Core::GetD3Device()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &result);

    if (hr != S_OK)
        throw std::exception("Error while creating vertex buffer");

    return result;
}

ID3D11Buffer* RenderingSystem::CreateIndexBuffer(const vector<DWORD>& indices)
{
    ID3D11Buffer* result;

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices.data();

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * (UINT)indices.size();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    HRESULT hr = Core::GetD3Device()->CreateBuffer(&indexBufferDesc, &iinitData, &result);

    if (hr != S_OK)
        throw std::exception("Error while creating index buffer");

    return result;
}

ID3D11ShaderResourceView* RenderingSystem::GetResourceFromTexturePath(std::string path)
{
    const Image* images = nullptr;

    DirectX::ScratchImage si;
    DirectX::TexMetadata meta;

    wstring ws(path.begin(), path.end());

    LoadFromWICFile(ws.c_str(), WIC_FLAGS_NONE, &meta, si);
    images = si.GetImages();


    ID3D11ShaderResourceView* srv;
    CreateShaderResourceView(Core::GetD3Device(), images, 1, meta, &srv);

    return srv;
}

DirectX::XMMATRIX RenderingSystem::GetMatrixFromAssimp(const aiMatrix4x4 &matrix)
{
    return XMMATRIX(matrix.a1, matrix.a2, matrix.a3, matrix.a4,
        matrix.b1, matrix.b2, matrix.b3, matrix.b4,
        matrix.c1, matrix.c2, matrix.c3, matrix.c4,
        matrix.d1, matrix.d2, matrix.d3, matrix.d4);
}

void RenderingSystem::ReleaseModel(const Model* const& model)
{
    for (const Mesh* const& mesh : model->Meshes)
    {
        mesh->IndexBuffer->Release();
        mesh->VertexBuffer->Release();
        delete mesh;
    }

    for (const Model* const& child : model->Children)
    {
        ReleaseModel(child);
    }

    delete model;
}
