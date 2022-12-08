#include "IED/UI/UILocalizationInterface.h"

#include "IED/ConfigEffectShader.h"

#include "UIEffectShaderFunctionSelector.h"

#include "UIEffectShaderEditorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		struct ESFEditorResult
		{
			bool changed{ false };
			bool reset{ false };

			inline constexpr ESFEditorResult& operator=(bool a_changed) noexcept
			{
				changed = a_changed;
				return *this;
			}

			inline constexpr ESFEditorResult& operator|=(bool a_changed) noexcept
			{
				changed |= a_changed;
				return *this;
			}

			[[nodiscard]] inline constexpr explicit operator bool() const noexcept
			{
				return changed;
			}
		};

		class UIEffectShaderFunctionEditorWidget :
			public UIEffectShaderFunctionSelector
		{
			inline static constexpr auto POPUP_ID = "esf_ed";

		public:
			UIEffectShaderFunctionEditorWidget() = default;

			void OpenEffectShaderFunctionEditor();

			ESFEditorResult DrawEffectShaderFunction(
				Data::configEffectShaderFunction_t& a_data);

		private:
			ESFEditorResult DrawFunc_UVLinearMotion(
				Data::configEffectShaderFunction_t& a_data);

			ESFEditorResult DrawFunc_Pulse(
				Data::configEffectShaderFunction_t& a_data);
		};
	}
}