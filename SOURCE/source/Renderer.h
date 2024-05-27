#ifndef RENDERER_H
#define RENDERER_H

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Camera;
	class Texture;
	class VehicleEffect;
	class FireEffect;

	template<typename EffectClass>
	class Mesh;

	enum class FilteringMode;

	class Renderer final
	{
	public:

		explicit Renderer(SDL_Window* pWindow, int width, int height);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void ToggleFilteringMode();
		void ToggleRotating();
		void ToggleNormalMap();
		void ToggleFireFX();

		void Update(const Timer* const pTimer);
		void Render() const;

	private:

		SDL_Window* m_pWindow;
		const int m_Width;
		const int m_Height;
		bool m_IsInitialized;

		FilteringMode m_CurrentFileringMode;

		// DIRECTX INIT// 
		HRESULT InitializeDirectX();
		void ReleaseDirectXResources();
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;
		////////

		bool m_ShowFireFX;

		Mesh<VehicleEffect>* m_pVehicleMesh;
		Mesh<FireEffect>* m_pFireMesh;
		Camera* m_pCamera;

		Texture* m_pVechicleDiffusedMap;
		Texture* m_pNormalMap;
		Texture* m_pSpecularMap;
		Texture* m_pGlossinessMap;

		Texture* m_pFireDiffusedMap;

		bool m_MeshRotating;
		float m_RotateAngle;
		const float m_MeshRotationSpeed;
		Matrix m_TranslationMatrix;
		Matrix m_RotationMatrix;
		Matrix m_WorldMatrix;
		Matrix m_WorldViewProjectionMatrix;

		void InitMesh();
	};
}

#endif // !RENDERER_H