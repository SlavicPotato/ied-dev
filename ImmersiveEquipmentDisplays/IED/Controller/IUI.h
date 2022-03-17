#pragma once

#include "IActorInfo.h"

#include "Drivers/UI.h"

#include "IED/UI/UIContextBase.h"

namespace IED
{
	namespace UI
	{
		class UIFormBrowser;
		class UIFormInfoCache;
		class UIPopupQueue;
		class UIMain;
	}

	class IUI;

	template <class T>
	class IUIRenderTask :
		public Tasks::UIRenderTaskBase
	{
	public:
		template <class... Args>
		IUIRenderTask(
			IUI&        a_interface,
			Controller& a_controller,
			Args&&... a_args);

		virtual ~IUIRenderTask() noexcept = default;

		[[nodiscard]] inline constexpr T* GetContext() const noexcept
		{
			return static_cast<T*>(m_context.get());
		}

		inline constexpr void QueueReset() noexcept
		{
			m_reset = true;
		}

	protected:
		virtual bool Run() override;
		virtual void OnTaskStop() override;
		virtual void OnTaskStart() override;

		virtual bool ShouldClose();

		virtual void OnStart(){};

		std::unique_ptr<UI::UIContextBase> m_context;

		bool m_reset{ false };

		IUI& m_owner;
	};

	template <class T>
	template <class... Args>
	IUIRenderTask<T>::IUIRenderTask(
		IUI&        a_interface,
		Controller& a_controller,
		Args&&... a_args) :
		m_owner(a_interface)
	{
		m_context = std::make_unique<T>(a_controller, std::forward<Args>(a_args)...);
		m_context->Initialize();
	}

	template <class T>
	bool IUIRenderTask<T>::Run()
	{
		IScopedLock lock(m_owner.UIGetLock());

		try
		{
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

			return m_context->IsWindowOpen();
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
	}

	template <class T>
	void IUIRenderTask<T>::OnTaskStop()
	{
		IScopedLock lock(m_owner.UIGetLock());

		try
		{
			m_context->SetOpenState(false);
			m_context->OnClose();
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
	}

	template <class T>
	void IUIRenderTask<T>::OnTaskStart()
	{
		IScopedLock lock(m_owner.UIGetLock());

		try
		{
			m_context->SetOpenState(true);
			m_context->OnOpen();
			OnStart();
		}
		catch (const std::exception& e)
		{
			HALT(e.what());
		}
	}

	template <class T>
	bool IUIRenderTask<T>::ShouldClose()
	{
		return false;
	}

	class IUIRenderTaskMain :
		public IUIRenderTask<UI::UIMain>
	{
	public:
		using IUIRenderTask<UI::UIMain>::IUIRenderTask;

	private:
		virtual void OnTaskStart() override;
		virtual bool ShouldClose() override;
	};

	template <class T>
	class IUITimedRenderTask :
		public IUIRenderTask<T>
	{
	public:
		template <class... Args>
		IUITimedRenderTask(
			IUI&        a_interface,
			Controller& a_controller,
			long long   a_lifetime,
			Args&&... a_args);

	private:
		virtual bool ShouldClose() override;
		virtual void OnStart() override;

		long long m_lifetime;
		long long m_deadline{ 0 };
	};

	template <class T>
	template <class... Args>
	IUITimedRenderTask<T>::IUITimedRenderTask(
		IUI&        a_interface,
		Controller& a_controller,
		long long   a_lifetime,
		Args&&... a_args) :
		IUIRenderTask<T>(
			a_interface,
			a_controller,
			std::forward<Args>(a_args)...),
		m_lifetime(a_lifetime)
	{
	}

	template <class T>
	void IUITimedRenderTask<T>::OnStart()
	{
		m_deadline = m_state.startTime + IPerfCounter::T(m_lifetime);
	}

	template <class T>
	bool IUITimedRenderTask<T>::ShouldClose()
	{
		return IPerfCounter::Query() >= m_deadline;
	}

	class IUI :
		public IActorInfo
	{
		friend class IUIRenderTaskMain;

		template <class T>
		friend class IUIRenderTask;

		enum class UIOpenResult
		{
			kResultNone,
			kResultEnabled,
			kResultDisabled
		};

	public:
		IUI()                   = default;
		virtual ~IUI() noexcept = default;

		UI::UIPopupQueue&    UIGetPopupQueue() noexcept;
		UI::UIFormBrowser&   UIGetFormBrowser() noexcept;
		UI::UIFormInfoCache& UIGetFormLookupCache() noexcept;
		void                 UIReset();

		[[nodiscard]] inline constexpr auto& UIGetRenderTask() noexcept
		{
			return m_task;
		}

	protected:
		void         UIInitialize(Controller& a_controller);
		UIOpenResult UIToggle();
		UIOpenResult UIOpen();

		[[nodiscard]] bool UIIsInitialized() const noexcept;

	private:
		virtual constexpr WCriticalSection& UIGetLock() noexcept = 0;
		virtual void                        OnUIOpen(){};

		std::shared_ptr<IUIRenderTaskMain> m_task;

		UIOpenResult UIOpenImpl();
	};

}