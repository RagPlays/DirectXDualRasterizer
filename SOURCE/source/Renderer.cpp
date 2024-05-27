#include "pch.h"
#include "Renderer.h"
#include "DataTypes.h"
#include "Camera.h"
#include "Texture.h"
#include "Utils.h"
#include "VehicleEffect.h"
#include "FireEffect.h"
#include "Mesh.h"

namespace dae 
{
	Renderer::Renderer(SDL_Window* pWindow, int width, int height) 
		: m_pWindow{ pWindow }
		, m_Width{ width }
		, m_Height{ height }
		, m_IsInitialized{ false }
		, m_CurrentFileringMode{ FilteringMode::Point }
		, m_pVehicleMesh{ nullptr }
		, m_pCamera{ nullptr }
		, m_pVechicleDiffusedMap{ nullptr }
		, m_RotateAngle{ 0.f }
		, m_MeshRotating{ true }
		, m_ShowFireFX{ true }
		, m_MeshRotationSpeed{ static_cast<float>(M_PI) / 4.f } // 45°/sec
	{
		// Camera
		m_pCamera = new Camera{ {0.f, 0.f, -50.f}, 45.f, width / static_cast<float>(height), 0.1f, 1000.f };

		//Initialize DirectX
		if (InitializeDirectX() == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		InitMesh();
	}

	Renderer::~Renderer()
	{
		if (m_pVehicleMesh) delete m_pVehicleMesh;
		if (m_pFireMesh) delete m_pFireMesh;

		if (m_pCamera) delete m_pCamera;

		if (m_pVechicleDiffusedMap) delete m_pVechicleDiffusedMap;
		if (m_pFireDiffusedMap) delete m_pFireDiffusedMap;
		if (m_pNormalMap) delete m_pNormalMap;
		if (m_pSpecularMap) delete m_pSpecularMap;
		if (m_pGlossinessMap) delete m_pGlossinessMap;

		ReleaseDirectXResources();
	}

	void Renderer::ToggleFilteringMode()
	{
		std::cout << "FilteringMode: ";

		switch (m_CurrentFileringMode)
		{
		case dae::FilteringMode::Point:
			m_CurrentFileringMode = FilteringMode::Linear;
			std::cout << "LINEAR";
			break;
		case dae::FilteringMode::Linear:
			m_CurrentFileringMode = FilteringMode::Anisotropic;
			std::cout << "ANISOTROPIC";
			break;
		case dae::FilteringMode::Anisotropic:
			m_CurrentFileringMode = FilteringMode::Point;
			std::cout << "POINT";
			break;
		default:
			std::cout << "UNKNOWN!";
			break;
		}
		std::cout << "\n";
	}

	void Renderer::ToggleRotating()
	{
		m_MeshRotating = !m_MeshRotating;
	}

	void Renderer::ToggleNormalMap()
	{

	}

	void Renderer::ToggleFireFX()
	{
		m_ShowFireFX = !m_ShowFireFX;
	}

	void Renderer::Update(const Timer* const pTimer)
	{
		m_pCamera->Update(pTimer);

		if (m_MeshRotating)
		{
			m_RotateAngle += pTimer->GetElapsed() * m_MeshRotationSpeed;
			m_RotationMatrix = Matrix::CreateRotation(0.f, m_RotateAngle, 0.f);
			m_WorldMatrix = m_RotationMatrix * m_TranslationMatrix;
		}

		// World View Projection Matrix
		m_WorldViewProjectionMatrix = m_WorldMatrix * m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix();
		m_pVehicleMesh->GetEffect()->GetWorldViewProjectionMatrix()->SetMatrix(reinterpret_cast<float*>(&m_WorldViewProjectionMatrix));
		m_pFireMesh->GetEffect()->GetWorldViewProjectionMatrix()->SetMatrix(reinterpret_cast<float*>(&m_WorldViewProjectionMatrix));

		// Camera Pos
		Vector3 cameraPos{ m_pCamera->GetOrigin() };
		m_pVehicleMesh->GetEffect()->GetCameraPos()->SetFloatVector(reinterpret_cast<float*>(&cameraPos));

		// World Matrix
		m_pVehicleMesh->GetEffect()->GetWorldMatrix()->SetMatrix(reinterpret_cast<float*>(&m_WorldMatrix));
	}

	void Renderer::Render() const
	{
		// check if initialization worked
		if (!m_IsInitialized) return;

		//1. CLEAR RTV & DSV
		constexpr float backGroundColor[4]{ 0.39f, 0.59f, 0.93f, 1.f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, backGroundColor);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		//2. SET PIPELINE + INVOKE DRAW CALLS (= RENDER)
		m_pVehicleMesh->Render(m_pDeviceContext, static_cast<uint32_t>(m_CurrentFileringMode));

		if (m_ShowFireFX)
		{
			m_pFireMesh->Render(m_pDeviceContext, static_cast<uint32_t>(m_CurrentFileringMode));
		}

		//3. PRESENT BACKBUFFER (SWAP)
		m_pSwapChain->Present(0, 0);
	}

	HRESULT Renderer::InitializeDirectX()
	{
		// 1. Create Device & DeviceContext
		D3D_FEATURE_LEVEL featureLevel{ D3D_FEATURE_LEVEL_11_1 };
		uint32_t createDeviceFlags{ 0 };
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG; // device for debug
#endif
		HRESULT result
		{ 
			D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext) 
		};
		if (FAILED(result)) return result;

		// Create DXGI Factory
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result)) return result;

		// 2. Create swapchain
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		// Get the handle (HWND) from the SDL backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;
		
		//Create swapChain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result)) return result;

		// 3. Create DepthStencil (DS) & DepthStencilView (DSV)
		// Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		// vieuw
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		
		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result)) return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result)) return result;

		// 4. Create RenderTarget (RT) & RenderTargetView (RTV)

		// Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result)) return result;

		// View
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result)) return result;

		// 5. Bind RTV & DSV to Output Merger Stage
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		// 6. Set Viewport
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		return S_OK;
	}

	void Renderer::ReleaseDirectXResources()
	{
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		if(m_pDevice) m_pDevice->Release();
		if(m_pSwapChain) m_pSwapChain->Release();
		if(m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();
		if(m_pDepthStencilView) m_pDepthStencilView->Release();
		if(m_pRenderTargetBuffer) m_pRenderTargetBuffer->Release();
		if(m_pRenderTargetView) m_pRenderTargetView->Release();
	}

	void Renderer::InitMesh()
	{
		m_TranslationMatrix = Matrix::CreateTranslation(0.f, 0.f, 0.f);
		m_RotationMatrix = Matrix::CreateRotation(0.f, 0.f, 0.f);
		m_WorldMatrix = m_RotationMatrix * m_TranslationMatrix;

		//// VEHICLE ////
		// mesh vertices / indices vechicle
		std::vector<Vertex> vehileVertices;
		std::vector<uint32_t> vehicleIndices;
		if (!dae::Utils::ParseOBJ("Resources/vehicle.obj", vehileVertices, vehicleIndices))
		{
			assert(false);
		}

		m_pVehicleMesh = new Mesh<VehicleEffect>{ m_pDevice, vehileVertices, vehicleIndices, L"Resources/Vehicle.fx" };

		// load in maps / textures
		m_pVechicleDiffusedMap = Texture::LoadFromFile(m_pDevice, "Resources/vehicle_diffuse.png");
		m_pVehicleMesh->GetEffect()->SetDiffusemap(m_pVechicleDiffusedMap);

		m_pNormalMap = Texture::LoadFromFile(m_pDevice, "Resources/vehicle_normal.png");
		m_pVehicleMesh->GetEffect()->SetNormalMap(m_pNormalMap);

		m_pSpecularMap = Texture::LoadFromFile(m_pDevice, "Resources/vehicle_specular.png");
		m_pVehicleMesh->GetEffect()->SetSpecualarMap(m_pSpecularMap);

		m_pGlossinessMap = Texture::LoadFromFile(m_pDevice, "Resources/vehicle_gloss.png");
		m_pVehicleMesh->GetEffect()->SetGlossinessMap(m_pGlossinessMap);

		//// FIRE ////
		// mesh vertices / indices vechicle
		std::vector<Vertex> fireVertices;
		std::vector<uint32_t> fireIndices;
		if (!dae::Utils::ParseOBJ("Resources/fireFX.obj", fireVertices, fireIndices))
		{
			assert(false);
		}

		m_pFireMesh = new Mesh<FireEffect>{ m_pDevice, fireVertices, fireIndices, L"Resources/Fire.fx" };

		// load in maps / textures
		m_pFireDiffusedMap = Texture::LoadFromFile(m_pDevice, "Resources/fireFX_diffuse.png");
		m_pFireMesh->GetEffect()->SetDiffusemap(m_pFireDiffusedMap);
	}
}