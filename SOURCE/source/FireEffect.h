#ifndef FIREEFFECT_H
#define FIREEFFECT_H

#include "BaseEffect.h"

struct ID3DX11EffectShaderResourceVariable;

namespace dae
{
	class Texture;

	class FireEffect final : public BaseEffect
	{
	public:
		explicit FireEffect(ID3D11Device* pDevice, const std::wstring& assertfile);
		virtual ~FireEffect();

		FireEffect(const FireEffect&) = delete;
		FireEffect(FireEffect&&) noexcept = delete;
		FireEffect& operator=(const FireEffect&) = delete;
		FireEffect& operator=(FireEffect&&) noexcept = delete;

		void SetDiffusemap(Texture* pDiffuseTexture) const;

	private:
		ID3DX11EffectShaderResourceVariable* m_pDiffusedMapVariable;
	};
}

#endif // !FIREEFFECT_H