#include "pch.h"
#include "Texture.h"

namespace dae
{
	Texture::Texture(ID3D11Device* pDevice, SDL_Surface* pSurface)
		: m_pDevice{ pDevice }
		, m_pSurface{ pSurface }
		, m_pSurfacePixels{ static_cast<uint32_t*>(pSurface->pixels) }
		, m_pResource{ nullptr }
		, m_pSRV{ nullptr }
	{
		DXGI_FORMAT format{ DXGI_FORMAT_R8G8B8A8_UNORM };
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = pSurface->w;
		desc.Height = pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);
		HRESULT result{ pDevice->CreateTexture2D(&desc, &initData, &m_pResource) };
		if (FAILED(result))
		{
			assert(false);
			return;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		result = pDevice->CreateShaderResourceView(m_pResource, &SRVDesc, &m_pSRV);
		if (FAILED(result))
		{
			assert(false);
			return;
		}
	}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
		}
		if (m_pResource) m_pResource->Release();
		if (m_pSRV) m_pSRV->Release();
	}

	ID3D11Device* Texture::GetDevice() const
	{
		return m_pDevice;
	}

	ID3D11Texture2D* Texture::GetResource() const
	{
		return m_pResource;
	}

	ID3D11ShaderResourceView* Texture::GetSRV() const
	{
		return m_pSRV;
	}

	Texture* Texture::LoadFromFile(ID3D11Device* pDevice, const std::string& path)
	{
		return new Texture{ pDevice, IMG_Load(path.c_str()) };
	}
}