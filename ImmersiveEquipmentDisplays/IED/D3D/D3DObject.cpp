#include "pch.h"

#include "D3DObject.h"

#include "D3DAssets.h"
#include "D3DCommon.h"
#include "D3DHelpers.h"
#include "D3DModelData.h"

#include "Common/VectorMath.h"

namespace IED
{
	using namespace DirectX;

	D3DObject::D3DObject(
		ID3D11Device*                        a_device,
		const std::shared_ptr<D3DModelData>& a_data) noexcept(false) :
		D3DEffect(a_device),
		m_data(a_data)
	{
		m_effect->SetTextureEnabled(false);
		m_effect->EnableDefaultLighting();

		if (a_data->HasVertexColors())
		{
			m_effect->SetVertexColorEnabled(true);
			m_effect->SetAlpha(0.5f);
		}
		else
		{
			m_effect->SetColorAndAlpha({ 0.5f, 0.5f, 0.5f, 0.5f });
		}

		CreateInputLayout(a_device);
	}

	void D3DObject::UpdateBound()
	{
		m_data->GetBound().Transform(m_bound, m_world);
	}

	bool XM_CALLCONV D3DObject::Intersects(
		FXMVECTOR a_origin,
		FXMVECTOR a_direction,
		float&    a_distance) const
	{
		return m_bound.Intersects(a_origin, a_direction, a_distance);
	}

	void D3DObject::Draw(
		D3DCommon& a_scene)
	{
		auto context = a_scene.GetContext().Get();

		SetMatrices(a_scene.GetViewMatrix(), a_scene.GetProjectionMatrix());

		a_scene.SetRasterizerState(m_flagsbf.rasterizerState);

		std::optional<bool> lastDepthState;

		if (m_flags.test(D3DObjectFlags::kOverrideDepth))
		{
			lastDepthState = a_scene.GetDepthEnabled();

			a_scene.SetRenderTargets(m_flags.test(D3DObjectFlags::kDepth));
		}

		ApplyEffect(context);

		m_data->Draw(context);

		if (lastDepthState)
		{
			a_scene.SetRenderTargets(*lastDepthState);
		}
	}

	void D3DObject::Draw(
		D3DCommon&      a_scene,
		const XMMATRIX& a_view,
		const XMMATRIX& a_projection)
	{
		SetMatrices(a_view, a_projection);
		Draw(a_scene);
	}
}