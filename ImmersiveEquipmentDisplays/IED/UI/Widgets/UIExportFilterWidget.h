#pragma once

#include "IED/ConfigSerializationFlags.h"

namespace IED
{
	namespace UI
	{
		class UIExportFilterWidget
		{
		protected:
			static bool DrawExportFilters(
				stl::flag<Data::ConfigStoreSerializationFlags>& a_flags);
		};
	}
}