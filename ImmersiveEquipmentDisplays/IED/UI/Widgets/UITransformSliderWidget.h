#pragma once

#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UITips.h"

#include "IED/ConfigTransform.h"
#include "UIPopupToggleButtonWidget.h"

#include "UITransformSliderWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		enum class TransformSliderContextAction
		{
			None,
			PasteOver
		};

		class UITransformSliderWidget
		{
		public:
			enum class TransformUpdateValue
			{
				Position,
				Rotation,
				Scale,
				All
			};

		private:
			using func_upd_t = std::function<void(TransformUpdateValue)>;
			using func_ext_t = std::function<void()>;

			enum class ContextMenuResult
			{
				kNone,
				kReset
			};

		public:
			UITransformSliderWidget() = default;

			TransformSliderContextAction DrawTransformHeaderContextMenu(
				Data::configTransform_t& a_data,
				func_upd_t               a_updateop);

			void DrawTransformTree(
				Data::configTransform_t& a_data,
				const bool               a_indent,
				func_upd_t               a_updateop,
				func_ext_t               a_extra,
				Localization::StringID   a_label = static_cast<Localization::StringID>(CommonStrings::Transform),
				ImGuiTreeNodeFlags       a_flags = ImGuiTreeNodeFlags_DefaultOpen);

			void DrawTransformFlags(
				Data::configTransform_t& a_data,
				func_upd_t               a_updateop);

			void DrawTransformSliders(
				Data::configTransform_t& a_data,
				func_upd_t               a_updateop);

			ContextMenuResult DrawTransformSliderContextMenu();
		};

	}
}