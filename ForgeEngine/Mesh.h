#pragma once
#include <d3d11.h>

class Material;

struct Mesh
{
    ID3D11Buffer* VertexBuffer;
    UINT Stride;

    UINT IndicesAmount;
    ID3D11Buffer* IndexBuffer;

    Material* Material;
};

