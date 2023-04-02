#pragma once

#include "IED/UI/Profile/UIProfileEditorBase.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UIData.h"
#include "IED/UI/Widgets/KeyBind/UIKeyBindEditorWidget.h"

#include "IED/GlobalProfileManager.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIProfileEditorKeybind :
			public UIProfileEditorBase<KeybindProfile>,
			UIKeyBindEditorWidget
		{
		public:
			static constexpr auto CHILD_ID = ChildWindowID::kProfileEditorKeybind;

			UIProfileEditorKeybind(Controller& a_controller);
			~UIProfileEditorKeybind() noexcept override;

			void Initialize() override;

			void DrawProfileEditorMenuBarItems() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			virtual void DrawItem(KeybindProfile& a_profile) override;

			virtual ProfileManager<KeybindProfile>& GetProfileManager() const override;
			virtual UIPopupQueue&                   GetPopupQueue_ProfileBase() const override;

			virtual WindowLayoutData GetWindowDimensions() const;

			Controller& m_controller;
		};
	}
}