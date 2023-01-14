#pragma once

#include "IActorInfo.h"

#include "Drivers/UI.h"

#include "IED/UI/UIContext.h"

#include "Localization/ILocalization.h"

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

		virtual ~IUIRenderTask() noexcept override = default;

		template <class T = UI::UIContext>
		[[nodiscard]] constexpr T* GetContext() const noexcept
			requires std::is_base_of_v<UI::UIContext, T>
		{
			return static_cast<T*>(m_context.get());
		}

		[[nodiscard]] inline UI::UIContext* GetContext() const noexcept
		{
			return m_context.get();
		}

		template <class T, class... Args>
		[[nodiscard]] void InitializeContext(Args&&... a_args)
			requires std::is_base_of_v<UI::UIContext, T>
		{
			m_context = std::make_unique<T>(*this, std::forward<Args>(a_args)...);
			m_context->Initialize();
		}

		constexpr void QueueReset() noexcept
		{
			m_reset = true;
		}

	protected:
		virtual bool Run() override;
		virtual void PrepareGameData() override;
		virtual void Render() override;
		virtual void OnMouseMove(const Handlers::MouseMoveEvent& a_evn) override;
		virtual void OnKeyEvent(const Handlers::KeyEvent& a_evn) override;

		virtual void OnTaskStop() override;
		virtual void OnTaskStart() override;

		virtual bool ShouldClose();

		virtual void OnStart();

		std::unique_ptr<UI::UIContext> m_context;

		bool m_reset{ false };

		IUI& m_owner;
	};

	class IUIRenderTaskMain :
		public IUIRenderTask
	{
	public:
		IUIRenderTaskMain(
			IUI& a_interface);

		UI::UIMain& GetContext() const noexcept;

	private:
		virtual void OnTaskStart() override;
		virtual void OnTaskStop() override;
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
		public Localization::ILocalization,
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
		template <class Tc, class... Args>
		inline auto make_render_task(
			Args&&... a_args)
		{
			auto result = std::make_shared<IUIRenderTask>(
				*this);

			result->InitializeContext<Tc>(
				std::forward<Args>(a_args)...);

			return result;
		}

		IUI()                   = default;
		virtual ~IUI() noexcept = default;

		UI::UIPopupQueue&    UIGetPopupQueue() noexcept;
		UI::UIFormBrowser&   UIGetFormBrowser() noexcept;
		UI::UIFormInfoCache& UIGetFormLookupCache() noexcept;
		void                 UIReset();

		[[nodiscard]] constexpr auto& UIGetRenderTask() const noexcept
		{
			return m_task;
		}

		void QueueToast(
			const std::string&           a_message,
			const std::optional<ImVec4>& a_color = {});

		void QueueToastAsync(
			const std::string&           a_message,
			const std::optional<ImVec4>& a_color = {});

	protected:
		void QueueToastImpl(
			const std::string&           a_message,
			const std::optional<ImVec4>& a_color);

		void QueueToastImpl(
			std::string&&                a_message,
			const std::optional<ImVec4>& a_color);

		void         UIInitialize(Controller& a_controller);
		UIOpenResult UIToggle();
		UIOpenResult UIOpen();

		[[nodiscard]] bool UIIsInitialized() const noexcept;

		bool m_safeToOpenUI{ false };

	private:
		const std::shared_ptr<IUIRenderTask>& GetOrCreateToastTask();

		virtual constexpr stl::recursive_mutex& UIGetLock() noexcept = 0;
		virtual void                            OnUIOpen(){};
		virtual void                            OnUIClose(){};

		UIOpenResult UIOpenImpl();

		std::shared_ptr<IUIRenderTaskMain> m_task;
		std::shared_ptr<IUIRenderTask>     m_toastTask;
	};

}