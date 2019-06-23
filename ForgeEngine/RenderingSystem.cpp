#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "RenderingSystem.h"
#include "Model.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Transform.h"
#include "Core.h"

using namespace DirectX;
using namespace std;

RenderingSystem::RenderingSystem(ID3D11Device* const& d3Device, ID3D11DeviceContext* const& d3DeviceContext)
{
    m_d3Device = d3Device;
    m_d3DeviceContext = d3DeviceContext;
}

RenderingSystem::~RenderingSystem()
{
}

void RenderingSystem::Render(Camera* const& camera)
{
    int i = 0;
    for (MeshRenderer* const& renderer : m_meshRenderers)
    {
        for (const Mesh* const& mesh : *renderer->m_meshes)
        {
            ++i;
            m_d3DeviceContext->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            m_d3DeviceContext->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &stride, &offset);

            cbPerObj.WVP = XMMatrixTranspose(renderer->GetOwner()->GetTransform()->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix());

            static ID3D11Buffer* buff = nullptr;

            static bool tmp = false;

            if (!tmp)
            {
                static D3D11_BUFFER_DESC cbbd;
                ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

                cbbd.Usage = D3D11_USAGE_DEFAULT;
                cbbd.ByteWidth = sizeof(cbPerObject);
                cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

                m_d3Device->CreateBuffer(&cbbd, nullptr, &buff);
                tmp = true;
            }

            m_d3DeviceContext->UpdateSubresource(buff, 0, nullptr, &cbPerObj, 0, 0);
            m_d3DeviceContext->VSSetConstantBuffers(0, 1, &buff);

            m_d3DeviceContext->DrawIndexed(mesh->IndicesAmount, 0, 0);
        }
    }
}

void RenderingSystem::InitializeMeshRendererWithModelPath(MeshRenderer* const& meshRenderer, const std::string& modelPath)
{
    auto alreadyCreated = m_models.find(modelPath);
    const Model* model = nullptr;


    if (alreadyCreated != m_models.end())
        model = alreadyCreated->second;
    else
    {
        model = LoadModelFromPath(modelPath);
        m_models.insert({ modelPath, model });
    }

    InitializeMeshRendererWithModel(meshRenderer, model);
}

void RenderingSystem::InitializeMeshRendererWithModel(MeshRenderer* const& meshRenderer, const Model* const& model)
{
    meshRenderer->m_meshes = &(model->Meshes);
    m_meshRenderers.insert(meshRenderer);

    for (const Model* const& child : model->Children)
    {
        Object* obj = Core::InstantiateObject<Object>();
        obj->GetTransform()->SetParent(meshRenderer->GetOwner()->GetTransform());
        obj->GetTransform()->SetFromMatrix(child->TransformMatrix);
        obj->AddComponent<MeshRenderer>(child);

        if (child->Name.length() > 0)
            obj->Name = child->Name;
    }
}

const Model* RenderingSystem::LoadModelFromPath(const std::string& modelPath)
{
    const Model* model;

    Assimp::Importer importer;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    unsigned int flags = aiProcess_Triangulate
        | aiProcess_ConvertToLeftHanded;

    const aiScene* pScene = importer.ReadFile(modelPath, flags);
    model = LoadModelFromNode(pScene, pScene->mRootNode);

    return model;
}

const Model* RenderingSystem::LoadModelFromNode(const aiScene* const& scene, const aiNode* const& node)
{
    Model* model = new Model();
    model->TransformMatrix = GetMatrixFromAssimp(node->mTransformation);
    model->TransformMatrix = XMMatrixTranspose(model->TransformMatrix);

    model->Meshes = LoadMeshesFromNode(scene, node);
    model->Name = string(node->mName.C_Str());

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        if(node->mChildren[i]->mNumMeshes == 0 && node->mChildren[i]->mChildren == 0)
            continue;

        const Model* child = LoadModelFromNode(scene, node->mChildren[i]);
        model->Children.push_back(child);
    }

    return model;
}

vector<const Mesh*> RenderingSystem::LoadMeshesFromNode(const aiScene* const& scene, const aiNode* const& node)
{
    vector<const Mesh*> meshes;

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        Mesh* mesh = new Mesh;

        vector<Vertex> vertices;
        vector<DWORD> indices;

        aiMesh* meshData = scene->mMeshes[node->mMeshes[i]];

        aiMaterial* mat = scene->mMaterials[meshData->mMaterialIndex];

        //Opacity - ignoring transparency for now
        float opacity;
        mat->Get(AI_MATKEY_OPACITY, opacity);

        if (opacity < 1.0f)
            continue;
        //Finish opacity

        for (unsigned int x = 0; x < meshData->mNumVertices; ++x)
        {
            aiVector3D verts = meshData->mVertices[x];

            aiVector3D uvs;
            if (meshData->mTextureCoords[0])
                uvs = meshData->mTextureCoords[0][x];
            vertices.push_back(Vertex(verts.x, verts.y, verts.z, uvs.x, uvs.y));
        }

        for (unsigned int f = 0; f < meshData->mNumFaces; ++f)
        {
            aiFace face = meshData->mFaces[f];
            for (unsigned int x = 0; x < face.mNumIndices; ++x)
            {
                indices.push_back(face.mIndices[x]);
            }
        }

        mesh->IndexBuffer = CreateIndexBuffer(indices);
        mesh->VertexBuffer = CreateVertexBuffer(vertices);

        mesh->IndicesAmount = (UINT)indices.size();

        meshes.push_back(mesh);
    }

    return meshes;
}

ID3D11Buffer* RenderingSystem::CreateVertexBuffer(const vector<Vertex>& vertices)
{
    ID3D11Buffer* result;

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * (UINT)vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData;

    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = vertices.data();
    HRESULT hr = m_d3Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &result);

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
    HRESULT hr = m_d3Device->CreateBuffer(&indexBufferDesc, &iinitData, &result);

    if (hr != S_OK)
        throw std::exception("Error while creating index buffer");

    return result;
}

DirectX::XMMATRIX RenderingSystem::GetMatrixFromAssimp(const aiMatrix4x4 &matrix)
{
    //float arr[16];

    //for (int x = 0; x < 4; ++x)
    //{
    //    for (int y = 0; y < 4; ++y)
    //    {
    //        arr[y + x * 4] = matrix[x][y];
    //    }
    //}

    return XMMATRIX(matrix.a1, matrix.a2, matrix.a3, matrix.a4,
        matrix.b1, matrix.b2, matrix.b3, matrix.b4, 
        matrix.c1, matrix.c2, matrix.c3, matrix.c4, 
        matrix.d1, matrix.d2, matrix.d3, matrix.d4);
}
