#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Mesh.h"

struct Model
{
    DirectX::XMMATRIX TransformMatrix;

    std::string Name;

    std::vector<const Model*> Children;

    std::vector<const Mesh*> Meshes;
};

