#pragma once
#include <string>
#include <memory>
#include "Geometry.h"
#include "VertexData.h"
#include "ConstantBuffer.h"
#include "GameObject.h"

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
	HWND _hwnd;

	std::shared_ptr<Graphics> _graphics;
	std::shared_ptr<Pipeline> _pipeline;

	std::shared_ptr<GameObject> _gameObject;
};

