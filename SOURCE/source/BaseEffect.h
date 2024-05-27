#ifndef BASEEFFECT_H
#define BASEEFFECT_H

namespace dae
{
	class BaseEffect
	{
	public:
		explicit BaseEffect(ID3D11Device* pDevice, const std::wstring& assertfile);
		virtual ~BaseEffect();

		BaseEffect(const BaseEffect&) = delete;
		BaseEffect(BaseEffect&&) noexcept = delete;
		BaseEffect& operator=(const BaseEffect&) = delete;
		BaseEffect& operator=(BaseEffect&&) noexcept = delete;

		ID3D11Device* GetDevice() const;
		const std::wstring& GetFileName() const;
		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;
		ID3D11InputLayout* GetInputLayout() const;
		ID3DX11EffectMatrixVariable* GetWorldViewProjectionMatrix() const;

	protected:
		ID3DX11Effect* m_pEffect;
		ID3D11InputLayout* m_pInputLayout;
		ID3DX11EffectTechnique* m_pTechnique;

	private:
		const std::wstring m_FileName;
		ID3D11Device* m_pDevice;
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;

		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assertfile);
	};
}

#endif // !BASEEFFECT_H