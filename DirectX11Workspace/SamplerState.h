#pragma once
class SamplerState
{
public:
	SamplerState(ComPtr<ID3D11Device> device);
	~SamplerState();

	const ComPtr<ID3D11SamplerState>& GetComPtr() const { return _samplerState; }

	void Create();

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11SamplerState> _samplerState;
};