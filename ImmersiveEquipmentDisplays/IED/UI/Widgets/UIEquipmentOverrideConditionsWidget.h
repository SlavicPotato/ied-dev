#pragma once

#include "UIConditionParamEditorWidget.h"
#include "UIDescriptionPopup.h"

#include "IED/UI/UILocalizationInterface.h"

#include "UIEquipmentOverrideResult.h"

#include "IED/ConditionalVariableStorage.h"
#include "IED/ConfigEquipment.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIEquipmentOverrideConditionsWidget :
			UIConditionParamExtraInterface,
			public virtual UIDescriptionPopupWidget,
			public virtual UITipsInterface,
			public virtual UILocalizationInterface
		{
		public:
			UIEquipmentOverrideConditionsWidget(Controller& a_controller);

		protected:
			using update_func_t = std::function<void()>;

			UIEquipmentOverrideResult DrawEquipmentOverrideEntryContextMenu(
				bool a_drawDelete);

			BaseConfigEditorAction DrawEquipmentOverrideEntryConditionHeaderContextMenu(
				Data::equipmentOverrideConditionList_t& a_entry,
				update_func_t                           a_updFunc);

			void DrawEquipmentOverrideConditionTree(
				Data::equipmentOverrideConditionList_t& a_entry,
				update_func_t                           a_updFunc);

			void DrawEquipmentOverrideEntryConditionTable(
				Data::equipmentOverrideConditionList_t& a_entry,
				bool                                    a_isnested,
				update_func_t                           a_updFunc);

		private:
			void UpdateMatchParamAllowedTypes(Data::EquipmentOverrideConditionType a_type);

			bool DrawFormCountExtraSegment(Data::equipmentOverrideCondition_t* a_match);

			virtual bool DrawConditionParamExtra(
				void*       a_p1,
				const void* a_p2) override;

			virtual bool DrawConditionItemExtra(
				ConditionParamItem           a_item,
				ConditionParamItemExtraArgs& a_args) override;

			virtual void OnConditionItemChange(
				ConditionParamItem                    a_item,
				const ConditionParamItemOnChangeArgs& a_args) override;

			static void GetConditionListDepth(
				const Data::equipmentOverrideConditionList_t& a_in,
				std::uint32_t&                                a_result,
				std::uint32_t&                                a_offset) noexcept;

		protected:
			Game::FormID             m_aoNewEntryID;
			Game::FormID             m_aoNewEntryKWID;
			Game::FormID             m_aoNewEntryRaceID;
			Game::FormID             m_aoNewEntryActorID;
			Game::FormID             m_aoNewEntryNPCID;
			Game::FormID             m_aoNewEntryGlobID;
			Game::FormID             m_aoNewEntryFactionID;
			BIPED_OBJECT             m_ooNewBiped{ BIPED_OBJECT::kNone };
			Data::ExtraConditionType m_ooNewExtraCond{ Data::ExtraConditionType::kNone };
			Data::ObjectSlotExtra    m_aoNewSlot{ Data::ObjectSlotExtra::kNone };

			UIConditionParamEditorWidget m_condParamEditor;
		};

	}
}