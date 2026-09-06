#pragma once
#include <d3d11.h>
#include <memory>
#include <vector>
#include <wrl/client.h>
#include "Geometry.h"
#include "VertexData.h"
#include "ConstantBuffer.h"
#include "Struct.h"
#include "Transform.h"

class Pipeline;
class VertexBuffer;
class IndexBuffer;
class InputLayout;

class VertexShader;
class PixelShader;
class Texture;
class RasterizerState;
class SamplerState;
class BlendState;

class Transform;

class GameObject
{
public:
	GameObject(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	~GameObject();

	void Update(bool isParent);

	void Render(std::shared_ptr<Pipeline> pipeline);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> _device;

	// Geometry. 도형 = Mesh 만들기.
	// Geometry는 리소스이다. 각 '객체'가 아닌, '리소스'(메쉬) 단위로 하나씩 들고 있으면 되는 것.
	std::shared_ptr<Geometry<VertexTextureData>> _geometry;
	// 	std::shared_ptr<Geometry<VertexColorData>> _geometry;
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

	// --- 여기 '위'는 '리소스' 마다 같은 부분.
	// --- 여기 '밑'은 '오브젝트' 마다 다른 부분.
	
	// constant buffer
	TransformData _transformData;
	std::shared_ptr<ConstantBuffer<TransformData>> _constantBuffer;
public:
	std::shared_ptr<Transform> _transform = std::make_shared<Transform>();
};

