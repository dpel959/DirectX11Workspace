#pragma once
#include <d3d11.h>
#include <d3dcommon.h>
#include <vector>
#include <wrl/client.h>
class InputLayout
{
public:
	InputLayout(Microsoft::WRL::ComPtr<ID3D11Device> device);
	~InputLayout();

	const Microsoft::WRL::ComPtr<ID3D11InputLayout>& GetComPtr() const { return _inputLayout; }

	void Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs, Microsoft::WRL::ComPtr<ID3DBlob> blob);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> _device;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout;
};
