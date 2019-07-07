#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string.h>
#include <DirectXMath.h>
#include <vector>
#include <assimp/matrix4x4.h>
#include "Material.h"
#include <d3d11.h>
#include <FW1FontWrapper/FW1FontWrapper.h>
#include "ConstantBuffers.h"

struct aiScene;
struct aiNode;
struct Model;
struct Mesh;
class Object;
class MeshRenderer;
class Camera;
class ShadersManager;

enum class TextAnchor
{
    Top = 0,
    Center = 1,   
    Bottom = 2,
    Left = 4,
    Right = 8
};

inline TextAnchor operator| (const TextAnchor& l, const TextAnchor& r) { return static_cast<TextAnchor>(static_cast<UINT>(l) | static_cast<UINT>(r)); }

class RenderingSystem
{
public:
    RenderingSystem(ID3D11Device* const& d3Device, ID3D11DeviceContext* const& d3DeviceContext);
    ~RenderingSystem();


    void RenderRegisteredMeshRenderers(Camera* const& camera);

    void InitializeMeshRendererWithModelPath(MeshRenderer* const& meshRenderer, const std::string& modelPath, const std::string& shaderPath);
    void InitializeMeshRendererWithModel(MeshRenderer* const& meshRenderer, const Model* const& model, const std::string& shaderPath);

    void DrawText(const std::string& text, const float& size, const float& x, const float& y, const DirectX::XMFLOAT4& color, const TextAnchor& anchor = TextAnchor::Center) const;

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

    ID3D11Device* m_d3Device;
    ID3D11DeviceContext* m_d3DeviceContext;

    std::unordered_set<MeshRenderer*> m_meshRenderers;

    cbPerObject m_cbPerObj;
    ID3D11Buffer* m_cbPerObjectBuff;

    IFW1Factory* m_textFactory;
    IFW1FontWrapper* m_fontWrapper;

};

