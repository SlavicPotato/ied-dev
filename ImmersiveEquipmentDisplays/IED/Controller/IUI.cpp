#include "pch.h"

#include "IUI.h"

#include "IED/UI/UIFormBrowser.h"
#include "IED/UI/UIFormInfoCache.h"
#include "IED/UI/UIMain.h"
#include "IED/UI/UIToast.h"

#include "Drivers/UI.h"

namespace IED
{
	void IUI::QueueToast(
		std::string                  a_message,
		const std::optional<ImVec4>& a_color)
	{
		if (a_message.empty())
		{
			return;
		}

		if (!Drivers::UI::IsImInitialized())
		{
			return;
		}

		QueueToastImpl(std::move(a_message), a_color);
	}

	void IUI::QueueToastAsync(
		std::string                  a_message,
		const std::optional<ImVec4>& a_color)
	{
		if (a_message.empty())
		{
			return;
		}

		if (!Drivers::UI::IsImInitialized())
		{
			return;
		}

		ITaskPool::AddTask(
			[this,
		     msg = std::move(a_message),
		     col = a_color]() mutable {
				QueueToastImpl(std::move(msg), col);
			});
	}

	void IUI::QueueToastImpl(
		const std::string&           a_message,
		const std::optional<ImVec4>& a_color)
	{
		auto& task = GetOrCreateToastTask();

		auto context = task->GetContext<UI::UIToast>();
		assert(context);

		context->QueueMessage(a_message, a_color);

		Drivers::UI::AddTask(TOAST_TASK_ID, task);
	}

	void IUI::QueueToastImpl(
		std::string&&                a_message,
		const std::optional<ImVec4>& a_color)
	{
		auto& task = GetOrCreateToastTask();

		auto context = task->GetContext<UI::UIToast>();
		assert(context);

		context->QueueMessage(std::move(a_message), a_color);

		Drivers::UI::AddTask(TOAST_TASK_ID, task);
	}

	void IUI::UIInitialize(Controller& a_controller)
	{
		auto tmp = stl::make_smart<IUIRenderTaskMain>(*this);
		tmp->InitializeContext<UI::UIMain>(a_controller);
		m_task = std::move(tmp);
	}

	const stl::smart_ptr<IUIRenderTask>& IUI::GetOrCreateToastTask()
	{
		const stl::lock_guard lock(m_makeToastLock);

		auto& task = m_toastTask;

		if (!task)
		{
			task = make_render_task<UI::UIToast>();

			task->SetControlLock(false);
			task->SetFreezeTime(false);
			task->SetWantCursor(false);
			task->SetEnabledInMenu(true);
			task->EnableRestrictions(false);
		}

		return task;
	}

	UI::UIPopupQueue& IUI::UIGetPopupQueue() noexcept
	{
		return m_task->GetContext().GetPopupQueue();
	}

	UI::UIFormBrowser& IUI::UIGetFormBrowser() noexcept
	{
		return m_task->GetContext().GetFormBrowser();
	}

	UI::UIFormInfoCache& IUI::UIGetFormLookupCache() noexcept
	{
		return m_task->GetContext().GetFormLookupCache();
	}

	void IUI::UIReset()
	{
		m_task->QueueReset();
	}

	auto IUI::UIOpen() -> UIOpenResult
	{
		return UIOpenImpl();
	}

	stl::smart_ptr<IUIRenderTaskMain> IUI::UIOpenGetRenderTask()
	{
		const stl::lock_guard lock(UIGetLock());

		return m_task && m_safeToOpenUI ? m_task : nullptr;
	}

	auto IUI::UIOpenImpl() -> UIOpenResult
	{
		if (const auto task = UIOpenGetRenderTask())
		{
			if (Drivers::UI::AddTask(0, task))
			{
				return UIOpenResult::kResultEnabled;
			}
		}

		return UIOpenResult::kResultNone;
	}

	IUIRenderTask::IUIRenderTask(
		IUI& a_interface) :
		m_owner(a_interface)
	{
	}

