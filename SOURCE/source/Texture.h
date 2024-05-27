#ifndef TEXTURE_H
#define TEXTURE_H

struct SDL_Surface;

namespace dae
{
	class Texture
	{
	public:
		~Texture();

		Texture(const Texture&) = delete;
		Texture(Texture&&) noexcept = delete;
		Texture& operator=(const Texture&) = delete;
		Texture& operator=(Texture&&) noexcept = delete;

		ID3D11Device* GetDevice() const;

		ID3D11Texture2D* GetResource() const;
		ID3D11ShaderResourceView* GetSRV() const;

		static Texture* LoadFromFile(ID3D11Device* pDevice, const std::string& path);

	private:
		explicit Texture(ID3D11Device* pDevice, SDL_Surface* pSurface);

		SDL_Surface* m_pSurface;
		uint32_t* m_pSurfacePixels;
		ID3D11Device* m_pDevice;
		ID3D11Texture2D* m_pResource;
		ID3D11ShaderResourceView* m_pSRV;
	};
}

#endif // !TEXTURE_H