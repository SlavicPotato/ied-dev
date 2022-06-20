#pragma once

#include "I3DIMOVNode.h"
#include "I3DIObjectController.h"
#include "I3DIWeaponNode.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		struct I3DICommonData;

		class I3DIActorContext
		{
		public:
			I3DIActorContext(
				I3DICommonData& a_data,
				Controller&     a_controller,
				Game::FormID    a_actor) noexcept(false);

			bool Update();
			void Draw(I3DICommonData& a_data);
			void Render(I3DICommonData& a_data);

			[[nodiscard]] inline constexpr auto& GetController() const noexcept
			{
				return m_controller;
			}

			[[nodiscard]] inline constexpr auto& GetActorFormID() const noexcept
			{
				return m_actor;
			}

			[[nodiscard]] inline constexpr bool LastUpdateFailed() const noexcept
			{
				return m_lastUpdateFailed;
			}

		private:
			Game::FormID m_actor;

			std::unordered_map<stl::fixed_string, I3DIWeaponNode> m_weaponNodes;

			I3DIObjectController m_objectController;

			bool m_ranFirstUpdate{ false };
			bool m_lastUpdateFailed{ false };

			Controller& m_controller;
		};
	}
}