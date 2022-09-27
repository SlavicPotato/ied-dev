#pragma once

namespace IED
{
	namespace UI
	{
		class UIEditorInterface
		{
		public:
			virtual void EditorInitialize(){};
			virtual void EditorDraw(){};
			virtual void EditorReset(){};
			virtual void EditorOnOpen(){};
			virtual void EditorOnClose(){};
			virtual void EditorQueueUpdateCurrent(){};
			virtual void EditorDrawMenuBarItems(){};
		};

	}
}