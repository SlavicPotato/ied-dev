#pragma once

#include "IED/UI/Widgets/Filters/UIGenericFilter.h"
#include "IED/UI/Widgets/UIDescriptionPopup.h"
#include "IED/UI/Widgets/UIEquipmentOverrideConditionsWidget.h"
#include "IED/UI/Widgets/UIVariableTypeSelectorWidget.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/UI/Controls/UICollapsibles.h"

#include "IED/UI/UIEditorInterface.h"
#include "IED/UI/UILocalizationInterface.h"

#include "IED/ConfigConditionalVars.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		enum class CondVarEntryAction
		{
			kNone,

			kDelete,
			kAdd,
			kAddGroup,
			kRename,
			kSwap,
			kPaste,
			kPasteOver,
			kClear
		};

		struct CondVarEntryResult
		{
			CondVarEntryAction action{ CondVarEntryAction::kNone };
			std::string        desc;
			SwapDirection      dir;
		};

		enum class CondVarEntryChangeAction
		{
			kNone,

			kReset,
			kEvaluate
		};

		struct CondVarDataChangeParams
		{
			Data::configConditionalVariablesHolder_t& holder;
			CondVarEntryChangeAction                  action{ CondVarEntryChangeAction::kNone };
		};

		class UIConditionalVariablesEditorWidget :
			public UIEquipmentOverrideConditionsWidget,
			public UIEditorInterface,
			UIVariableTypeSelectorWidget,
			public virtual UIDescriptionPopupWidget,
			public virtual UICollapsibles,
			public virtual UITipsInterface,
			public virtual UILocalizationInterface
		{
		public:
			UIConditionalVariablesEditorWidget(Controller& a_controller);

			void DrawCondVarEditor(
				Data::configConditionalVariablesHolder_t& a_data);

		protected:
			virtual void EditorDrawMenuBarItems() override;

		private:
			void DrawItemFilter();

			CondVarEntryResult DrawEntryHeaderControls(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesEntry_t&  a_data);

			void DrawNewItemContextItems(
				bool                a_insert,
				CondVarEntryResult& a_result);

			CondVarEntryResult DrawEntryContextMenu(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesEntry_t&  a_data);

			void DrawEntryList(
				Data::configConditionalVariablesHolder_t&    a_holder,
				Data::configConditionalVariablesEntryList_t& a_data,
				bool                                         a_isNested);

			bool DrawVariable(
				Data::configConditionalVariable_t& a_var);

			void DrawVariableTree(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesList_t&   a_data,
				Data::configConditionalVariable_t&        a_var);

			CondVarEntryResult DrawVariableHeaderContextMenu(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesList_t&   a_data,
				Data::configConditionalVariable_t&        a_var);

			void DrawVariableListTree(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesList_t&   a_data);

			CondVarEntryResult DrawVariableListHeaderContextMenu(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesList_t&   a_data);

			void DrawVariableList(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesList_t&   a_data);

			CondVarEntryResult DrawGroupHeaderContextMenu();

			void DrawEntryTree(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesEntry_t&  a_data,
				bool                                      a_isNested);

			void DrawGroupEntryList(
				Data::configConditionalVariablesHolder_t&    a_holder,
				Data::configConditionalVariablesEntryList_t& a_data);

			bool CreateNewVariable(
				const std::string&                                a_name,
				Data::configConditionalVariablesList_t&           a_list,
				Data::configConditionalVariablesList_t::iterator& a_where);

			virtual void DrawMainHeaderControlsExtra(
				Data::configConditionalVariablesHolder_t& a_data);

			virtual void OnCondVarEntryChange(
				const CondVarDataChangeParams& a_params) = 0;

			virtual std::optional<std::reference_wrapper<Data::configConditionalVariablesHolder_t>> GetCurrentData() = 0;

			UIGenericFilter m_itemFilter;
		};
	}
}