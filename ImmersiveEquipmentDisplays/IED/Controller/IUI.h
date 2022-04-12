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
	class Controller;

	class IUIRenderTask :
		public Tasks::UIRenderTaskBase
	{
	public:
		IUIRenderTask(
			IUI& a_interface);

		virtual ~IUIRenderTask() noexcept = default;

		template <class T = UI::UIContextBase>
		[[nodiscard]] inline constexpr T* GetContext() const noexcept requires
			std::is_base_of_v<UI::UIContextBase, T>
		{
			auto result = dynamic_cast<T*>(m_context.get());
			assert(result);
			return result;
		}

		[[nodiscard]] inline UI::UIContextBase* GetContext() const noexcept
		{
			return m_context.get();
		}

		template <class T, class... Args>
		[[nodiscard]] void InitializeContext(Controller& a_controller, Args&&... a_args) requires
			std::is_base_of_v<UI::UIContextBase, T>
		{
			m_context = std::make_unique<T>(a_controller, std::forward<Args>(a_args)...);
			m_context->Initialize();
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

		virtual void OnStart();

		std::unique_ptr<UI::UIContextBase> m_context;

		bool m_reset{ false };

		IUI& m_owner;
	};

	class IUIRenderTaskMain :
		public IUIRenderTask
	{
	public:
		IUIRenderTaskMain(
			IUI&        a_interface,
			Controller& a_controller);

		UI::UIMain* GetContext() const noexcept;

	private:
		virtual void OnTaskStart() override;
		virtual bool ShouldClose() override;
	};

	class IUITimedRenderTask :
		public IUIRenderTask
	{
	public:
		IUITimedRenderTask(
			IUI&      a_interface,
			long long a_lifetime);

	private:
		virtual bool ShouldClose() override;
		virtual void OnStart() override;

		long long m_lifetime;
		long long m_deadline{ 0 };
	};

	class IUI :
		public IActorInfo
	{
		friend class IUIRenderTaskMain;

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

		[[nodiscard]] inline constexpr auto& UIGetRenderTask() const noexcept
		{
			return m_task;
		}

	protected:
		void         UIInitialize(Controller& a_controller);
		UIOpenResult UIToggle();
		UIOpenResult UIOpen();

		[[nodiscard]] bool UIIsInitialized() const noexcept;

		bool m_safeToOpenUI{ false };

	private:
		virtual constexpr stl::critical_section& UIGetLock() noexcept = 0;
		virtual void                             OnUIOpen(){};

		UIOpenResult UIOpenImpl();

		std::shared_ptr<IUIRenderTaskMain> m_task;
	};

}