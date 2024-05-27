#include "pch.h"
#include "VehicleEffect.h"
#include "Texture.h"

namespace dae
{
	VehicleEffect::VehicleEffect(ID3D11Device* pDevice, const std::wstring& assertfile)
		: BaseEffect{pDevice, assertfile }
	{
		// Get World Matrix
		m_pWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
		if (!m_pWorldMatrixVariable)
		{
			std::wcout << L"m_pWorldMatrixVariable not valid!\n";
		}

		// Get Camera Position
		m_pCameraPositionVariable = m_pEffect->GetVariableByName("gCameraPos")->AsVector();
		if (!m_pCameraPositionVariable)
		{
			std::wcout << L"m_pCameraPositionVariable not valid!\n";
		}

		// Get Shader Resource Maps 
		// DiffusedMap
		m_pDiffusedMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffusedMapVariable->IsValid())
		{
			std::wcout << L"m_pDiffusemapVariable not valid!\n";
		}
		// NormalMap
		m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
		if (!m_pNormalMapVariable->IsValid())
		{
			std::wcout << L"m_pNormalMapVariable not valid!\n";
		}
		// SpecularMap
		m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
		if (!m_pSpecularMapVariable->IsValid())
		{
			std::wcout << L"m_pSpecularMapVariable not valid!\n";
		}
		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
		if (!m_pGlossinessMapVariable->IsValid())
		{
			std::wcout << L"m_pGlossinessMapVariable not valid!\n";
		}

		// Create Vertex Layout
		static constexpr uint32_t numElements{ 4 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "TEXCOORD";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "NORMAL";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[2].AlignedByteOffset = 20;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[3].SemanticName = "TANGENT";
		vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[3].AlignedByteOffset = 32;
		vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		// Create Input Layout
		D3DX11_PASS_DESC passDesc{};
		m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

		HRESULT result
		{
			pDevice->CreateInputLayout
			(
				vertexDesc,
				numElements,
				passDesc.pIAInputSignature,
				passDesc.IAInputSignatureSize,
				&m_pInputLayout
			)
		};
		if (FAILED(result))
		{
			assert(false);
			return;
		}
	}

	VehicleEffect::~VehicleEffect()
	{
		if (m_pWorldMatrixVariable) m_pWorldMatrixVariable->Release();
		if (m_pDiffusedMapVariable) m_pDiffusedMapVariable->Release();
		if (m_pNormalMapVariable) m_pNormalMapVariable->Release();
		if (m_pSpecularMapVariable) m_pSpecularMapVariable->Release();
		if (m_pGlossinessMapVariable) m_pGlossinessMapVariable->Release();
		if (m_pCameraPositionVariable) m_pCameraPositionVariable->Release();
	}

	void VehicleEffect::SetDiffusemap(Texture* pDiffuseTexture) const
	{
		if (m_pDiffusedMapVariable)
		{
			m_pDiffusedMapVariable->SetResource(pDiffuseTexture->GetSRV());
		}
	}

	void VehicleEffect::SetNormalMap(Texture* pNormalMap) const
	{
		if (m_pNormalMapVariable)
		{
			m_pNormalMapVariable->SetResource(pNormalMap->GetSRV());
		}
	}

	void VehicleEffect::SetSpecualarMap(Texture* pSpecularMap) const
	{
		if (m_pSpecularMapVariable)
		{
			m_pSpecularMapVariable->SetResource(pSpecularMap->GetSRV());
		}
	}

	void VehicleEffect::SetGlossinessMap(Texture* pGlossinessMap) const
	{
		if (m_pGlossinessMapVariable)
		{
			m_pGlossinessMapVariable->SetResource(pGlossinessMap->GetSRV());
		}
	}

	ID3DX11EffectMatrixVariable* VehicleEffect::GetWorldMatrix() const
	{
		return m_pWorldMatrixVariable;
	}

	ID3DX11EffectVectorVariable* VehicleEffect::GetCameraPos() const
	{
		return m_pCameraPositionVariable;
	}
}
