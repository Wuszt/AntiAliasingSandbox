#pragma once
#include <vector>

#include "Component.h"
#include "Mesh.h"

class Object;
struct Model;

class MeshRenderer : public Component
{
    friend class RenderingSystem;

public:
    MeshRenderer(Object* const& owner, const std::string& modelPath, const std::string& shaderPath);
    MeshRenderer(Object* const& owner, const Model* const& model, const std::string& shaderPath);
    ~MeshRenderer();

private:
    const std::vector<const Mesh*>* m_meshes;
};

