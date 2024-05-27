#include "pch.h"
#include "Effect.h"
#include "Texture.h"

namespace dae
{
	Effect::Effect(ID3D11Device* pDevice, const std::wstring& assertfile)
		: m_pDevice{ pDevice }
		, m_FileName{ assertfile }
	{
		assert(pDevice);
		assert(assertfile.size() > 0);

		// Load Effect
		m_pEffect = LoadEffect(pDevice, assertfile);

		// Get Technique
		m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
		if (!m_pTechnique->IsValid())
		{
			std::wcout << L"Technique not valid\n";
		}

		// Get World View Projection
		m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		if (!m_pMatWorldViewProjVariable->IsValid())
		{
			std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";
		}

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

	Effect::~Effect()
	{
		if (m_pTechnique) m_pTechnique->Release();
		if (m_pEffect) m_pEffect->Release();
		if (m_pInputLayout) m_pInputLayout->Release();
	}

	const std::wstring& Effect::GetFileName() const
	{
		return m_FileName;
	}

	ID3D11Device* Effect::GetDevice() const
	{
		return m_pDevice;
	}

	ID3DX11Effect* Effect::GetEffect() const
	{
		return m_pEffect;
	}

	ID3DX11EffectTechnique* Effect::GetTechnique() const
	{
		return m_pTechnique;
	}

	ID3D11InputLayout* Effect::GetInputLayout() const
	{
		return m_pInputLayout;
	}

	ID3DX11EffectMatrixVariable* Effect::GetWorldMatrix() const
	{
		return m_pWorldMatrixVariable;
	}

	ID3DX11EffectMatrixVariable* Effect::GetWorldViewProjection() const
	{
		return m_pMatWorldViewProjVariable;
	}

	ID3DX11EffectVectorVariable* Effect::GetCameraPos() const
	{
		return m_pCameraPositionVariable;
	}

	void Effect::SetDiffusemap(Texture* pDiffuseTexture) const
	{
		if (m_pDiffusedMapVariable)
		{
			m_pDiffusedMapVariable->SetResource(pDiffuseTexture->GetSRV());
		}
	}

	void Effect::SetNormalMap(Texture* pNormalMap) const
	{
		if (m_pNormalMapVariable)
		{
			m_pNormalMapVariable->SetResource(pNormalMap->GetSRV());
		}
	}

	void Effect::SetSpecualarMap(Texture* pSpecularMap) const
	{
		if (m_pSpecularMapVariable)
		{
			m_pNormalMapVariable->SetResource(pSpecularMap->GetSRV());
		}
	}

	void Effect::SetGlossinessMap(Texture* pGlossinessMap) const
	{
		if (m_pGlossinessMapVariable)
		{
			m_pGlossinessMapVariable->SetResource(pGlossinessMap->GetSRV());
		}
	}

	ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assertfile)
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{ nullptr };
		ID3DX11Effect* pEffect;

		DWORD shaderFlags{ 0 };

#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // defined( DEBUG ) || defined( _DEBUG )

		result = D3DX11CompileEffectFromFile
		(
			static_cast<LPCWSTR>(assertfile.c_str()),
			nullptr,
			nullptr,
			shaderFlags,
			0,
			pDevice,
			&pEffect,
			&pErrorBlob
		);

		if (FAILED(result))
		{
			if (pErrorBlob)
			{
				const char* pErrors{ static_cast<char*>(pErrorBlob->GetBufferPointer()) };

				std::wstringstream ss;
				for (unsigned int idx{}; idx < pErrorBlob->GetBufferSize(); ++idx)
				{
					ss << pErrors[idx];
				}

				OutputDebugStringW(ss.str().c_str());
				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << "\n";
			}
			else
			{
				std::wstringstream ss;
				ss << "EFFECT::EFFECTLOADER::Failed to CreateEffectFromFile\tPath: " << assertfile;
				std::wcout << ss.str() << "\n";
				return nullptr;
			}
		}

		return pEffect;
	}
}