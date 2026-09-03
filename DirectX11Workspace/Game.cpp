#include "pch.h"
#include "Game.h"
#include <climits>
#include "Graphics.h"
#include "GeometryHelper.h"
#include "Shader.h"
#include "Texture.h"
#include "RasterizerState.h"
#include "SamplerState.h"
#include "BlendState.h"
#include "Pipeline.h"
#include "GameObject.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;

	_graphics = std::make_shared<Graphics>(hwnd);

	const auto& device = _graphics->GetDevice();
	const auto& deviceContext = _graphics->GetDeviceContext();

	_pipeline = std::make_shared<Pipeline>(deviceContext);

	_gameObject = std::make_shared<GameObject>(device, deviceContext);
}

void Game::Update()
{
	// 이걸 모든 gameObject에 대해 실행해야할 것.
	_gameObject->Update();
}

void Game::Render()
{
	_graphics->RenderBegin(); // 준비 작업. 도화지 초기화 등.

	// 실제 그리는 작업.
	// IA - VS - RS - PS - OM

	_gameObject->Render(_pipeline);

	_graphics->RenderEnd(); // 다 그렸으니 제출. 이런 흐름
}