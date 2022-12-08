#pragma once

#include "UILocalizationInterface.h"
#include "UITipsData.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UITipsInterface
		{
		public:
			static void        DrawTip(UITip a_id, bool a_sameLine = true);
			static void        DrawTipImportant(UITip a_id, bool a_sameLine = true);
			static void        DrawTip(const char* a_text, bool a_sameLine = true);
			static const char* GetTipText(UITip a_id);

		private:
			static void DrawTipText(const char* a_text, bool a_sameLine);
			static void DrawTipTextImportant(const char* a_text, bool a_sameLine);
		};

	}
}