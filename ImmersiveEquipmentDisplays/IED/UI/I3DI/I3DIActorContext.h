#pragma once

#include "I3DIMOVNode.h"
#include "I3DIWeaponNode.h"

#include <ext/ILUID.h>

namespace IED
{
	class Controller;

	namespace UI
	{
		struct I3DICommonData;
		class I3DIObjectController;
		class I3DIActorObject;

		class I3DIActorContext
		{
		public:
			I3DIActorContext(
				I3DICommonData& a_data,
				Controller&     a_controller,
				Game::FormID    a_actor,
				const std::shared_ptr<I3DIActorObject>& a_actorObject) noexcept(false);

			void RegisterObjects(I3DIObjectController& a_objectController);
			void UnregisterObjects(I3DIObjectController& a_objectController);

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

			[[nodiscard]] inline constexpr auto& GetActorObject() const noexcept
			{
				return m_actorObject;
			}

		private:
			Game::FormID m_actor;

			std::unordered_map<stl::fixed_string, std::shared_ptr<I3DIWeaponNode>> m_weaponNodes;

			bool m_ranFirstUpdate{ false };
			bool m_lastUpdateFailed{ false };

			std::shared_ptr<I3DIActorObject> m_actorObject;

			Controller& m_controller;
		};
	}
}