#pragma once

#include "ConfigOverrideCommon.h"
#include "ConfigOverrideEffectShader.h"
#include "ConfigOverrideTransform.h"

namespace IED
{
	namespace Data
	{
		enum class ConfigModelGroupEntryFlags : std::uint32_t
		{
			kNone = 0,

			kDropOnDeath       = 1u << 1,
			kKeepTorchFlame    = 1u << 2,
			kRemoveScabbard    = 1u << 3,
			kLoad1pWeaponModel = 1u << 4,
			kDisableHavok      = 1u << 5,
			kLeftWeapon        = 1u << 6,
			kUseWorldModel     = 1u << 7
		};

		DEFINE_ENUM_CLASS_BITWISE(ConfigModelGroupEntryFlags);

		struct configModelGroupEntry_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<ConfigModelGroupEntryFlags> flags{ ConfigModelGroupEntryFlags::kNone };
			configCachedForm_t                    form;
			configTransform_t                     transform;
			//configEffectShaderHolder_t effects;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& form;
				ar& transform;
			}
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
			using data_type = std::unordered_map<stl::fixed_string, configModelGroupEntry_t>;

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
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& entries;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	IED::Data::configModelGroup_t,
	IED::Data::configModelGroup_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configModelGroupEntry_t,
	IED::Data::configModelGroupEntry_t::Serialization::DataVersion1);
