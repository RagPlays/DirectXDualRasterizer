#ifndef MESH_H
#define MESH_H

#include "DataTypes.h"
#include "Matrix.h"
#include "VehicleEffect.h"
#include "FireEffect.h"

namespace dae
{
	struct Matrix;
	struct Vertex;

	class BaseEffect;
	class VehicleEffect;
	class FireEffect;

	template<typename EffectClass>
	class Mesh final
	{
	public:
		explicit Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& rawVertices, const std::vector<uint32_t>& rawIndices, const std::wstring& effectFileName);
		virtual ~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) noexcept = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) noexcept = delete;

		EffectClass* GetEffect() const;

		void Render(ID3D11DeviceContext* pDeviceContext, uint32_t filterModeIndex) const;

	private:
		EffectClass* m_pEffect;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
		uint32_t m_NumIndices;
	};

	// sorry for unstructured implementation, dividing .h and .cpp gets me linker errors
	template<typename EffectClass>
	Mesh<EffectClass>::Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::wstring& effectFileName)
	{
		m_pEffect = new EffectClass{ pDevice, effectFileName };

		// Create Vertex Buffer
		D3D11_BUFFER_DESC bd{};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = vertices.data();

		HRESULT result{ pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer) };
		if (FAILED(result))
		{
			assert(false);
			return;
		}

		// Create Index Buffer
		m_NumIndices = static_cast<uint32_t>(indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		initData.pSysMem = indices.data();
		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
		if (FAILED(result))
		{
			assert(false);
			return;
		}
	}

	template<typename EffectClass>
	Mesh<EffectClass>::~Mesh()
	{
		if (m_pEffect) delete m_pEffect;
		if (m_pVertexBuffer) m_pVertexBuffer->Release();
		if (m_pIndexBuffer) m_pIndexBuffer->Release();
	}

	template<typename EffectClass>
	EffectClass* Mesh<EffectClass>::GetEffect() const
	{
		return m_pEffect;
	}

	template<typename EffectClass>
	void Mesh<EffectClass>::Render(ID3D11DeviceContext* pDeviceContext, uint32_t filterModeIndex) const
	{
		// 1. Set Primitive Topology
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 2. Set Input Layout
		pDeviceContext->IASetInputLayout(m_pEffect->GetInputLayout());

		// 3. Set VertexBuffer
		constexpr UINT stride{ sizeof(Vertex) };
		constexpr UINT offset{ 0 };
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		// 4. Set IndexBuffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// 5. Draw / Render
		D3DX11_TECHNIQUE_DESC techDesc{};
		m_pEffect->GetTechnique()->GetDesc(&techDesc);
		m_pEffect->GetTechnique()->GetPassByIndex(filterModeIndex)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

#endif // !MESH_H