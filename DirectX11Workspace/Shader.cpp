#include "pch.h"
#include "Shader.h"

Shader::Shader(ComPtr<ID3D11Device> device)
	:_device(device)
{
}

Shader::~Shader()
{
}

void Shader::LoadShaderFromFile(const std::wstring& path, const std::string& name, const std::string& version)
{
	_path = path;
	_name = name;

	// 디버그 모드고, 최적화 건너뛰겠다.
	const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	//d3dcompiler의 함수임. d3d11 기본 제공이 아님
	HRESULT hr = ::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		_blob.GetAddressOf(),
		nullptr
	);

	assert(SUCCEEDED(hr));
}

VertexShader::VertexShader(ComPtr<ID3D11Device> device)
	:Super(device)
{
}

VertexShader::~VertexShader()
{
}

// 파일로 존재하던 셰이더를, 메모리에 들고 동작을 시킨다
// 딱 흐름을 정리하자면, InputLayout에서 ELEMENT_DESC 생성(C++) -> HLSL 짜기 -> C++로 돌아와 D3DCompileFromFile로 HLSL 로드
// -> Blob으로 셰이더 파일 로드, Blob을 통해 셰이더를 C++에서 생성 -> 다시 InputLayout으로 와 CreateInputLayout
void VertexShader::Create(const std::wstring& path, const std::string& name, const std::string& version)
{
	// 파일에서 셰이더 정보를 blob으로 끌어온다!
	LoadShaderFromFile(path, name, version); // 셰이더 이름, 셰이더 종류, 셰이더 버전, 받을 블롭

	// blob을 통해 버텍스 셰이더를 만든다! 굳이 blob을 거쳐서 만드는 이유는, 나중에 InputLayout을 만들때도 blob이 필요하기 때문. (검증용)
	HRESULT hr = _device->CreateVertexShader(_blob->GetBufferPointer(),
		_blob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());

	assert(SUCCEEDED(hr));
}

PixelShader::PixelShader(ComPtr<ID3D11Device> device)
	:Super(device)
{
}

PixelShader::~PixelShader()
{
}

void PixelShader::Create(const std::wstring& path, const std::string& name, const std::string& version)
{
	LoadShaderFromFile(path, name, version);

	HRESULT hr = _device->CreatePixelShader(_blob->GetBufferPointer(),
		_blob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());

	assert(SUCCEEDED(hr));
}
