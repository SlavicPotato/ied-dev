#pragma once

#include "ConfigCommon.h"
#include "ConfigVariableSource.h"

namespace IED
{
	namespace Data
	{
		struct configVariableSourceSelector_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
			};

			configVariableSource_t  varSource;
			configFixedStringList_t formVars;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& varSource;
				a_ar& formVars;
			}
		};
	}
}