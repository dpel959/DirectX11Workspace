#include "pch.h"
#include "Texture.h"

Texture::Texture(ComPtr<ID3D11Device> device)
	:_device(device)
{
}

Texture::~Texture()
{
}

// ShaderResourceView. 텍스처 사용에 필요함. 이미지를 받아오는 방법, 라이브러리는 여러가지 있지만, DirectXTex를 사용한다.
// RTV는 여기다 쓰세요 - OM 단계 쓰기 전용이고, SRV는 텍스처 가져왔으니 쓰세요 - PS 단계 읽기 전용 (자원)이다.
// RTV나 SRV나 Texture2D를 사용하고, 이 Texture2D는 이미지, 백버퍼 등이 될 수 있다. 실제로 둘 다 RTV, SRV에 사용할 수 있다. (CCTV, 미니맵 등)
// 유명한 Render-to-texture라는 기술이 있는데, 처음 파이프라인에 RTV로 그리고, 다음 파이프라인에 SRV로 덮어씌워 본 렌더링 위에 덮는 미니맵/후처리 등을 쓰기도 한다.
void Texture::Create(const std::wstring& path)
{
	DirectX::TexMetadata md; // width, height, detph 등 이미지의 스펙 정보
	DirectX::ScratchImage img; // 실제 픽셀 알맹이(바이트 배열) 정보

	HRESULT hr = ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &md, img);

	assert(SUCCEEDED(hr));

	hr = CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));
}
