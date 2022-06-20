#pragma once

#include "I3DIActorContext.h"
#include "I3DIActorSelector.h"
#include "I3DICommonData.h"
#include "I3DIPopupWindow.h"

#include "IED/D3D/D3DAssets.h"
#include "IED/D3D/D3DCommon.h"
#include "IED/D3D/D3DModelData.h"
#include "IED/D3D/D3DObject.h"
#include "IED/D3D/D3DPrimitiveBatch.h"

#include "IED/UI/UIContextBase.h"
#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class I3DIMain :
			public UIContextBase,
			public UILocalizationInterface
		{
			struct Data : I3DICommonData
			{
				Data(
					ID3D11Device*               a_device,
					ID3D11DeviceContext*        a_context,
					const DXGI_SWAP_CHAIN_DESC& a_desc) noexcept(false) :
					I3DICommonData(a_device, a_context, a_desc),
					actorSelector(a_device, a_context)
				{
				}

				I3DIActorSelector actorSelector;
				I3DIPopupWindow   commonPopup;
			};

			inline static constexpr auto WINDOW_ID = "ied_i3di";

		public:
			I3DIMain(Controller& a_controller);

			virtual void Initialize() override;
			virtual void Draw() override;
			virtual void PrepareGameData() override;
			virtual void Render() override;

			virtual void OnOpen() override;
			virtual void OnClose() override;

		private:
			bool GetCameraPV();

			std::unique_ptr<Data>             m_data;
			std::unique_ptr<I3DIActorContext> m_actorContext;
		};
	}
}