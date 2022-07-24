#pragma once

#include "I3DIActorContext.h"
#include "I3DICommonData.h"

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
			/*struct Data : I3DICommonData
			{
				Data(
					ID3D11Device*                      a_device,
					ID3D11DeviceContext*               a_context,
					const DXGI_SWAP_CHAIN_DESC&        a_desc,
					std::unique_ptr<I3DIActorContext>& a_actorContext) noexcept(false) :
					I3DICommonData(a_device, a_context, a_desc, a_actorContext)
				{
				}
			};*/

			inline static constexpr auto WINDOW_ID = "ied_i3di";

		public:
			I3DIMain(Controller& a_controller);

			virtual void Initialize() override;
			virtual void Draw() override;
			virtual void PrepareGameData() override;
			virtual void Render() override;

			virtual void OnOpen() override;
			virtual void OnClose() override;

			virtual void OnMouseMove(const Handlers::MouseMoveEvent& a_evn) override;

		private:
			bool CreateCommonData();

			void SwitchToActorContext(Game::FormID a_actor);
			void UpdateActorObjects();
			NiCamera* GetCamera();

			void SetNodeConditionForced(Game::FormID a_actor, bool a_switch) const ;

			std::unique_ptr<I3DICommonData>   m_data;
			std::unique_ptr<I3DIActorContext> m_actorContext;
		};
	}
}