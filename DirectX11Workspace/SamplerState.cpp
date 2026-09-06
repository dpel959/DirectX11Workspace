#include "pch.h"
#include "SamplerState.h"

SamplerState::SamplerState(ComPtr<ID3D11Device> device)
	:_device(device)
{
}

SamplerState::~SamplerState()
{
}

// 그래서, 이 Sampler가 하는 것이 무엇이냐? 텍스처의 UV좌표는 0.0f~1.0f이다.
// 그런데 넣어진 값이 '범위를 벗어나면, 그 값을 어떻게 반영할 것인가?'를 담당한다.
// 그것을 어떻게 담당할지를 'ADDRESS MODE'라고 한다.
void SamplerState::Create()
{
	D3D11_SAMPLER_DESC desc;
	::ZeroMemory(&desc, sizeof(desc));

	// BORDER: 밑에서 설정한 BorderColor로 채운다. 스나이퍼 조준경 밖 암전 처리
	// MIRROR: UV 좌표 값을 거울에 대칭시키듯 채운다. 
	// WRAP: UV 좌표 값을 반복 적용한다. 타일 등에 이용.
	// CLAMP: 가장자리 픽셀을 쭉 늘린다. UI 아이콘, 스카이박스 등에 이용
	// MIRROR_ONCE: 0,0을 기준으로 딱 한번만 거울 반사하고, 나머지는 CLAMP한다. 나비 같이 완벽한 좌우 대칭 등에 이용
	desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

	// BorderColor는 순서대로 RGBA이다.
	desc.BorderColor[0] = 1;
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 1;

	// 일반 Texture2D.Sample은 값을 비교하지 않고 보간하는 일반 필터를 사용한다.
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 16;
	desc.MaxLOD = (std::numeric_limits<float>::max)();
	desc.MinLOD = (std::numeric_limits<float>::min)();
	desc.MipLODBias = 0.0f;

	HRESULT hr = _device->CreateSamplerState(&desc, _samplerState.GetAddressOf());

	assert(SUCCEEDED(hr));
}
