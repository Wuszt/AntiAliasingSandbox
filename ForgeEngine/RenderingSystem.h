#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string.h>
#include <DirectXMath.h>
#include <vector>
#include <assimp/matrix4x4.h>
#include "Material.h"
#include <d3d11.h>
#include "ConstantBuffers.h"

struct aiScene;
struct aiNode;
struct Model;
struct Mesh;
class Object;
class MeshRenderer;
class Camera;
class ShadersManager;

class RenderingSystem
{
public:
    RenderingSystem();
    ~RenderingSystem();


    void RenderRegisteredMeshRenderers(Camera* const& camera);

    void InitializeMeshRendererWithModelPath(MeshRenderer* const& meshRenderer, const std::string& modelPath, const std::string& shaderPath);
    void InitializeMeshRendererWithModel(MeshRenderer* const& meshRenderer, const Model* const& model, const std::string& shaderPath);

private:
    const Model* LoadModelFromPath(const std::string& modelPath, const std::string& shaderPath);

    const Model* LoadModelFromNode(const aiScene* const& scene, const aiNode* const& node, const std::string& shaderPath);

    std::vector<const Mesh*> LoadMeshesFromNode(const aiScene* const& scene, const aiNode* const& node, const std::string& shaderPath);

    ID3D11Buffer* CreateVertexBuffer(const std::vector<float>& vertData);
    ID3D11Buffer* CreateIndexBuffer(const std::vector<DWORD>& indices);

    ID3D11ShaderResourceView* GetResourceFromTexturePath(std::string path);

    DirectX::XMMATRIX GetMatrixFromAssimp(const aiMatrix4x4 &matrix);

    void ReleaseModel(const Model* const& model);

    std::unordered_map<std::string,const Model* const> m_models;

    std::unordered_set<MeshRenderer*> m_meshRenderers;

    cbPerObject m_cbPerObj;
    ID3D11Buffer* m_cbPerObjectBuff;
};

