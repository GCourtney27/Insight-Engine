#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffers.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Mesh
{
public:
	Mesh(ID3D11Device * device, ID3D11DeviceContext * deviceContext, std::vector<Vertex3D> & verticies, std::vector<DWORD> & indicies, std::vector<Texture> & textures, const DirectX::XMMATRIX & transformMatrix);
	Mesh(const Mesh & mesh);
	void Draw();
	const DirectX::XMMATRIX & GetTransformMatrix();

private:
	VertexBuffer<Vertex3D> vertexBuffer; // A mesh can have a bunch of verticies
	IndexBuffer indexBuffer; // Mesh can have a bunch of Indicies
	ID3D11DeviceContext * deviceContext;
	std::vector<Texture> textures;
	DirectX::XMMATRIX transformMatrix;
};