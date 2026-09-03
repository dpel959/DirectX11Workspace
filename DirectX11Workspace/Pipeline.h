#pragma once
#include <memory>
#include "ConstantBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "SamplerState.h"

// 굳이 렌더링 파이프라인을 클래스로 분리하는 이유는, 필요마다 렌더링을 다르게 해주기 위해서이다.
// 즉, 물체마다의 '나는 이렇게 그려지고 싶어요'의 정보.
// 
// 그리고, 이 struct가 그것을 나누는 역할을 한다. 보통 쉐이더는 오브젝트들이 공유해서 사용 하고는 한다. 
// 상용 엔진의 쉐이더를 사용하면 공용 쉐이더 하나를 대부분의 오브젝트가 쓰는 경우도 흔하고.

class InputLayout;
class VertexShader;
class RasterizerState;
class PixelShader;
class BlendState;

// 무조건적으로 사용하는 공용 자원들을 PipelineInfo에 들고 있는다.

struct PipelineInfo
{
	std::shared_ptr<InputLayout> inputLayout;
	std::shared_ptr<VertexShader> vertexShader;
	std::shared_ptr<PixelShader> pixelShader;
	std::shared_ptr<RasterizerState> rasterizerState;
	std::shared_ptr<BlendState> blendState;
	D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

class Pipeline
{
public:
	Pipeline(ComPtr<ID3D11DeviceContext> _deviceContext);
	~Pipeline();

	void UpdatePipeline(PipelineInfo info);

	// 이 밑 사항들이 PipelineInfo에 들어가 있지 않은 이유는, 필수가 아닌 선택적 사항이기 때문이다.
	void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer);
	void SetIndexbuffer(const std::shared_ptr<IndexBuffer>& buffer);

	// slot에 _constantBuffer의 Buffer 내용 등록! 
	// ShaderScope 가 필요한 이유. ConstantBuffer은 VS에, 혹은 PS에 들어갈 수도 있으므로 그걸 걸러낸다.
	template<typename T>
	void SetConstantBuffer(uint32 slot, ShaderScope scope, const std::shared_ptr<ConstantBuffer<T>>& buffer)
	{
		if ((scope & ShaderScope::VertexShader) != ShaderScope::None)
		{
			_deviceContext->VSSetConstantBuffers(slot, 1, buffer->GetComPtr().GetAddressOf());
		}
		else if ((scope & ShaderScope::PixelShader) != ShaderScope::None)
		{
			_deviceContext->PSSetConstantBuffers(slot, 1, buffer->GetComPtr().GetAddressOf());
		}

	}

	void SetTexture(uint32 slot, ShaderScope scope, const std::shared_ptr<Texture>& texture)
	{
		if ((scope & ShaderScope::VertexShader) != ShaderScope::None)
		{
			_deviceContext->VSSetShaderResources(slot, 1, texture->GetComPtr().GetAddressOf());// 맨 앞은 셰이더 인덱스. (t0, t1)
		}
		else if ((scope & ShaderScope::PixelShader) != ShaderScope::None)
		{
			_deviceContext->PSSetShaderResources(slot, 1, texture->GetComPtr().GetAddressOf());// 맨 앞은 셰이더 인덱스. (t0, t1)
		}
	}

	void SetSamplerState(uint32 slot, ShaderScope scope, const std::shared_ptr<SamplerState>& samplerState)
	{
		if ((scope & ShaderScope::VertexShader) != ShaderScope::None)
		{
			_deviceContext->VSSetSamplers(0, 1, samplerState->GetComPtr().GetAddressOf());
		}
		else if ((scope & ShaderScope::PixelShader) != ShaderScope::None)
		{
			_deviceContext->PSSetSamplers(0, 1, samplerState->GetComPtr().GetAddressOf());
		}
	}

	void Draw(uint32 vertexCount, uint32 startVertexLocation)
	{
		_deviceContext->Draw(vertexCount, startVertexLocation); // 정점만 사용해서 Draw
	}

	void DrawIndexed(uint32 indexCount, uint32 startindexLocation, uint32 baseVertexLocation)
	{
		_deviceContext->DrawIndexed(indexCount, startindexLocation, baseVertexLocation); // 인덱스 정보를 사용해서 Draw
	}

private:
	ComPtr<ID3D11DeviceContext> _deviceContext;
};

