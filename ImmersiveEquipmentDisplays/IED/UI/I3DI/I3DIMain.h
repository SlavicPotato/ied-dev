#pragma once

#include "I3DIActorContext.h"
#include "I3DICommonData.h"

#include "IED/UI/UIContext.h"
#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class I3DIMain :
			public UIContext,
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
			inline static constexpr auto CHILD_ID = ChildWindowID::kI3DI;

			I3DIMain(Controller& a_controller);

			virtual void Initialize() override;
			virtual void Draw() override;
			virtual void PrepareGameData() override;
			virtual void Render() override;

			virtual void OnOpen() override;
			virtual void OnClose() override;
			virtual bool OnWantOpenStateChange(bool a_newState) override;

			virtual void OnMouseMove(const Handlers::MouseMoveEvent& a_evn) override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

			[[nodiscard]] inline constexpr auto& GetActorContext() const noexcept
			{
				return m_actorContext;
			}

			[[nodiscard]] inline constexpr auto& GetCommonData() const noexcept
			{
				return m_data;
			}

			[[nodiscard]] inline constexpr auto GetLastPrepTime() const noexcept
			{
				return m_lastPrepTime;
			}

		private:
			bool CreateCommonData();

			void PrepareGameDataImpl();

			bool                SwitchToActorContext(Game::FormID a_actor);
			void                ReleaseCurrentActorContext();
			void                UpdateActorObjects();
			NiPointer<NiCamera> GetCamera();

			void SetNodeConditionForced(Game::FormID a_actor, bool a_switch) const;

			std::unique_ptr<I3DICommonData>   m_data;
			std::unique_ptr<I3DIActorContext> m_actorContext;

			PerfTimerInt m_prepPT;
			long long    m_lastPrepTime{ 0 };

			std::optional<Game::FormID> m_heldActorOnClose;

			Controller& m_controller;
		};
	}
}