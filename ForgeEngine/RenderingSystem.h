#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string.h>
#include <DirectXMath.h>
#include <vector>
#include <assimp/matrix4x4.h>
#include "Material.h"

#include <d3d11.h>

struct aiScene;
struct aiNode;
struct Model;
struct Mesh;
class Object;
class MeshRenderer;
class Camera;


struct Vertex
{
    Vertex() {}
    Vertex(const float& x, const float& y, const float& z,
        const float& u, const float& v)
        : pos(x, y, z), texCoord(u, v) {}

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 texCoord;
};

struct cbPerObject
{
    DirectX::XMMATRIX WVP;
};

class RenderingSystem
{
public:
    RenderingSystem(ID3D11Device* const& d3Device, ID3D11DeviceContext* const& d3DeviceContext);
    ~RenderingSystem();

    void Render(Camera* const& camera);

    void InitializeMeshRendererWithModelPath(MeshRenderer* const& meshRenderer, const std::string& modelPath);
    void InitializeMeshRendererWithModel(MeshRenderer* const& meshRenderer, const Model* const& model);

private:
    const Model* LoadModelFromPath(const std::string& modelPath);

    const Model* LoadModelFromNode(const aiScene* const& scene, const aiNode* const& node);

    TextureTypes GetTextureTypeFromAssimp(const int& type);

    std::vector<const Mesh*> LoadMeshesFromNode(const aiScene* const& scene, const aiNode* const& node);

    ID3D11Buffer* CreateVertexBuffer(const std::vector<Vertex>& vertices);
    ID3D11Buffer* CreateIndexBuffer(const std::vector<DWORD>& indices);

    ID3D11ShaderResourceView* GetResourceFromTexturePath(std::string path);

    DirectX::XMMATRIX GetMatrixFromAssimp(const aiMatrix4x4 &matrix);

    void ReleaseModel(const Model* const& model);

    std::unordered_map<std::string,const Model* const> m_models;

    ID3D11Device* m_d3Device;
    ID3D11DeviceContext* m_d3DeviceContext;

    std::unordered_set<MeshRenderer*> m_meshRenderers;

    cbPerObject cbPerObj;

    ID3D11Buffer* m_buff;
};

