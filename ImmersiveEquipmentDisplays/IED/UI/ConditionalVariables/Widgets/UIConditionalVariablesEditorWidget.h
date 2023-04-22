#pragma once

#include "IED/UI/Widgets/Filters/UIGenericFilter.h"
#include "IED/UI/Widgets/UIDescriptionPopup.h"
#include "IED/UI/Widgets/UIEquipmentOverrideConditionsWidget.h"
#include "IED/UI/Widgets/UILastEquippedWidget.h"
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
		};

		struct CondVarDataChangeParams
		{
			Data::configConditionalVariablesHolder_t& holder;
			CondVarEntryChangeAction                  action{ CondVarEntryChangeAction::kNone };
		};

		class UIConditionalVariablesEditorWidget :
			public UILastEquippedWidget,
			public virtual UIDescriptionPopupWidget,
			public virtual UICollapsibles,
			public UIEditorInterface
		{
		public:
			UIConditionalVariablesEditorWidget(Controller& a_controller);

			void DrawCondVarEditor(
				Data::configConditionalVariablesHolder_t& a_data);

		protected:
			virtual void EditorDrawMenuBarItems() override;
			virtual bool IsProfileEditor() const = 0;

		private:
			void DrawItemFilter();

			CondVarEntryResult DrawEntryHeaderContextMenu(
				Data::configConditionalVariablesHolder_t&         a_holder,
				Data::configConditionalVariablesEntryListValue_t& a_data);

			void DrawNewContextItem(
				bool                a_insert,
				CondVarEntryResult& a_result);

			void DrawEntryList(
				Data::configConditionalVariablesHolder_t&    a_holder,
				Data::configConditionalVariablesEntryList_t& a_data);

			void DrawCurrentVariableValue(
				Data::configConditionalVariablesEntryListValue_t& a_data);

			bool DrawVariableValue(
				ConditionalVariableType                     a_type,
				Data::configConditionalVariableValueData_t& a_data);
			
			bool DrawVariableForm(
				ConditionalVariableType                     a_type,
				Data::configConditionalVariableValueData_t& a_data);

			void DrawVariableTree(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesEntry_t&  a_entry,
				Data::configConditionalVariable_t&        a_var);

			CondVarEntryResult DrawVariableHeaderControls(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesList_t&   a_data,
				Data::configConditionalVariable_t&        a_var);

			void DrawVariableListTree(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesEntry_t&  a_entry,
				Data::configConditionalVariablesList_t&   a_data);

			CondVarEntryResult DrawVariableListHeaderContextMenu(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesList_t&   a_data);

			void DrawVariableList(
				Data::configConditionalVariablesHolder_t& a_holder,
				Data::configConditionalVariablesEntry_t&  a_entry,
				Data::configConditionalVariablesList_t&   a_data);

			void DrawEntryTree(
				Data::configConditionalVariablesHolder_t&         a_holder,
				Data::configConditionalVariablesEntryListValue_t& a_data);

			virtual void DrawMainHeaderControlsExtra(
				Data::configConditionalVariablesHolder_t& a_data);

			virtual void OnCondVarEntryChange(
				const CondVarDataChangeParams& a_params) = 0;

			virtual std::optional<std::reference_wrapper<Data::configConditionalVariablesHolder_t>> GetCurrentData() = 0;

			ConditionalVariableType m_tmpType{ ConditionalVariableType::kInt32 };

			UIGenericFilter m_itemFilter;

			Controller& m_controller;
		};
	}
}