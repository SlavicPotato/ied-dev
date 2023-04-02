#pragma once

#include "IED/UI/UIContext.h"
#include "IED/UI/Window/UIWindow.h"

#include "IED/UI/Widgets/KeyBind/UIKeyBindEditorWidget.h"
#include "IED/UI/Widgets/UIProfileSelectorWidget.h"

#include "IED/GlobalProfileManager.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		struct KeyBindEditorPSParams
		{
			Data::configKeybindEntryHolder_t& data;
		};

		class UIKeyBindEditorWindow :
			public UIContext,
			public UIWindow,
			UIKeyBindEditorWidget,
			UIProfileSelectorWidget<
				KeyBindEditorPSParams,
				KeybindProfile>
		{
			static constexpr auto WINDOW_ID = "ied_kbe";

		public:
			static constexpr auto CHILD_ID = ChildWindowID::kKeyBindEditor;

			UIKeyBindEditorWindow(
				Controller& a_controller);
			
			~UIKeyBindEditorWindow();

			void Initialize() override;
			void Draw() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			void DrawEditor();
			void DrawProfileTree();

			void DrawMenuBar();
			void DrawFileMenu();
			void DrawActionMenu();

			virtual void ApplyProfile(
				const KeyBindEditorPSParams& a_data,
				const KeybindProfile&        a_profile) override;

			virtual void MergeProfile(
				const KeyBindEditorPSParams& a_data,
				const KeybindProfile&        a_profile) override;

			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual KeybindProfile::base_type GetData(
				const KeyBindEditorPSParams& a_params) override;

			virtual void OnKeybindErase(const Data::configKeybindEntryHolder_t::container_type::key_type& a_key) override;
			virtual void OnKeybindAdd(const Data::configKeybindEntryHolder_t::container_type::value_type& a_key) override;
			virtual void OnKeybindChange(const Data::configKeybindEntryHolder_t::container_type::value_type& a_data) override;

			virtual bool GetKeybindState(
				const Data::configKeybindEntryHolder_t::container_type::key_type& a_key,
				std::uint32_t&                                                    a_stateOut) override;

			Controller& m_controller;
		};

	}
}
