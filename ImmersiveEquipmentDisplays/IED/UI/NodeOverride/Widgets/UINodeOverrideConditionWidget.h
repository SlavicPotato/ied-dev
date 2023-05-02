#pragma once

#include "IED/ConfigCommon.h"

#include "IED/UI/UITips.h"

#include "IED/UI/Widgets/UIConditionParamEditorWidget.h"
#include "IED/UI/Widgets/UIDescriptionPopup.h"

#include "UINodeOverrideEditorWidgetStrings.h"
#include "UINodeOverrideWidgetCommon.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINodeOverrideConditionWidget :
			UIConditionParamExtraInterface,
			public UIDescriptionPopupWidget
		{
			using update_func_t = std::function<void()>;

		public:
			UINodeOverrideConditionWidget(Controller& a_controller);

			NodeOverrideCommonResult DrawConditionContextMenu(
				NodeOverrideDataType                 a_type,
				const bool                           a_ignoreNode);

			NodeOverrideCommonAction DrawConditionHeaderContextMenu(
				const stl::fixed_string&                 a_name,
				NodeOverrideDataType                     a_type,
				entryNodeOverrideData_t&                 a_data,
				Data::configNodeOverrideConditionList_t& a_entry,
				const bool                               a_exists,
				const bool                               a_ignoreNode,
				update_func_t                            a_updateFunc);

			void DrawConditionTable(
				const stl::fixed_string&                 a_name,
				NodeOverrideDataType                     a_type,
				entryNodeOverrideData_t&                 a_data,
				Data::configNodeOverrideConditionList_t& a_entry,
				const bool                               a_exists,
				bool                                     a_isnested,
				bool                                     a_ignoreNode,
				update_func_t                            a_updateFunc);

			void DrawConditionTree(
				const stl::fixed_string&                 a_name,
				NodeOverrideDataType                     a_type,
				entryNodeOverrideData_t&                 a_data,
				Data::configNodeOverrideConditionList_t& a_entry,
				const bool                               a_exists,
				const bool                               a_ignoreNode,
				update_func_t                            a_updateFunc);

		private:
			virtual bool DrawConditionParamExtra(
				void*       a_p1,
				const void* a_p2) override;

			virtual bool DrawConditionItemExtra(
				ConditionParamItem           a_item,
				ConditionParamItemExtraArgs& a_args) override;

			virtual void OnConditionItemChange(
				ConditionParamItem                    a_item,
				const ConditionParamItemOnChangeArgs& a_args) override;

			void UpdateMatchParamAllowedTypes(Data::NodeOverrideConditionType a_type);

			constexpr auto reset_oo()
			{
				m_ooNewEntryID        = {};
				m_ooNewEntryIDKW      = {};
				m_ooNewEntryIDActor   = {};
				m_ooNewEntryIDNPC     = {};
				m_ooNewEntryIDGlob    = {};
				m_ooNewEntryIDFaction = {};
				m_ooNewEntryIDEffect  = {};
				m_ooNewEntryIDPerk    = {};
				m_ooNewEntryIDCell    = {};
				m_ooNewBiped          = BIPED_OBJECT::kNone;
				m_ooNewSlot           = Data::ObjectSlotExtra::kNone;
				m_ooNewExtraCond      = Data::ExtraConditionType::kNone;
			}

			Game::FormID m_ooNewEntryID;
			Game::FormID m_ooNewEntryIDKW;
			Game::FormID m_ooNewEntryIDActor;
			Game::FormID m_ooNewEntryIDNPC;
			Game::FormID m_ooNewEntryIDGlob;
			Game::FormID m_ooNewEntryIDFaction;
			Game::FormID m_ooNewEntryIDEffect;
			Game::FormID m_ooNewEntryIDPerk;
			Game::FormID m_ooNewEntryIDCell;

			BIPED_OBJECT             m_ooNewBiped{ BIPED_OBJECT::kNone };
			Data::ObjectSlotExtra    m_ooNewSlot{ Data::ObjectSlotExtra::kNone };
			Data::ExtraConditionType m_ooNewExtraCond{ Data::ExtraConditionType::kNone };

			UIConditionParamEditorWidget m_condParamEditor;

			//char m_buffer1[260]{ 0 };
		};
	}
}