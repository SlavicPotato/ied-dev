#pragma once

#include "IED/UI/Widgets/UIFormTypeSelectorWidget.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	struct formInfoResult_t;
	struct ObjectEntryBase;

	class Controller;

	namespace UI
	{
		class UIFormInfoTooltipWidget :
			public virtual UIFormTypeSelectorWidget,
			public virtual UILocalizationInterface
		{
		public:
			UIFormInfoTooltipWidget(Controller &a_controller);

			void DrawFormInfoTooltip(
				const formInfoResult_t*  a_info,
				const ObjectEntryBase& a_entry);

			void DrawObjectEntryHeaderInfo(
				const formInfoResult_t*  a_info,
				const ObjectEntryBase& a_entry);

			Controller& m_controller;
		};
	}
}