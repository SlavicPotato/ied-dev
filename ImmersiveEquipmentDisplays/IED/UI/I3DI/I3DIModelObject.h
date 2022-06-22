#pragma once

#include "I3DIObject.h"

#include "IED/D3D/D3DObject.h"

namespace IED
{
	class D3DCommon;

	namespace UI
	{
		class I3DIObjectController;
		struct I3DICommonData;

		class I3DIModelObject :
			public I3DIObject,
			public D3DObject
		{
			friend class I3DIObjectController;

		public:
			I3DIModelObject(
				ID3D11Device*                        a_device,
				ID3D11DeviceContext*                 a_context,
				const std::shared_ptr<D3DModelData>& a_data);

			virtual ~I3DIModelObject() noexcept override = default;

			virtual I3DIModelObject* GetAsModelObject() override
			{
				return this;
			};

			virtual void DrawObjectExtra(I3DICommonData& a_data) override;
			virtual void RenderObject(D3DCommon& a_data);

			virtual void OnMouseMoveOver(I3DICommonData& a_data) override;
			virtual void OnMouseMoveOut(I3DICommonData& a_data) override;
			virtual void OnSelect(I3DICommonData& a_data) override;
			virtual void OnUnselect(I3DICommonData& a_data) override;

			virtual bool  ObjectIntersects(
				I3DICommonData& a_data,
				float&            a_dist) override;

			virtual DirectX::XMVECTOR XM_CALLCONV GetParentCenter() const = 0;

		protected:
			virtual bool WantDrawTooltip();
			virtual bool WantDrawBound();

			virtual void DrawTooltip(I3DICommonData& a_data){};
		};

	}
}