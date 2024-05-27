#ifndef VECHILEEFFECT_H
#define VECHILEEFFECT_H

#include "BaseEffect.h"

namespace dae
{
	class Texture;

	class VehicleEffect final : public BaseEffect
	{
	public:
		explicit VehicleEffect(ID3D11Device* pDevice, const std::wstring& assertfile);
		virtual ~VehicleEffect();

		VehicleEffect(const VehicleEffect&) = delete;
		VehicleEffect(VehicleEffect&&) noexcept = delete;
		VehicleEffect& operator=(const VehicleEffect&) = delete;
		VehicleEffect& operator=(VehicleEffect&&) noexcept = delete;

		void SetDiffusemap(Texture* pDiffuseTexture) const;
		void SetNormalMap(Texture* pNormalMap) const;
		void SetSpecualarMap(Texture* pSpecularMap) const;
		void SetGlossinessMap(Texture* pGlossinessMap) const;

		ID3DX11EffectMatrixVariable* GetWorldMatrix() const;
		ID3DX11EffectVectorVariable* GetCameraPos() const;

	private:
		ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable;
		ID3DX11EffectShaderResourceVariable* m_pDiffusedMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;
		ID3DX11EffectVectorVariable* m_pCameraPositionVariable;

	};
}

#endif // !VECHILEEFFECT_H
