#pragma once

#include "ConfigCommon.h"
#include "ConfigOverrideTransform.h"
#include "NodeDescriptor.h"

namespace IED
{
	namespace Data
	{
		enum class FlagsBase : std::uint32_t
		{
			kNone = 0,

			kReserved0 = 1u << 0,
			kReserved1 = 1u << 1,
			kInvisible = 1u << 2,
			kHideIfUsingFurniture = 1u << 3,
			kDropOnDeath = 1u << 4,
			kDisabled = 1u << 5,
			kSyncReferenceTransform = 1u << 6,
			kReferenceMode = 1u << 7,
			kPlaySound = 1u << 8,
			kKeepTorchFlame = 1u << 9,
			kRemoveScabbard = 1u << 10,
			kLoad1pWeaponModel = 1u << 11,
			kHideLayingDown = 1u << 12,

			kResetTriggerFlags = kDropOnDeath |
			                     kReferenceMode |
			                     kKeepTorchFlame |
			                     kRemoveScabbard |
			                     kLoad1pWeaponModel,

			kPapyrus = 1u << 16,
		};

		DEFINE_ENUM_CLASS_BITWISE(FlagsBase);

		struct configBaseValues_t :
			public configTransform_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			inline static constexpr auto DEFAULT_FLAGS =
				Data::FlagsBase::kDropOnDeath |
				Data::FlagsBase::kSyncReferenceTransform |
				Data::FlagsBase::kPlaySound |
				Data::FlagsBase::kReferenceMode;

			stl::flag<Data::FlagsBase> flags{ DEFAULT_FLAGS };
			NodeDescriptor targetNode;

		protected:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& static_cast<configTransform_t&>(*this);
				ar& flags.value;
				ar& targetNode;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	IED::Data::configBaseValues_t,
	IED::Data::configBaseValues_t::Serialization::DataVersion1);
