#pragma once

#include "IED/ConfigCommon.h"

#include "UICustomEditorActor.h"
#include "UICustomEditorRace.h"
#include "UICustomEditorNPC.h"
#include "UICustomEditorGlobal.h"

#include "IED/UI/UIEditorTabPanel.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UICustomEditorTabPanel :
			public UIEditorTabPanel
		{
		public:
			UICustomEditorTabPanel(Controller& a_controller);

			UICustomEditorActor m_editorActor;
			UICustomEditorRace m_editorRace;
			UICustomEditorGlobal m_editorGlobal;
			UICustomEditorNPC m_editorNPC;

		private:
			virtual Data::SettingHolder::EditorPanel& GetEditorConfig() override;

			Controller& m_controller;
		};

	}  // namespace UI
}  // namespace IED