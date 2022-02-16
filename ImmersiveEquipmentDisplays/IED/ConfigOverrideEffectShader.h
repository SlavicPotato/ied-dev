#pragma once

#include "ConfigOverrideColor.h"
#include "ConfigUUIDTag.h"

namespace IED
{
	namespace Data
	{
		enum class EffectShaderDataFlags : std::uint32_t
		{
			kNone = 0,

			kTargetAllNodes = 1u << 0
		};

		DEFINE_ENUM_CLASS_BITWISE(EffectShaderDataFlags);

		struct configEffectShaderData_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			stl::flag<EffectShaderDataFlags> flags{ EffectShaderDataFlags::kNone };
			stl::set<stl::fixed_string>      targetNodes;
			std::string                      baseTexture;
			configColorRGBA_t                fillColor;
			configColorRGBA_t                rimColor;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& targetNodes;
				ar& baseTexture;
				ar& fillColor;
				ar& rimColor;
			}
		};

		enum class EffectShaderHolderFlags : std::uint32_t
		{
			kNone = 0,

			kEnabled = 1u << 0,
		};

		DEFINE_ENUM_CLASS_BITWISE(EffectShaderHolderFlags);

		struct configEffectShaderHolder_t : 
			configUUIDTag_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			[[nodiscard]] inline constexpr bool enabled() const noexcept
			{
				return flags.test(EffectShaderHolderFlags::kEnabled);
			}

			[[nodiscard]] inline bool empty() const noexcept
			{
				return data.empty();
			}

			stl::flag<EffectShaderHolderFlags>                              flags{ EffectShaderHolderFlags::kNone };
			std::unordered_map<stl::fixed_string, configEffectShaderData_t> data;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& data;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	IED::Data::configEffectShaderData_t,
	IED::Data::configEffectShaderData_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configEffectShaderHolder_t,
	IED::Data::configEffectShaderHolder_t::Serialization::DataVersion1);
