#include "pch.h"
#include "BaseEffect.h"

namespace dae
{
	BaseEffect::BaseEffect(ID3D11Device* pDevice, const std::wstring& assertfile)
		: m_pDevice{ pDevice }
		, m_FileName{ assertfile }
	{
		assert(pDevice);
		assert(assertfile.size() > 0);

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
	}

	BaseEffect::~BaseEffect()
	{
		if (m_pMatWorldViewProjVariable) m_pMatWorldViewProjVariable->Release();
		if (m_pTechnique) m_pTechnique->Release();
		if (m_pEffect) m_pEffect->Release();
		if (m_pInputLayout) m_pInputLayout->Release();
	}

	ID3D11Device* BaseEffect::GetDevice() const
	{
		return m_pDevice;
	}

	const std::wstring& BaseEffect::GetFileName() const
	{
		return m_FileName;
	}

	ID3DX11Effect* BaseEffect::GetEffect() const
	{
		return m_pEffect;
	}

	ID3DX11EffectTechnique* BaseEffect::GetTechnique() const
	{
		return m_pTechnique;
	}

	ID3D11InputLayout* BaseEffect::GetInputLayout() const
	{
		return m_pInputLayout;
	}

	ID3DX11EffectMatrixVariable* BaseEffect::GetWorldViewProjectionMatrix() const
	{
		return m_pMatWorldViewProjVariable;
	}

	ID3DX11Effect* BaseEffect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assertfile)
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