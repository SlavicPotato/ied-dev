#pragma once

#include "UIConditionalVariablesEditor.h"

#include "IED/UI/UIContext.h"
#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIConditionalVariablesEditorWindow :
			public UIContext,
			public UIWindow,
			public UIConditionalVariablesEditor,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID = "ied_cve";

		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUIConditionalVariablesEditorWindow;

			UIConditionalVariablesEditorWindow(Controller& a_controller);

			void Initialize() override;
			void Reset() override;
			void Draw() override;
			void OnOpen() override;
			void OnClose() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:

			void DrawMenuBar();

			Controller& m_controller;
		};
	}
}