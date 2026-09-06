#pragma once
#include <d3d11.h>
#include <string>
#include <wrl/client.h>

class Texture
{
public:
	Texture(Microsoft::WRL::ComPtr<ID3D11Device> device);
	~Texture();

	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetComPtr() const { return _shaderResourceView; }

	void Create(const std::wstring& path);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> _device;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
};

