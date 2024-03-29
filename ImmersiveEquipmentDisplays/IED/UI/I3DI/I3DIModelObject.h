#pragma once

#include "I3DIBoundObject.h"

#include "IED/D3D/D3DObject.h"

namespace IED
{
	class D3DCommon;

	namespace UI
	{
		class I3DIObjectController;
		struct I3DICommonData;

		class I3DIModelObject :
			public I3DIBoundObject,
			public D3DObject
		{
			friend class I3DIObjectController;

		public:
			I3DIModelObject(
				ID3D11Device*                        a_device,
				BoundingShape                        a_boundingShape,
				const std::shared_ptr<D3DModelData>& a_data);

			virtual ~I3DIModelObject() noexcept override = default;

			virtual I3DIModelObject* AsModelObject() override
			{
				return this;
			};

			virtual void UpdateBound() override;

			virtual void DrawObjectExtra(I3DICommonData& a_data) override;
			virtual void RenderObject(D3DCommon& a_data) override;

			virtual void OnMouseMoveOver(I3DICommonData& a_data) override;
			virtual void OnMouseMoveOut(I3DICommonData& a_data) override;
			virtual bool OnSelect(I3DICommonData& a_data) override;
			virtual void OnUnselect(I3DICommonData& a_data) override;

			virtual void                 SetLastDistance(const std::optional<float>& a_distance) override;
			virtual std::optional<float> GetLastDistance() const override;

			virtual I3DIBoundObject* GetParentObject() const;

		protected:
			virtual bool WantDrawTooltip();
			virtual bool WantDrawBound();
			virtual bool WantDrawContents();

			virtual void DrawTooltip(I3DICommonData& a_data){};
			virtual void DrawContents(I3DICommonData& a_data){};

		private:
			std::optional<float> m_lastDistance;
		};

	}
}