#include "pch.h"
#include "FireEffect.h"
#include "Texture.h"

namespace dae
{
	FireEffect::FireEffect(ID3D11Device* pDevice, const std::wstring& assertfile)
		: BaseEffect{ pDevice, assertfile }
	{
		// DiffusedMap
		m_pDiffusedMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffusedMapVariable->IsValid())
		{
			std::wcout << L"m_pDiffusemapVariable not valid!\n";
		}

		// Create Vertex Layout
		static constexpr uint32_t numElements{ 2 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "TEXCOORD";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

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

	FireEffect::~FireEffect()
	{
		if (m_pDiffusedMapVariable) m_pDiffusedMapVariable->Release();
	}

	void FireEffect::SetDiffusemap(Texture* pDiffuseTexture) const
	{
		if (m_pDiffusedMapVariable)
		{
			m_pDiffusedMapVariable->SetResource(pDiffuseTexture->GetSRV());
		}
	}
}