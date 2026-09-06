#pragma once
#include <string>

class Texture
{
public:
	Texture(ComPtr<ID3D11Device> device);
	~Texture();

	const ComPtr<ID3D11ShaderResourceView>& GetComPtr() const { return _shaderResourceView; }

	void Create(const std::wstring& path);

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
};

