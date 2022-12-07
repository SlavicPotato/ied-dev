#pragma once

#include "IED/UI/Widgets/UIFormTypeSelectorWidget.h"

#include "IED/UI/UIMiscTextInterface.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	struct formInfoResult_t;
	struct formInfo_t;
	struct ObjectEntryBase;

	class Controller;

	namespace UI
	{
		class UIFormInfoTooltipWidget :
			public virtual UIMiscTextInterface,
			public virtual UIFormTypeSelectorWidget,
			public virtual UILocalizationInterface
		{
		public:
			UIFormInfoTooltipWidget(Controller& a_controller);

			void DrawFormInfoTooltip(
				const formInfoResult_t* a_info,
				const ObjectEntryBase&  a_entry);

			void DrawObjectEntryHeaderInfo(
				const formInfoResult_t* a_info,
				const ObjectEntryBase&  a_entry);

			void DrawFormWithInfo(Game::FormID a_form);
			void DrawFormWithInfoWrapped(Game::FormID a_form);

		private:
			void DrawGeneralFormInfoTooltip(Game::FormID a_form);

			void DrawGeneralFormInfoTooltip(
				const formInfoResult_t* a_info);

			void DrawFormInfo(
				const formInfo_t& a_info);

			Controller& m_controller;
		};
	}
}