#ifndef EFFECT_H
#define EFFECT_H

namespace dae
{
	class Texture;

	class Effect final
	{
	public:
		explicit Effect(ID3D11Device* pDevice, const std::wstring& assertfile);
		~Effect();

		Effect(const Effect&) = delete;
		Effect(Effect&&) noexcept = delete;
		Effect& operator=(const Effect&) = delete;
		Effect& operator=(Effect&&) noexcept = delete;
		
		// Getters / Setters
		const std::wstring& GetFileName() const;
		ID3D11Device* GetDevice() const;
		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;
		ID3D11InputLayout* GetInputLayout() const;
		ID3DX11EffectMatrixVariable* GetWorldMatrix() const;
		ID3DX11EffectMatrixVariable* GetWorldViewProjection() const;
		ID3DX11EffectVectorVariable* GetCameraPos() const;


		void SetDiffusemap(Texture* pDiffuseTexture) const;
		void SetNormalMap(Texture* pNormalMap) const;
		void SetSpecualarMap(Texture* pSpecularMap) const;
		void SetGlossinessMap(Texture* pGlossinessMap) const;

	private:
		const std::wstring m_FileName;

		ID3D11Device* m_pDevice;
		ID3DX11Effect* m_pEffect;
		ID3DX11EffectTechnique* m_pTechnique;
		ID3D11InputLayout* m_pInputLayout;
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
		ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable;
		ID3DX11EffectShaderResourceVariable* m_pDiffusedMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;
		ID3DX11EffectVectorVariable* m_pCameraPositionVariable;

		// loading
		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assertfile);
	};
}

#endif // !EFFECT_H