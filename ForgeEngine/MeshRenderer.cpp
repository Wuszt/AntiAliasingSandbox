#include "MeshRenderer.h"
#include "Model.h"
#include "RenderingSystem.h"
#include "Core.h"

MeshRenderer::MeshRenderer(Object* const& owner, const std::string& modelPath, const std::string& shaderPath) : Component(owner)
{
    Core::GetRenderingSystem()->InitializeMeshRendererWithModelPath(this, modelPath, shaderPath);
}

MeshRenderer::MeshRenderer(Object* const& owner, const Model* const& model, const std::string& shaderPath) : Component(owner)
{
    Core::GetRenderingSystem()->InitializeMeshRendererWithModel(this, model, shaderPath);
}

MeshRenderer::~MeshRenderer()
{
}
