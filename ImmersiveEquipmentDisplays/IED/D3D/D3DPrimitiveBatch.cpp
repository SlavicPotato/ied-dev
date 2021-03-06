#include "pch.h"

#include "D3DPrimitiveBatch.h"

#include "D3DHelpers.h"

namespace IED
{
	using namespace DirectX;

	D3DPrimitiveBatch::D3DPrimitiveBatch(
		ID3D11Device*        a_device,
		ID3D11DeviceContext* a_context) noexcept(false) :
		D3DEffect(
			a_device,
			D3DVertexShaderID::kColorVertexShader,
			D3DPixelShaderID::kBasicPixelShader)
	{
		m_batch = std::make_unique<PrimitiveBatch<VertexPositionColorAV>>(a_context);
	}

	void D3DPrimitiveBatch::Draw(
		D3DCommon& a_scene)
	{
		if (m_lines.empty())
		{
			return;
		}

		SetMatrices(a_scene.GetViewMatrix(), a_scene.GetProjectionMatrix());

		a_scene.SetRasterizerState(D3DObjectRasterizerState::kWireframe);
		a_scene.SetRenderTargets(m_flags.test(D3DPrimitiveBatchFlags::kDepth));
		a_scene.SetDepthStencilState(D3DDepthStencilState::kNone);

		auto context = a_scene.GetContext().Get();

		context->IASetInputLayout(a_scene.GetILVertexPositionColorAV().Get());
		ApplyEffect(context, a_scene);

		m_batch->Begin();

		for (auto& e : m_lines)
		{
			m_batch->DrawLine(e.first, e.second);
		}

		m_batch->End();

		m_lines.clear();
	}

	void XM_CALLCONV D3DPrimitiveBatch::AddLine(
		XMVECTOR a_p1,
		XMVECTOR a_c1,
		XMVECTOR a_p2,
		XMVECTOR a_c2)
	{
		m_lines.emplace_back(
			VertexPositionColorAV(a_p1, a_c1),
			VertexPositionColorAV(a_p2, a_c2));
	}

	void XM_CALLCONV D3DPrimitiveBatch::AddLine(
		XMVECTOR a_p1,
		XMVECTOR a_p2,
		XMVECTOR a_c)
	{
		m_lines.emplace_back(
			VertexPositionColorAV(a_p1, a_c),
			VertexPositionColorAV(a_p2, a_c));
	}
}