	bool IUIRenderTask::Run()
	{
		const stl::lock_guard lock(m_owner.UIGetLock());

#if defined(DEBUG)
		try
		{
#endif
			if (m_reset)
			{
				m_reset = false;
				m_context->Reset();
			}

			if (ShouldClose())
			{
				m_context->SetOpenState(false);
			}

			m_context->Draw();

			return m_context->IsContextOpen();

#if defined(DEBUG)
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
#endif
	}

	void IUIRenderTask::PrepareGameData()
	{
		const stl::lock_guard lock(m_owner.UIGetLock());

#if defined(DEBUG)
		try
		{
#endif
			m_context->PrepareGameData();
#if defined(DEBUG)
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
#endif
	}

	void IUIRenderTask::Render()
	{
		const stl::lock_guard lock(m_owner.UIGetLock());

#if defined(DEBUG)
		try
		{
#endif
			m_context->Render();
#if defined(DEBUG)
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
#endif
	}

	void IUIRenderTask::OnMouseMove(
		const Handlers::MouseMoveEvent& a_evn)
	{
		const stl::lock_guard lock(m_owner.UIGetLock());

#if defined(DEBUG)
		try
		{
#endif
			m_context->OnMouseMove(a_evn);
#if defined(DEBUG)
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
#endif
	}

	void IUIRenderTask::OnKeyEvent(const Handlers::KeyEvent& a_evn)
	{
		const stl::lock_guard lock(m_owner.UIGetLock());

#if defined(DEBUG)
		try
		{
#endif
			m_context->OnKeyEvent(a_evn);
#if defined(DEBUG)
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
#endif
	}

	void IUIRenderTask::OnTaskStop()
	{
		const stl::lock_guard lock(m_owner.UIGetLock());

#if defined(DEBUG)
		try
		{
#endif
			m_context->SetOpenState(false);
			m_context->OnClose();
#if defined(DEBUG)
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
#endif
	}

	void IUIRenderTask::OnTaskStart()
	{
		const stl::lock_guard lock(m_owner.UIGetLock());

#if defined(DEBUG)
		try
		{
#endif
			m_context->SetOpenState(true);
			m_context->OnOpen();
			OnStart();
#if defined(DEBUG)
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
#endif
	}

	bool IUIRenderTask::ShouldClose()
	{
		return false;
	}

	void IUIRenderTask::OnStart()
	{
	}

	IUIRenderTaskMain::IUIRenderTaskMain(
		IUI& a_interface) :
		IUIRenderTask(a_interface)
	{
	}

	UI::UIMain& IUIRenderTaskMain::GetContext() const noexcept
	{
		return static_cast<UI::UIMain&>(*m_context);
	}

	void IUIRenderTaskMain::OnTaskStart()
	{
		const stl::lock_guard lock(m_owner.UIGetLock());

#if defined(DEBUG)
		try
		{
#endif
			m_context->SetOpenState(true);
			m_owner.OnUIOpen();
			m_context->OnOpen();
			OnStart();
#if defined(DEBUG)
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
#endif
	}

	void IUIRenderTaskMain::OnTaskStop()
	{
		const stl::lock_guard lock(m_owner.UIGetLock());

#if defined(DEBUG)
		try
		{
#endif
			m_context->SetOpenState(false);
			m_context->OnClose();
			m_owner.OnUIClose();
#if defined(DEBUG)
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
#endif
	}

	bool IUIRenderTaskMain::ShouldClose()
	{
		return GetContext().GetUISettings().closeOnESC &&
		           ImGui::GetIO().KeysDown[VK_ESCAPE] ||
		       (!GetEnabledInMenu() && Game::InPausedMenu());
	}

	IUITimedRenderTask::IUITimedRenderTask(
		IUI&      a_interface,
		long long a_lifetime) :
		IUIRenderTask(a_interface),
		m_lifetime(a_lifetime)
	{
	}

	void IUITimedRenderTask::OnStart()
	{
		m_deadline = m_state.startTime + IPerfCounter::T(m_lifetime);
	}

	bool IUITimedRenderTask::ShouldClose()
	{
		return IPerfCounter::Query() >= m_deadline;
	}
}