#pragma once

#include "IED/UI/Widgets/UIFormTypeSelectorWidget.h"

#include "IED/UI/UIMiscTextInterface.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	struct FormInfoResult;
	struct FormInfoData;
	class BaseExtraFormInfo;
	struct ObjectEntryBase;

	class Controller;

	namespace UI
	{
		class UIFormInfoTooltipWidget :
			public virtual UIMiscTextInterface
		{
		public:
			UIFormInfoTooltipWidget(Controller& a_controller);

			void DrawFormInfoTooltip(
				const FormInfoResult*  a_info,
				const ObjectEntryBase& a_entry);

			void DrawObjectEntryHeaderInfo(
				const FormInfoResult*  a_info,
				const ObjectEntryBase& a_entry);

			void DrawFormWithInfo(Game::FormID a_form);
			void DrawFormWithInfoWrapped(Game::FormID a_form);

		private:
			void DrawGeneralFormInfoTooltip(Game::FormID a_form);

			void DrawGeneralFormInfoTooltip(
				const FormInfoResult* a_info);

			void DrawFormInfoResult(
				const FormInfoResult& a_info);

			void DrawFormInfo(
				const FormInfoData& a_info);

			void DrawExtraFormInfo(
				const BaseExtraFormInfo& a_info);

			Controller& m_controller;
		};
	}
}