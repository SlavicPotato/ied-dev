#pragma once

#include "IED/ConfigStore.h"

#include "Widgets/UISlotEditorWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		template <class T>
		class UISlotEditorBase :
			public UISlotEditorWidget<T>
		{
		public:
			UISlotEditorBase(
				Controller& a_controller);

		private:
			SlotProfile::base_type m_tempData;
		};

		template <class T>
		UISlotEditorBase<T>::UISlotEditorBase(
			Controller& a_controller) :
			UISlotEditorWidget<T>(a_controller)
		{}

	}
}