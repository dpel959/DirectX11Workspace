#pragma once
#include <d3d11.h>
#include <wrl/client.h>
class SamplerState
{
public:
	SamplerState(Microsoft::WRL::ComPtr<ID3D11Device> device);
	~SamplerState();

	const Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetComPtr() const { return _samplerState; }

	void Create();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> _device;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> _samplerState;
};
