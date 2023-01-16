#pragma once

#include "ConfigData.h"
#include "ConfigEffectShader.h"
#include "ConfigTransform.h"

namespace IED
{
	namespace Data
	{
		enum class ConfigModelGroupEntryFlags : std::uint32_t
		{
			kNone = 0,

			kDropOnDeath                = 1u << 1,
			kKeepTorchFlame             = 1u << 2,
			kRemoveScabbard             = 1u << 3,
			kLoad1pWeaponModel          = 1u << 4,
			kDisableHavok               = 1u << 5,
			kLeftWeapon                 = 1u << 6,
			kUseWorldModel              = 1u << 7,
			kDisabled                   = 1u << 8,
			kPlaySequence               = 1u << 9,
			kDisableWeaponAnims         = 1u << 10,
			kDisableAnimEventForwarding = 1u << 11,
			kAnimationEvent             = 1u << 12,
			kRemoveProjectileTracers    = 1u << 13,
			kAttachLight                = 1u << 14,
			kRemoveEditorMarker         = 1u << 15,
			kForceTryLoadAnim           = 1u << 16,
		};

		DEFINE_ENUM_CLASS_BITWISE(ConfigModelGroupEntryFlags);

		struct configModelGroupEntry_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2,
				DataVersion3 = 3,
			};

			stl::flag<ConfigModelGroupEntryFlags> flags{ ConfigModelGroupEntryFlags::kNone };
			configCachedForm_t                    form;
			configTransform_t                     transform;
			stl::fixed_string                     niControllerSequence;
			stl::fixed_string                     animationEvent;

		private:
			template <class Archive>
			void save(Archive& a_ar, const unsigned int a_version) const
			{
				a_ar& flags.value;
				a_ar& form;
				a_ar& transform;
				a_ar& niControllerSequence;
				a_ar& animationEvent;
			}

			template <class Archive>
			void load(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& form;
				a_ar& transform;

				if (a_version >= DataVersion2)
				{
					a_ar& niControllerSequence;

					if (a_version >= DataVersion3)
					{
						a_ar& animationEvent;
					}
				}

				form.zero_missing_or_deleted();
			}

			BOOST_SERIALIZATION_SPLIT_MEMBER();
		};

		enum class ConfigModelGroupFlags : std::uint32_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(ConfigModelGroupFlags);

		struct configModelGroup_t
		{
			friend class boost::serialization::access;

		public:
			using data_type = stl::boost_unordered_map<stl::fixed_string, configModelGroupEntry_t>;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			[[nodiscard]] inline bool empty() const noexcept
			{
				return entries.empty();
			}

			stl::flag<ConfigModelGroupFlags> flags{ ConfigModelGroupFlags::kNone };
			data_type                        entries;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& entries;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configModelGroup_t,
	::IED::Data::configModelGroup_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configModelGroupEntry_t,
	::IED::Data::configModelGroupEntry_t::Serialization::DataVersion3);
