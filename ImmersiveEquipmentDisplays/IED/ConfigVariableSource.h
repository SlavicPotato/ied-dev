#pragma once

#include "IED/ConfigCommon.h"

namespace IED
{
	namespace Data
	{
		enum class VariableSource : std::uint32_t
		{
			kActor         = 0,
			kPlayerHorse   = 1,
			kMountingActor = 2,
			kMountedActor  = 3,
			kSelf          = 4,
		};

		enum class VariableSourceFlags : std::uint32_t
		{
			kNone = 0,
		};

		DEFINE_ENUM_CLASS_BITWISE(VariableSourceFlags);

		struct configVariableSource_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
			};

			stl::flag<VariableSourceFlags> flags{ VariableSourceFlags::kNone };
			VariableSource                 source{ VariableSource::kActor };
			configForm_t                   form;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& source;
				a_ar& form;
			}
		};
	}
}

BOOST_CLASS_VERSION(
	IED::Data::configVariableSource_t,
	IED::Data::configVariableSource_t::Serialization::DataVersion1);
