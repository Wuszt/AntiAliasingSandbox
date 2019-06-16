#include "MeshRenderer.h"
#include "Model.h"
#include "RenderingSystem.h"
#include "Core.h"

MeshRenderer::MeshRenderer(Object* const& owner, const std::string modelPath) : Component(owner)
{
    Core::GetRenderingSystem()->InitializeMeshRendererWithModelPath(this, modelPath);
}

MeshRenderer::MeshRenderer(Object* const& owner, const Model* const& model) : Component(owner)
{
    Core::GetRenderingSystem()->InitializeMeshRendererWithModel(this, model);
}

MeshRenderer::~MeshRenderer()
{
}
