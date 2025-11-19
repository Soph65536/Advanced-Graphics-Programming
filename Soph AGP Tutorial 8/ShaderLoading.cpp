#include "ShaderLoading.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#include "ReadData.h"
#include "Debug.h"

namespace ShaderLoading {

	int ReflectVShaderInputLayout(std::vector<uint8_t>& vShaderBytecode,
		ID3D11Device* dev, ID3D11InputLayout** outIL) {

		HRESULT hr;

		ID3D11ShaderReflection* vShaderReflection = NULL;
		D3DReflect(vShaderBytecode.data(), vShaderBytecode.size(), IID_ID3D11ShaderReflection, (void**)&vShaderReflection);

		D3D11_SHADER_DESC desc;
		hr = vShaderReflection->GetDesc(&desc);
		if (FAILED(hr)) {
			LOG("Failed to get Vertex Shader Reflection Description.");
			return hr;
		}

		D3D11_SIGNATURE_PARAMETER_DESC* signatureParamDescriptions = new D3D11_SIGNATURE_PARAMETER_DESC[desc.InputParameters]{ 0 };
		for (UINT i = 0; i < desc.InputParameters; i++) {
			hr = vShaderReflection->GetInputParameterDesc(i, &signatureParamDescriptions[i]);
			if (FAILED(hr)) {
				LOG("Failed to get Input parameter description for vertex shader.");
				return hr;
			}
		}

		//signature param desc contains bitmasks, and put input layout together from shader
		D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[desc.InputParameters]{ 0 };
		for (size_t i = 0; i < desc.InputParameters; i++) {

			ied[i].SemanticName = signatureParamDescriptions[i].SemanticName;
			ied[i].SemanticIndex = signatureParamDescriptions[i].SemanticIndex;

			if (signatureParamDescriptions[i].ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
				//only covers x, yx, zyx, wzyx
				switch (signatureParamDescriptions[i].Mask) {
				case 1:
					ied[i].Format = DXGI_FORMAT_R32_FLOAT;
					break;
				case 3:
					ied[i].Format = DXGI_FORMAT_R32G32_FLOAT;
					break;
				case 7:
					ied[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
					break;
				case 15:
					ied[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					break;
				default:
					break;
				}
			}

			ied[i].InputSlot = 0;
			ied[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			ied[i].InstanceDataStepRate = 0;
		}

		hr = dev->CreateInputLayout(ied, desc.InputParameters, vShaderBytecode.data(), vShaderBytecode.size(), outIL);
		if (FAILED(hr)) {
			LOG("Failed to create input layout.");
			return hr;
		}

		delete[] signatureParamDescriptions;
		delete[] ied;

		return S_OK;
	}

	long LoadVertexShader(std::string filename, ID3D11Device* dev,
		ID3D11VertexShader** outVS, ID3D11InputLayout** outIL) {
		
		HRESULT hr;

		auto shaderBytecode = DX::ReadData(std::wstring(filename.begin(), filename.end()).c_str());
		hr = dev->CreateVertexShader(
			shaderBytecode.data(), shaderBytecode.size(), NULL, outVS);
		if (FAILED(hr)) {
			LOG("Failed to load vertex shader" + filename + ".");
			return hr;
		}

		hr = ReflectVShaderInputLayout(shaderBytecode, dev, outIL);
		if (FAILED(hr)) {
			LOG("Failed to reflect vertex shader" + filename + ".");
			return hr;
		}
		
		return S_OK;
	}

	long LoadFragmentShader(std::string filename, ID3D11Device* dev, 
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