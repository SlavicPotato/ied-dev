#pragma once

namespace IED
{
	namespace UI
	{

		enum class ChildWindowID : std::uint32_t
		{
			kUIFormBrowser = 0,
			kUISettings,
			kUIDialogImportExport,
			kUIProfileEditorSlot,
			kUIProfileEditorCustom,
			kUIProfileEditorNodeOverride,
			kUINodeMapEditor,
			kUINodeOverrideEditorWindow,
			kUIProfileEditorFormFilters,
			kUILog,
			kUIStats,
			kUISkeletonExplorer,
			kUIActorInfo,
			kUIDisplayManagement,
			kUIConditionalVariablesEditorWindow,
			kUIProfileEditorConditionalVariables,
			kI3DI,
			kKeyBindEditor,
			kOutfitFormManager,
			kOutfitEditor,
			kProfileEditorOutfit,
			kGeneralInfo,
			kProfileEditorKeybind,
			kStyleEditor,

			kMax
		};

	}
}