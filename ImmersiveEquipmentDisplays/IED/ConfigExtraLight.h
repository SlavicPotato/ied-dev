#pragma once

#include "ConfigLUIDTag.h"

#include "ExtraLightData.h"

namespace IED
{
	namespace Data
	{
		struct configExtraLight_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
			};

			ExtraLightData data;

		protected:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& data.flags.value;
				a_ar& data.shadowDepthBias;
				a_ar& data.fieldOfView;
			}
		};
	}
}

BOOST_CLASS_VERSION(
	IED::Data::configExtraLight_t,
	IED::Data::configExtraLight_t::Serialization::DataVersion1);
