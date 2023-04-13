#include "pch.h"

#include "Tasks.h"

#include <ext/GameCommon.h>

namespace IED
{
	namespace Tasks
	{
		long long UIRenderTaskBase::GetRunTime() const noexcept
		{
			if (m_state.running)
			{
				return IPerfCounter::delta_us(
					m_state.startTime,
					IPerfCounter::Query());
			}
			else
			{
				return 0;
			}
		}

		bool UIRenderTaskBase::RunEnableChecks() const
		{
			if (m_state.running)
			{
				return false;
			}

			if (!m_options.enableInMenu)
			{
				if (Game::InPausedMenu())
				{
					return false;
				}

				constexpr UIStringHolder::STRING_INDICES menus[] = {
					UIStringHolder::STRING_INDICES::kcontainerMenu,
					UIStringHolder::STRING_INDICES::kinventoryMenu,
					UIStringHolder::STRING_INDICES::kmagicMenu,
					UIStringHolder::STRING_INDICES::ktweenMenu,
					UIStringHolder::STRING_INDICES::ksleepWaitMenu,
					UIStringHolder::STRING_INDICES::kbarterMenu,
					UIStringHolder::STRING_INDICES::kgiftMenu,
					UIStringHolder::STRING_INDICES::kfavoritesMenu,
					UIStringHolder::STRING_INDICES::ktrainingMenu,
					UIStringHolder::STRING_INDICES::ktutorialMenu,
					UIStringHolder::STRING_INDICES::klockpickingMenu,
					UIStringHolder::STRING_INDICES::kbookMenu,
					UIStringHolder::STRING_INDICES::kconsole,
					UIStringHolder::STRING_INDICES::kjournalMenu,
					UIStringHolder::STRING_INDICES::kmessageBoxMenu,
					UIStringHolder::STRING_INDICES::kmapMenu,
					UIStringHolder::STRING_INDICES::klevelUpMenu,
					UIStringHolder::STRING_INDICES::kstatsMenu
				};

				if (Game::IsAnyMenuOpen(menus))
				{
					return false;
				}

				if (const auto mm = MenuManager::GetSingleton())
				{
					if (mm->IsMenuOpen("CustomMenu"))
					{
						return false;
					}
				}
			}

			if (m_options.enableChecks)
			{
				auto player = *g_thePlayer;
				if (!player)
				{
					return false;
				}

				if (player->IsInCombat())
				{
					Game::Debug::Notification("UI unavailable while in combat");
					return false;
				}

				if (auto pl = Game::ProcessLists::GetSingleton())
				{
					if (pl->GuardsPursuing(player))
					{
						Game::Debug::Notification("UI unavailable while pursued by guards");
						return false;
					}
				}

				if (auto tm = MenuTopicManager::GetSingleton())
				{
					if (tm->talkingHandle && tm->talkingHandle.IsValid())
					{
						Game::Debug::Notification("UI unavailable while in a conversation");
						return false;
					}
				}

				if (player->unkBDA & PlayerCharacter::FlagBDA::kAIDriven)
				{
					Game::Debug::Notification("UI unavailable while the player is AI driven");
					return false;
				}

				if (player->byCharGenFlag & PlayerCharacter::ByCharGenFlag::kHandsBound)
				{
					Game::Debug::Notification("UI unavailable while your hands are bound");
					return false;
				}
			}

			return true;
		}
	}
}