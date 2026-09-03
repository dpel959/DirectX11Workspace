#pragma once
#include <string>
#include <memory>
#include "Geometry.h"
#include "VertexData.h"
#include "ConstantBuffer.h"

class Pipeline;

class VertexShader;
class PixelShader;
class Texture;
class RasterizerState;
class SamplerState;
class BlendState;

class Game
{
public:
	Game();
	~Game();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();

private:

	void CreateRasterizerState();

	void CreateSamplerState();
	void CreateBlendState();

private:
	HWND _hwnd;

	std::shared_ptr<Graphics> _graphics;
	std::shared_ptr<Pipeline> _pipeline;

	// Geometry. 도형 = Mesh 만들기.
	// Geometry는 리소스이다. 각 '객체'가 아닌, '리소스'(메쉬) 단위로 하나씩 들고 있으면 되는 것.
	std::shared_ptr<Geometry<VertexTextureData>> _geometry;

	std::shared_ptr<VertexBuffer> _vertexBuffer;
	std::shared_ptr<IndexBuffer> _indexBuffer;
	std::shared_ptr<InputLayout> _inputLayout;

	// VS 
	std::shared_ptr<VertexShader> _vertexShader;

	// RS
	std::shared_ptr<RasterizerState> _rasterizerState;

	// PS
	std::shared_ptr<PixelShader> _pixelShader;

	// SRV
	std::vector<std::shared_ptr<Texture>> _textures;
	std::shared_ptr<SamplerState> _samplerState;
	std::shared_ptr<BlendState> _blendState;

	// constant buffer
	TransformData _transformData;
	std::shared_ptr<ConstantBuffer<TransformData>> _constantBuffer;

	// local SRT

	Vec3 _localPosition = { 0.f,0.f,0.f };
	Vec3 _localRotation = { 0.f,0.f,0.f };
	Vec3 _localScale = { 1.f,1.f,1.f };
};

