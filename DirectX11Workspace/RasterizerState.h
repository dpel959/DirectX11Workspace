#pragma once
class RasterizerState
{
public:
	RasterizerState(ComPtr<ID3D11Device> device);
	~RasterizerState();

	const ComPtr<ID3D11RasterizerState>& GetComPtr() const { return _rasterizerState; }

	void Create();

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11RasterizerState> _rasterizerState;
};