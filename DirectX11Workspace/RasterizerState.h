#pragma once
#include <d3d11.h>
#include <wrl/client.h>
class RasterizerState
{
public:
	RasterizerState(Microsoft::WRL::ComPtr<ID3D11Device> device);
	~RasterizerState();

	const Microsoft::WRL::ComPtr<ID3D11RasterizerState>& GetComPtr() const { return _rasterizerState; }

	void Create();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> _device;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> _rasterizerState;
};
