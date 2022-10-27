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
		const std::shared_ptr<D3DModelData>& a_data) :
		D3DEffect(
			a_device,
			D3DVertexShaderID::kLightingColorVertexShader,
			D3DPixelShaderID::kLightingPixelShader),
		m_data(a_data)
	{
		SetLightingEnabled(true);
		SetDiffuseColor({ 0.0f, 1.0f, 1.0f, 0.5f });
		SetAlpha(0.5f);
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

	float XM_CALLCONV D3DObject::GetDistance(
		FXMVECTOR a_origin) const
	{
		return XMVectorGetX(XMVector3Length(m_world.r[3] - a_origin));
	}

	float XM_CALLCONV D3DObject::GetDistanceSq(
		FXMVECTOR a_origin) const
	{
		return XMVectorGetX(XMVector3LengthSq(m_world.r[3] - a_origin));
	}

	float XM_CALLCONV D3DObject::GetCenterDistance(DirectX::FXMVECTOR a_origin) const
	{
		auto center = XMLoadFloat3(std::addressof(m_bound.Center));
		return XMVectorGetX(XMVector3Length(center - a_origin));
	}

	float XM_CALLCONV D3DObject::GetCenterDistanceSq(DirectX::FXMVECTOR a_origin) const
	{
		auto center = XMLoadFloat3(std::addressof(m_bound.Center));
		return XMVectorGetX(XMVector3LengthSq(center - a_origin));
	}

	void D3DObject::Draw(
		D3DCommon& a_scene)
	{
		auto context = a_scene.GetContext().Get();

		SetMatrices(a_scene.GetViewMatrix(), a_scene.GetProjectionMatrix());

		a_scene.SetRasterizerState(m_flagsbf.rasterizerState);
		a_scene.SetRenderTargets(m_flags.test(D3DObjectFlags::kDepth));
		a_scene.SetDepthStencilState(
			m_alpha >= 1.0f ?
				D3DDepthStencilState::kWrite :
                D3DDepthStencilState::kNone);

		context->IASetInputLayout(a_scene.GetILVertexPositionNormalColor().Get());
		ApplyEffect(context, a_scene);

		m_data->Draw(context);
	}

}