#include "pch.h"

#include "IUI.h"

#include "IED/UI/UIFormBrowser.h"
#include "IED/UI/UIFormInfoCache.h"
#include "IED/UI/UIMain.h"

#include "Drivers/UI.h"

#include <ext/GameCommon.h>

namespace IED
{
	void IUI::UIInitialize(Controller& a_controller)
	{
		m_UIContext = std::make_unique<UI::UIMain>(a_controller);
		m_UIContext->Initialize();
	}

	bool IUI::UIIsInitialized() const noexcept
	{
		return m_UIContext.get() != nullptr;
	}

	UI::UIPopupQueue& IUI::UIGetPopupQueue() noexcept
	{
		return m_UIContext->GetPopupQueue();
	}

	UI::UIFormBrowser& IUI::UIGetFormBrowser() noexcept
	{
		return m_UIContext->GetFormBrowser();
	}

	UI::UIFormInfoCache& IUI::UIGetFormLookupCache() noexcept
	{
		return m_UIContext->GetFormLookupCache();
	}

	void IUI::UIReset()
	{
		m_UIContext->Reset();
	}

	auto IUI::UIToggle() -> UIOpenResult
	{
		IScopedLock lock(UIGetLock());

		if (m_UIContext->IsWindowOpen())
		{
			m_UIContext->SetOpenState(false);

			return UIOpenResult::kResultDisabled;
		}
		else
		{
			return UIOpenImpl();
		}
	}

	auto IUI::UIOpen() -> UIOpenResult
	{
		IScopedLock lock(UIGetLock());

		if (!m_UIContext->IsWindowOpen())
		{
			return UIOpenImpl();
		}
		else
		{
			return UIOpenResult::kResultNone;
		}
	}

	bool IUI::UIRunTask()
	{
		IScopedLock lock(UIGetLock());

		try
		{
			if (m_resetUI)
			{
				m_resetUI = false;
				m_UIContext->Reset();
			}

			if ((m_UIContext->GetUISettings().closeOnESC && ImGui::GetIO().KeysDown[VK_ESCAPE]) ||
			    (!UIGetEnabledInMenu() && Game::InPausedMenu()))
			{
				m_UIContext->SetOpenState(false);
			}

			m_UIContext->Draw();

			if (!m_UIContext->IsWindowOpen())
			{
				m_UIContext->OnClose();
			}

			return m_UIContext->IsWindowOpen();
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
	}

	auto IUI::UIOpenImpl() -> UIOpenResult
	{
		if (UIRunEnableChecks())
		{
			if (Drivers::UI::AddTask(0, this))
			{
				m_UIContext->SetOpenState(true);
				OnUIOpen();
				m_UIContext->OnOpen();

				return UIOpenResult::kResultEnabled;
			}
		}

		return UIOpenResult::kResultNone;
	}

}  // namespace IED