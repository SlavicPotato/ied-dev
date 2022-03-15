#include "pch.h"

#include "IUI.h"

#include "IED/UI/UIFormBrowser.h"
#include "IED/UI/UIFormInfoCache.h"
#include "IED/UI/UIMain.h"

#include "Drivers/UI.h"

namespace IED
{
	void IUI::UIInitialize(Controller& a_controller)
	{
		m_task = std::make_unique<IUIRenderTaskMain>(*this, a_controller);
	}

	bool IUI::UIIsInitialized() const noexcept
	{
		return m_task.get() != nullptr;
	}

	UI::UIPopupQueue& IUI::UIGetPopupQueue() noexcept
	{
		return m_task->GetContext()->GetPopupQueue();
	}

	UI::UIFormBrowser& IUI::UIGetFormBrowser() noexcept
	{
		return m_task->GetContext()->GetFormBrowser();
	}

	UI::UIFormInfoCache& IUI::UIGetFormLookupCache() noexcept
	{
		return m_task->GetContext()->GetFormLookupCache();
	}

	void IUI::UIReset()
	{
		m_task->QueueReset();
	}

	auto IUI::UIToggle() -> UIOpenResult
	{
		IScopedLock lock(UIGetLock());

		if (!m_task)
		{
			return UIOpenResult::kResultNone;
		}

		if (m_task->IsRunning())
		{
			m_task->GetContext()->SetOpenState(false);

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

		return UIOpenImpl();
	}

	auto IUI::UIOpenImpl() -> UIOpenResult
	{
		if (m_task && m_task->RunEnableChecks())
		{
			if (Drivers::UI::AddTask(0, m_task))
			{
				return UIOpenResult::kResultEnabled;
			}
		}

		return UIOpenResult::kResultNone;
	}

	void IUIRenderTaskMain::OnTaskStart()
	{
		IScopedLock lock(m_owner.UIGetLock());

		try
		{
			m_context->SetOpenState(true);
			m_owner.OnUIOpen();
			m_context->OnOpen();
			OnStart();
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
	}

	bool IUIRenderTaskMain::ShouldClose()
	{
		return GetContext()->GetUISettings().closeOnESC &&
		           ImGui::GetIO().KeysDown[VK_ESCAPE] ||
		       (!GetEnabledInMenu() && Game::InPausedMenu());
	}

}