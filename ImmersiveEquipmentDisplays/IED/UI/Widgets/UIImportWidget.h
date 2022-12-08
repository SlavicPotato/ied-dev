#pragma once

#include "IED/UI/UILocalizationInterface.h"

#include "IED/UI/UITips.h"
#include "UIExportFilterWidget.h"

#include "IED/Controller/ImportFlags.h"

#include "IED/ConfigSerializationFlags.h"

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
			public UIExportFilterWidget
		{
		public:
			UIImportWidget(Controller& a_controller);

			void QueueImportPopup(
				const fs::path&          a_fullpath,
				const stl::fixed_string& a_key);

		private:
			virtual void OnDataImport(bool a_success) = 0;

			void DoImport(
				const Data::configStore_t&                     a_data,
				stl::flag<ImportFlags>                         a_flags,
				stl::flag<Data::ConfigStoreSerializationFlags> a_serFlags);

			Controller& m_controller;
		};
	}
}