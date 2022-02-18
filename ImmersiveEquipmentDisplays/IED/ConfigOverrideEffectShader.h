#pragma once

#include "ConfigOverrideColor.h"
#include "ConfigOverrideEquipment.h"
#include "ConfigUUIDTag.h"

namespace IED
{
	namespace Data
	{
		enum class EffectShaderDataFlags : std::uint32_t
		{
			kNone = 0,

			kGrayscaleToColor        = 1u << 10,
			kGrayscaleToAlpha        = 1u << 11,
			kIgnoreTextureAlpha      = 1u << 12,
			kBaseTextureProjectedUVs = 1u << 13,
			kIgnoreBaseGeomTexAlpha  = 1u << 14,
			kLighting                = 1u << 15,
			kAlpha                   = 1u << 16,
		};

		DEFINE_ENUM_CLASS_BITWISE(EffectShaderDataFlags);

		enum class EffectShaderTextureFlags : std::uint32_t
		{
			kNone = 0,

			kSelectedMask = 0xF,

			kTextureWhite = 1u << 0
		};

		DEFINE_ENUM_CLASS_BITWISE(EffectShaderTextureFlags);

		enum class EffectShaderSelectedTexture : std::uint32_t
		{
			None   = 0,
			White  = 1,
			Grey   = 2,
			Custom = 15
		};

		struct EffectShaderTextureFlagsBitfield
		{
			EffectShaderSelectedTexture selected: 4 { 0 };
			std::uint32_t               unused  : 28 { 0 };
		};

		static_assert(sizeof(EffectShaderTextureFlagsBitfield) == sizeof(EffectShaderDataFlags));

		struct configEffectShaderTexture_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configEffectShaderTexture_t() = default;

			inline constexpr configEffectShaderTexture_t(
				EffectShaderTextureFlags a_flags) noexcept :
				flags{ a_flags }
			{
			}

			union
			{
				stl::flag<EffectShaderTextureFlags> flags{ EffectShaderTextureFlags::kNone };
				EffectShaderTextureFlagsBitfield    fbf;
			};

			std::string path;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& path;
			}
		};

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
			configEffectShaderTexture_t      baseTexture{ EffectShaderTextureFlags::kTextureWhite };
			configEffectShaderTexture_t      paletteTexture;
			configEffectShaderTexture_t      blockOutTexture;
			configColorRGBA_t                fillColor;
			configColorRGBA_t                rimColor;

			bool create_shader_data(RE::BSTSmartPointer<BSEffectShaderData>& a_out) const;

		private:
			static void load_texture(
				const configEffectShaderTexture_t& a_in,
				bool                               a_force_default,
				NiPointer<NiTexture>&              a_out);

			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& targetNodes;
				ar& baseTexture;
				ar& paletteTexture;
				ar& blockOutTexture;
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

		struct configEffectShaderHolder_t
		{
			friend class boost::serialization::access;

		public:
			inline static constexpr auto DEFAULT_FLAGS = EffectShaderHolderFlags::kEnabled;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configEffectShaderHolder_t() = default;

			inline configEffectShaderHolder_t(
				const std::string& a_description) :
				description(a_description)
			{
			}

			[[nodiscard]] inline constexpr bool enabled() const noexcept
			{
				return flags.test(EffectShaderHolderFlags::kEnabled);
			}

			[[nodiscard]] inline bool empty() const noexcept
			{
				return data.empty();
			}

			stl::flag<EffectShaderHolderFlags>                              flags{ DEFAULT_FLAGS };
			std::string                                                     description;
			equipmentOverrideConditionList_t                                conditions;
			std::unordered_map<stl::fixed_string, configEffectShaderData_t> data;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& flags.value;
				ar& description;
				ar& conditions;
				ar& data;
			}
		};

		struct effectShaderList_t :
			configUUIDTag_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			[[nodiscard]] inline constexpr bool empty() const noexcept
			{
				return data.empty();
			}

			std::vector<configEffectShaderHolder_t> data;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
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

BOOST_CLASS_VERSION(
	IED::Data::effectShaderList_t,
	IED::Data::effectShaderList_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configEffectShaderTexture_t,
	IED::Data::configEffectShaderTexture_t::Serialization::DataVersion1);
