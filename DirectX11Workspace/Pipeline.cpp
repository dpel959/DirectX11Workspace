#include "pch.h"
#include "Pipeline.h"
#include "RasterizerState.h"
#include "BlendState.h"

Pipeline::Pipeline(ComPtr<ID3D11DeviceContext> _deviceContext)
	:_deviceContext(_deviceContext)
{
}

Pipeline::~Pipeline()
{
}

// 공통적인 부분은 PipelineInfo로 채워주고, 나머지 선택적 작업은 필요할때 채워준다.
void Pipeline::UpdatePipeline(PipelineInfo info)
{
	// 실제 그리는 작업.
	// IA - VS - RS - PS - OM

	// IA

	// '정점 하나'에 들어간 데이터는 어떤 식으로 해석해?
	_deviceContext->IASetInputLayout(info.inputLayout->GetComPtr().Get());
	// 자 읽어주신 데이터는 삼각형으로 인식해주세요.
	_deviceContext->IASetPrimitiveTopology(info.topology);

	// VS

	// GPU 네가 이 셰이더로 일했으면 좋겠어
	if (info.vertexShader)
	{
		_deviceContext->VSSetShader(info.vertexShader->GetComPtr().Get(), nullptr, 0);
	}

	// RS
	if (info.rasterizerState)
	{
		_deviceContext->RSSetState(info.rasterizerState->GetComPtr().Get());
	}

	// PS
	if (info.pixelShader)
	{
		_deviceContext->PSSetShader(info.pixelShader->GetComPtr().Get(), nullptr, 0);
	}


	// OM
	if (info.blendState)
	{
		_deviceContext->OMSetBlendState(info.blendState->GetComPtr().Get(), info.blendState->GetBlendFactor(), info.blendState->GetSampleMask());
	}

}

// 어떤 버텍스 버퍼를 세팅해줄거야?
void Pipeline::SetVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer)
{
	uint32 stride = buffer->GetStride();
	uint32 offset = buffer->GetOffset();
	_deviceContext->IASetVertexBuffers(0, 1, buffer->GetComPtr().GetAddressOf(), &stride, &offset);
}

// 그 버텍스들 어떤 순서로 읽을 거야? = 인덱스 버퍼 세팅
void Pipeline::SetIndexbuffer(const std::shared_ptr<IndexBuffer>& buffer)
{
	_deviceContext->IASetIndexBuffer(buffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
}
