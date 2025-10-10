#include "ShaderLoading.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#include "ReadData.h"
#include "Debug.h"

namespace ShaderLoading {
	long LoadVertexShader(std::string filename, ID3D11Device* dev,
		ID3D11VertexShader** outVS, ID3D11InputLayout** outIL) {
		
		auto shaderBytecode = DX::ReadData(std::wstring(filename.begin(), filename.end()).c_str());
		HRESULT hr = dev->CreateVertexShader(
			shaderBytecode.data(), shaderBytecode.size(), NULL, outVS);

		if (FAILED(hr)) {
			LOG("Failed to load vertex shader" + filename + ".");
			return hr;
		}
		
		return S_OK;
	}

	long LoadPixelShader(std::string filename, ID3D11Device* dev, 
		ID3D11PixelShader** outPS) {

		auto shaderBytecode = DX::ReadData(std::wstring(filename.begin(), filename.end()).c_str());
		HRESULT hr = dev->CreatePixelShader(
			shaderBytecode.data(), shaderBytecode.size(), NULL, outPS);

		if (FAILED(hr)) {
			LOG("Failed to load pixel shader" + filename + ".");
			return hr;
		}

		return S_OK;
	}
}