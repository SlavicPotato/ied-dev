#pragma once

#include "ConfigCommon.h"
#include "ConfigNodePhysicsValues.h"
#include "ConfigTransform.h"
#include "NodeDescriptor.h"

namespace IED
{
	namespace Data
	{
		enum class BaseFlags : std::uint32_t
		{
			kNone = 0,

			kReserved0                  = 1u << 0,
			kReserved1                  = 1u << 1,
			kInvisible                  = 1u << 2,
			kHideIfUsingFurniture       = 1u << 3,
			kDropOnDeath                = 1u << 4,
			kDisabled                   = 1u << 5,
			kSyncReferenceTransform     = 1u << 6,
			kReferenceMode              = 1u << 7,
			kPlaySound                  = 1u << 8,
			kKeepTorchFlame             = 1u << 9,
			kRemoveScabbard             = 1u << 10,
			kLoad1pWeaponModel          = 1u << 11,
			kHideLayingDown             = 1u << 12,
			kUseWorldModel              = 1u << 13,
			kIgnoreRaceEquipTypes       = 1u << 14,
			kPlaySequence               = 1u << 15,
			kPapyrus                    = 1u << 16,
			kDisableWeaponAnims         = 1u << 17,
			kDisableAnimEventForwarding = 1u << 18,
			kAnimationEvent             = 1u << 19,
			kDisableHavok               = 1u << 20,
			kRemoveProjectileTracers    = 1u << 21,
			kDynamicArrows              = 1u << 22,
			kAttachLight                = 1u << 23,
			kHideLight                  = 1u << 24,
			kRemoveEditorMarker         = 1u << 25,

			kResetTriggerFlags = kDropOnDeath |
			                     kReferenceMode |
			                     kKeepTorchFlame |
			                     kRemoveScabbard |
			                     kLoad1pWeaponModel |
			                     kUseWorldModel |
			                     kDisableWeaponAnims |
			                     kDisableHavok |
			                     kRemoveProjectileTracers |
			                     kAttachLight |
			                     kRemoveEditorMarker

		};

		DEFINE_ENUM_CLASS_BITWISE(BaseFlags);

		struct configBaseValues_t :
			public configTransform_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2,
				DataVersion3 = 3,
				DataVersion4 = 4,
				DataVersion5 = 5,
				DataVersion6 = 6,
			};

			inline static constexpr auto DEFAULT_FLAGS =
				BaseFlags::kDropOnDeath |
				BaseFlags::kSyncReferenceTransform |
				BaseFlags::kPlaySound |
				BaseFlags::kReferenceMode |
				BaseFlags::kDynamicArrows;

			stl::flag<BaseFlags>                                 flags{ DEFAULT_FLAGS };
			NodeDescriptor                                       targetNode;
			stl::fixed_string                                    niControllerSequence;
			stl::fixed_string                                    animationEvent;
			configCachedForm_t                                   forceModel;
			configCopyableUniquePtr_t<configNodePhysicsValues_t> physicsValues;
			configTransform_t                                    geometryTransform;

		protected:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& static_cast<const configTransform_t&>(*this);
				a_ar& flags.value;
				a_ar& targetNode;
				a_ar& niControllerSequence;
				a_ar& animationEvent;
				a_ar& forceModel;
				a_ar& physicsValues.data;
				a_ar& geometryTransform;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configTransform_t&>(*this);
				a_ar& flags.value;
				a_ar& targetNode;

				if (a_version >= DataVersion2)
				{
					a_ar& niControllerSequence;

					if (a_version >= DataVersion3)
					{
						a_ar& animationEvent;

						if (a_version >= DataVersion4)
						{
							a_ar& forceModel;

							forceModel.zero_missing_or_deleted();

							if (a_version >= DataVersion5)
							{
								a_ar& physicsValues.data;

								if (a_version >= DataVersion6)
								{
									a_ar& geometryTransform;
								}
							}
						}
					}
				}
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};
	}
}

BOOST_CLASS_VERSION(
	IED::Data::configBaseValues_t,
	IED::Data::configBaseValues_t::Serialization::DataVersion6);
