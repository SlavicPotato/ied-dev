#include "pch.h"

#include "Tasks.h"

#include <ext/GameCommon.h>

namespace IED
{
	namespace Tasks
	{
		bool UIRenderTaskBase::UIRunEnableChecks() const
		{
			if (!m_options.enableInMenu && Game::InPausedMenu())
			{
				//Game::Debug::Notification("UI unavailable while an in-game menu is open");
				return false;
			}

			if (!m_options.enableChecks)
			{
				return true;
			}

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
				if (tm->GetDialogueTarget() != nullptr)
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

			return true;
		}

	}
}