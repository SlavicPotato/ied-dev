#pragma once

#include "IED/UI/UILocalizationInterface.h"

#include "IED/UI/UITips.h"
#include "UIExportFilterWidget.h"

#include "IED/Controller/ImportFlags.h"

namespace IED
{
	namespace Data
	{
		struct configStore_t;
	}

	class Controller;

	namespace UI
	{
		class UIImportWidget :
			public UIExportFilterWidget,
			public virtual UITipsInterface
		{
		public:
			UIImportWidget(Controller& a_controller);

			void QueueImportPopup(
				const fs::path& a_fullpath,
				const stl::fixed_string& a_key);

		private:

			virtual void OnDataImport(bool a_success) = 0;

			void DoImport(
				Data::configStore_t&& a_data,
				stl::flag<ImportFlags> a_flags);

			Controller& m_controller;
		};
	}
}