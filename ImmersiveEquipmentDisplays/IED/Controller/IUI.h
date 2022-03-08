#pragma once

#include "IActorInfo.h"

#include "Drivers/UI.h"

namespace IED
{
	namespace UI
	{
		class UIMain;
		class UIFormBrowser;
		class UIFormInfoCache;
		class UIPopupQueue;
	}

	class IUI :
		public Tasks::UIRenderTaskBase,
		public IActorInfo
	{
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

	protected:
		void         UIInitialize(Controller& a_controller);
		UIOpenResult UIToggle();
		UIOpenResult UIOpen();

		[[nodiscard]] bool UIIsInitialized() const noexcept;

	private:
		virtual bool                        UIRunTask() override;
		virtual constexpr WCriticalSection& UIGetLock() noexcept = 0;
		virtual void                        OnUIOpen(){};

		UIOpenResult UIOpenImpl();

		std::unique_ptr<UI::UIMain> m_UIContext;
		bool                        m_resetUI{ false };
	};
}