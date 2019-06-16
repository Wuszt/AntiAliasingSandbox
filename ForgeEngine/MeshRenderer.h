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
    MeshRenderer(Object* const& owner, const std::string modelPath);
    MeshRenderer(Object* const& owner, const Model* const& model);
    ~MeshRenderer();

private:
    const std::vector<const Mesh*>* m_meshes;
};

