#pragma once

#include "IED/ConfigSerializationFlags.h"

namespace IED
{
	namespace UI
	{
		class UIExportFilterWidget
		{
		public:
			static bool DrawExportFilters(
				stl::flag<Data::ConfigStoreSerializationFlags>& a_flags);
		};
	}
}