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
			virtual void DrawExtraEquipmentOverrideOptions(
				T                          a_handle,
				Data::configBase_t&        a_data,
				const void*                a_params,
				Data::equipmentOverride_t& a_override) override;

			SlotProfile::base_type m_tempData;
		};

		template <class T>
		UISlotEditorBase<T>::UISlotEditorBase(
			Controller& a_controller) :
			UISlotEditorWidget<T>(a_controller)
		{}

		template <class T>
		void UISlotEditorBase<T>::DrawExtraEquipmentOverrideOptions(
			T                          a_handle,
			Data::configBase_t&        a_data,
			const void*                a_params,
			Data::equipmentOverride_t& a_override)
		{
			/*if (ImGui::CheckboxFlagsT(
					"Match slot item##eo_ctl",
					stl::underlying(std::addressof(a_override.eoFlags.value)),
					stl::underlying(Data::EquipmentOverrideFlags::kSlotItem)))
			{
				OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
			}

			ImGui::Spacing();*/
		}

	}
}