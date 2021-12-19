#pragma once

namespace IED
{
	namespace UI
	{
		class UIEditorInterface
		{
		public:
			virtual void Initialize(){};
			virtual void Draw(){};
			virtual void Reset(){};
			virtual void OnOpen(){};
			virtual void OnClose(){};
			virtual void QueueUpdateCurrent(){};

			virtual void DrawMenuBarItems(){};
		};

	}
}