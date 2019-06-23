#pragma once
#include <d3d11.h>

struct Material;

struct Mesh
{
    ID3D11Buffer* VertexBuffer;

    UINT IndicesAmount;
    ID3D11Buffer* IndexBuffer;

    Material* Material;
};

