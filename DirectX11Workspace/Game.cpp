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

	_vertexBuffer = std::make_shared<VertexBuffer>(device);
	_indexBuffer = std::make_shared<IndexBuffer>(device);
	_inputLayout = std::make_shared<InputLayout>(device);
	_geometry = std::make_shared<Geometry<VertexTextureData>>();
	_vertexShader = std::make_shared<VertexShader>(device);
	_pixelShader = std::make_shared<PixelShader>(device);
	_constantBuffer = std::make_shared<ConstantBuffer<TransformData>>(device, deviceContext);

	_textures.resize(2);

	_textures[0] = std::make_shared<Texture>(device);
	_textures[1] = std::make_shared<Texture>(device);

	_pipeline = std::make_shared<Pipeline>(deviceContext);

	//--- 기하(도형) 만들기

	// 버텍스들의 데이터! 이건 CPU에 저장하므로, 아직 CPU의 영역 = RAM에 저장됨.
	GeometryHelper::CreateRectangle(_geometry);

	_vertexBuffer->Create(_geometry->GetVertices());

	_indexBuffer->Create(_geometry->GetIndices());

	_vertexShader->Create(L"Default.hlsl", "VS", "vs_5_0");

	// 아직까지도 GPU 입장에서 버텍스 버퍼는 그냥 데이터 쪼가리임. 이걸로 뭘 하라고?를 알려줌
	// 실행 단계와 다르게, 생성에서는 InputLayout은 VS보다 뒤에 만들어야한다. vsBlob이 필요해서...
	_inputLayout->Create(VertexTextureData::descs, _vertexShader->GetBlob());

	//---

	_rasterizerState = std::make_shared<RasterizerState>(device);
	_samplerState = std::make_shared<SamplerState>(device);
	_blendState = std::make_shared<BlendState>(device);

	_pixelShader->Create(L"Default.hlsl", "PS", "ps_5_0");

	_rasterizerState->Create();
	_samplerState->Create();
	_blendState->Create();

	_textures[0]->Create(L"Skeleton.png");
	_textures[1]->Create(L"Golem.jpg");

	_constantBuffer->Create();
}

void Game::Update()
{
	_localPosition.x += 0.001f;

	Matrix matScale = Matrix::CreateScale(_localScale / 3);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	// 이 회전도 순서를 무조건 잘 지켜야한다. X->Y->Z 순이다. 순서에 따라 각도가 달라진다.
	// 물론, 이런 오일러 각은 사용이 좀 불편하고 짐벌락 문제도 있어서 쿼터니언을 쓰긴 한다. 지금은 오일러로.
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	Matrix matWorld = matScale * matRotation * matTranslation; // SRT! 

	_transformData.matWorld = matWorld;

	_constantBuffer->CopyData(_transformData);
}

void Game::Render()
{
	_graphics->RenderBegin(); // 준비 작업. 도화지 초기화 등.

	// 실제 그리는 작업.
	// IA - VS - RS - PS - OM

	{
		PipelineInfo info;
		info.inputLayout = _inputLayout;
		info.vertexShader = _vertexShader;
		info.pixelShader = _pixelShader;
		info.rasterizerState = _rasterizerState;
		info.blendState = _blendState;

		_pipeline->UpdatePipeline(info);

		// IA

		const auto& deviceContext = _graphics->GetDeviceContext();

		_pipeline->SetVertexBuffer(_vertexBuffer);

		_pipeline->SetIndexbuffer(_indexBuffer);

		// VS

		_pipeline->SetConstantBuffer(0, ShaderScope::VertexShader, _constantBuffer);

		// PS

		_pipeline->SetTexture(0, ShaderScope::PixelShader, _textures[0]);
		_pipeline->SetTexture(1, ShaderScope::PixelShader, _textures[1]);

		_pipeline->SetSamplerState(0, ShaderScope::PixelShader, _samplerState);

		// OM
		_pipeline->DrawIndexed(_geometry->GetIndexCount(), 0, 0);
	}

	_graphics->RenderEnd(); // 다 그렸으니 제출. 이런 흐름
}