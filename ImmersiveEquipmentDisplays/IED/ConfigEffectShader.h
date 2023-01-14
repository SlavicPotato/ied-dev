#pragma once

#include "ConfigColor.h"
#include "ConfigEffectShaderFunction.h"
#include "ConfigEquipment.h"
#include "ConfigLUIDTag.h"

namespace IED
{
	namespace Data
	{
		enum class EffectShaderDataFlags : std::uint32_t
		{
			kNone = 0,

			kForce      = 1u << 0,
			kTargetRoot = 1u << 1,

			kGrayscaleToColor        = 1u << 10,
			kGrayscaleToAlpha        = 1u << 11,
			kIgnoreTextureAlpha      = 1u << 12,
			kBaseTextureProjectedUVs = 1u << 13,
			kIgnoreBaseGeomTexAlpha  = 1u << 14,
			kLighting                = 1u << 15,
			kAlpha                   = 1u << 16,

			EntryMask = kForce | kTargetRoot
		};

		DEFINE_ENUM_CLASS_BITWISE(EffectShaderDataFlags);

		enum class EffectShaderTextureFlags : std::uint32_t
		{
			kNone = 0,

			kTextureWhite = 1u << 0,

			kSelectedMask = 0xF,
		};

		DEFINE_ENUM_CLASS_BITWISE(EffectShaderTextureFlags);

		enum class EffectShaderSelectedTexture : std::uint32_t
		{
			None   = 0,
			White  = 1,
			Grey   = 2,
			Black  = 3,
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

			constexpr configEffectShaderTexture_t(
				EffectShaderTextureFlags a_flags) noexcept :
				flags{ a_flags }
			{
			}

			union
			{
				stl::flag<EffectShaderTextureFlags> flags{ EffectShaderTextureFlags::kNone };
				EffectShaderTextureFlagsBitfield    fbf;
			};

			stl::fixed_string path;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& path;
			}
		};

		struct configEffectShaderData_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2
			};

			static constexpr auto DEFAULT_FLAGS = EffectShaderDataFlags::kNone;

			stl::flag<EffectShaderDataFlags> flags{ DEFAULT_FLAGS };
			TextureAddressMode               textureClampMode{ TextureAddressMode::kWrapSWrapT };
			DepthStencilDepthMode            zTestFunc{ DepthStencilDepthMode::kTest };
			NiAlphaProperty::AlphaFunction   srcBlend{ NiAlphaProperty::AlphaFunction::kSrcAlpha };
			NiAlphaProperty::AlphaFunction   destBlend{ NiAlphaProperty::AlphaFunction::kInvSrcAlpha };
			configFixedStringSet_t           targetNodes;
			configEffectShaderTexture_t      baseTexture{ EffectShaderTextureFlags::kTextureWhite };
			configEffectShaderTexture_t      paletteTexture;
			configEffectShaderTexture_t      blockOutTexture;
			configColorRGBA_t                fillColor;
			configColorRGBA_t                rimColor{ 0.0f, 0.0f, 0.0f, 0.0f };
			float                            baseFillScale{ 1.0f };
			float                            baseFillAlpha{ 1.0f };
			float                            baseRimAlpha{ 1.0f };
			union
			{
				float uvOffset[2]{ 0.0f, 0.0f };
				struct
				{
					float uOffset;
					float vOffset;
				} uvo;
			};
			union
			{
				float uvScale[2]{ 1.0f, 1.0f };
				struct
				{
					float uScale;
					float vScale;
				} uvp;
			};
			float                            edgeExponent{ 1.0f };
			float                            boundDiameter{ 0.0f };
			configEffectShaderFunctionList_t functions;

			[[nodiscard]] bool create_shader_data(RE::BSTSmartPointer<BSEffectShaderData>& a_out) const;

		private:
			static void load_texture(
				const configEffectShaderTexture_t& a_in,
				bool                               a_force_default,
				NiPointer<NiTexture>&              a_out);

			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& targetNodes;
				a_ar& baseTexture;
				a_ar& paletteTexture;
				a_ar& blockOutTexture;
				a_ar& fillColor;
				a_ar& rimColor;
				a_ar& textureClampMode;
				a_ar& baseFillScale;
				a_ar& baseFillAlpha;
				a_ar& baseRimAlpha;
				a_ar& uvo.uOffset;
				a_ar& uvo.vOffset;
				a_ar& uvp.uScale;
				a_ar& uvp.vScale;
				a_ar& edgeExponent;
				a_ar& boundDiameter;

				if (a_version >= DataVersion2)
				{
					a_ar& zTestFunc;
					a_ar& srcBlend;
					a_ar& destBlend;
					a_ar& functions;
				}
			}
		};

		enum class EffectShaderHolderFlags : std::uint32_t
		{
			kNone = 0,

			kEnabled = 1u << 0,
		};

		DEFINE_ENUM_CLASS_BITWISE(EffectShaderHolderFlags);

		struct configEffectShaderHolder_t :
			configLUIDTagAC_t
		{
			friend class boost::serialization::access;

		public:
			static constexpr auto DEFAULT_FLAGS = EffectShaderHolderFlags::kEnabled;

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

			[[nodiscard]] constexpr bool enabled() const noexcept
			{
				return flags.test(EffectShaderHolderFlags::kEnabled);
			}

			stl::flag<EffectShaderHolderFlags>                                    flags{ DEFAULT_FLAGS };
			std::string                                                           description;
			equipmentOverrideConditionList_t                                      conditions;
			stl::boost_unordered_map<stl::fixed_string, configEffectShaderData_t> data;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& description;
				a_ar& conditions;
				a_ar& data;
			}
		};

		struct effectShaderList_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			[[nodiscard]] constexpr bool empty() const noexcept
			{
				return data.empty();
			}

			stl::boost_vector<configEffectShaderHolder_t> data;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& data;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configEffectShaderData_t,
	::IED::Data::configEffectShaderData_t::Serialization::DataVersion2);

BOOST_CLASS_VERSION(
	::IED::Data::configEffectShaderHolder_t,
	::IED::Data::configEffectShaderHolder_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::effectShaderList_t,
	::IED::Data::effectShaderList_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configEffectShaderTexture_t,
	::IED::Data::configEffectShaderTexture_t::Serialization::DataVersion1);
