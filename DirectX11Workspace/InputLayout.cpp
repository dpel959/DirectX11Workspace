#include "pch.h"
#include "InputLayout.h"
#include "Types.h"

InputLayout::InputLayout(Microsoft::WRL::ComPtr<ID3D11Device> device)
	:_device(device)
{
}

InputLayout::~InputLayout()
{
}

// 여기서 만드는 DESC와 struct로 만든 vertex, HLSL의 VS가 전부 맞아야한다.
// 이번에는 desc를 따로 vector로 받았다. 이유는 '버텍스마다 모두 구성이 천차만별이기 때문'이므로. 그것들을 각각 모두 충족시켜주기 위해서이다.
// 거기에, 배열을 인자로 받아봐야 포인터로 넘어온다. 그러므로 sizeof로 크기를 참조할 수 없으므로 std::vector가 필요하다. (size 같이 넘겨줘도 되긴 하는데. 굳이?)
void InputLayout::Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs, Microsoft::WRL::ComPtr<ID3DBlob> blob)
{
	const int32 count = static_cast<int32>(descs.size());

	// InputLayout = "GPU가 데이터를 어떻게 읽어야할지 알려주는 번역기"
	// _vsBlob을 왜 필요로 하냐면, ELEMENT_DESC와 struct VS_INPUT이 서로 일치하나 '사전 검증'을 필요로 함.
	HRESULT hr = _device->CreateInputLayout(descs.data(), count, blob->GetBufferPointer(), blob->GetBufferSize(), _inputLayout.GetAddressOf());
	assert(SUCCEEDED(hr));
}
