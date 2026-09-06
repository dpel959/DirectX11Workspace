#include "pch.h"
#include "BlendState.h"

BlendState::BlendState(Microsoft::WRL::ComPtr<ID3D11Device> device)
	:_device(device)
{
}

BlendState::~BlendState()
{
}

void BlendState::Create(D3D11_RENDER_TARGET_BLEND_DESC blendDesc, float factor)
{
	for (float& blendFactor : _blendFactor)
	{
		blendFactor = factor;
	}

	D3D11_BLEND_DESC desc;
	::ZeroMemory(&desc, sizeof(desc));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0] = blendDesc;

	HRESULT hr = _device->CreateBlendState(&desc, _blendState.GetAddressOf());
	assert(SUCCEEDED(hr));
}

// 위에 코드가 뭘 한 것이냐면,
#if 0

// 텍스처의 Alpha 값에 따라, 어떻게 섞여야 할 것인지
void Game::CreateBlendState()
{
	D3D11_BLEND_DESC desc;
	::ZeroMemory(&desc, sizeof(desc));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // Src = 새로 만드는 픽셀에 Src의 알파값 만큼을 투명도를 곱하겠다
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // Dest = 원래 있었던 픽셀에 (1 - Src의 알파값) 만큼을 투명도에 곱하곘다.
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // 그걸 더하겠다. 일반적인 알파 블렌딩. 
	//보통 이펙트에 사용하는 가산 블렌딩은 SrcBlend와 DestBlend를 둘 다 D3D11_BLEND_ONE = 모두 그대로 적용.으로 설정한다. 빛 유지 + 새로운 빛 발광 = 즉 이펙트 등에 많이 사용한다.
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = _graphics->GetDevice()->CreateBlendState(&desc, _blendState.GetAddressOf());
	assert(SUCCEEDED(hr));
}

#endif
