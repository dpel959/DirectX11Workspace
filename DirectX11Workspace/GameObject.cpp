#include "pch.h"
#include "GameObject.h"
#include "GeometryHelper.h"
#include "Shader.h"
#include "RasterizerState.h"
#include "SamplerState.h"
#include "BlendState.h"
#include "Texture.h"
#include "Pipeline.h"

GameObject::GameObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
	:_device(device)
{
	//--- 기하(도형) 만들기
	_geometry = std::make_shared<Geometry<VertexTextureData>>();
	//_geometry = std::make_shared<Geometry<VertexColorData>>();
	GeometryHelper::CreateRectangle(_geometry);
	//GeometryHelper::CreateRectangle(_geometry, Color{1.f, 1.f, 1.f, 1.f});

	// 버텍스들의 데이터! 이건 CPU에 저장하므로, 아직 CPU의 영역 = RAM에 저장됨.
	_vertexBuffer = std::make_shared<VertexBuffer>(device);
	_vertexBuffer->Create(_geometry->GetVertices());

	_indexBuffer = std::make_shared<IndexBuffer>(device);
	_indexBuffer->Create(_geometry->GetIndices());

	_vertexShader = std::make_shared<VertexShader>(device);
	_vertexShader->Create(L"Default.hlsl", "VS", "vs_5_0");

	// 아직까지도 GPU 입장에서 버텍스 버퍼는 그냥 데이터 쪼가리임. 이걸로 뭘 하라고?를 알려줌
	// 실행 단계와 다르게, 생성에서는 InputLayout은 VS보다 뒤에 만들어야한다. vsBlob이 필요해서...

	_inputLayout = std::make_shared<InputLayout>(device);
	_inputLayout->Create(VertexTextureData::descs, _vertexShader->GetBlob());
	//_inputLayout->Create(VertexColorData::descs, _vertexShader->GetBlob());

	_constantBuffer = std::make_shared<ConstantBuffer<TransformData>>(device, deviceContext);

	_rasterizerState = std::make_shared<RasterizerState>(device);
	_rasterizerState->Create();

	_pixelShader = std::make_shared<PixelShader>(device);
	_pixelShader->Create(L"Default.hlsl", "PS", "ps_5_0");

	_samplerState = std::make_shared<SamplerState>(device);
	_samplerState->Create();

	_textures.resize(2);

	_textures[0] = std::make_shared<Texture>(device);
	_textures[1] = std::make_shared<Texture>(device);

	_textures[0]->Create(L"Skeleton.png");
	_textures[1]->Create(L"Golem.jpg");

	_blendState = std::make_shared<BlendState>(device);
	_blendState->Create();

	_constantBuffer->Create();
}

GameObject::~GameObject()
{

}

void GameObject::Update()
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

void GameObject::Render(std::shared_ptr<Pipeline> pipeline)
{
	{
		PipelineInfo info;
		info.inputLayout = _inputLayout;
		info.vertexShader = _vertexShader;
		info.pixelShader = _pixelShader;
		info.rasterizerState = _rasterizerState;
		info.blendState = _blendState;

		pipeline->UpdatePipeline(info);

		// IA

		pipeline->SetVertexBuffer(_vertexBuffer);

		pipeline->SetIndexbuffer(_indexBuffer);

		// VS

		pipeline->SetConstantBuffer(0, ShaderScope::VertexShader, _constantBuffer);

		// PS

		pipeline->SetTexture(0, ShaderScope::PixelShader, _textures[0]);
		pipeline->SetTexture(1, ShaderScope::PixelShader, _textures[1]);

		pipeline->SetSamplerState(0, ShaderScope::PixelShader, _samplerState);

		// OM
		pipeline->DrawIndexed(_geometry->GetIndexCount(), 0, 0);
	}
}