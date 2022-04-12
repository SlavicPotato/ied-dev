#pragma once

#include "ConfigLUIDTag.h"

namespace IED
{
	namespace Data
	{
		enum class EffectShaderFunctionFlags : std::uint32_t
		{
			kNone = 0u,

			kOpt1 = 1u << 0,
			kOpt2 = 1u << 1,
		};

		DEFINE_ENUM_CLASS_BITWISE(EffectShaderFunctionFlags);

		enum class EffectShaderFunctionType : std::uint16_t
		{
			None           = 0,
			UVLinearMotion = 1,
			Pulse          = 2,
		};

		enum class EffectShaderPulseFlags : std::uint32_t
		{
			kNone = 0u,

			kFillR = 1u << 0,
			kFillG = 1u << 1,
			kFillB = 1u << 2,
			kFillA = 1u << 3,
			kRimR  = 1u << 4,
			kRimG  = 1u << 5,
			kRimB  = 1u << 6,
			kRimA  = 1u << 7,
		};

		DEFINE_ENUM_CLASS_BITWISE(EffectShaderPulseFlags);

		struct configEffectShaderFunction_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configEffectShaderFunction_t() = default;

			inline configEffectShaderFunction_t(
				EffectShaderFunctionType a_type) noexcept :
				type{ a_type }
			{
				switch (a_type)
				{
				case EffectShaderFunctionType::UVLinearMotion:
				case EffectShaderFunctionType::Pulse:
					speed = 1.0f;
					break;
				}
			}

			[[nodiscard]] inline constexpr auto& get_unique_id() const noexcept
			{
				return uniqueID;
			}

			stl::flag<EffectShaderFunctionFlags> flags{ EffectShaderFunctionFlags::kNone };
			EffectShaderFunctionType             type{ EffectShaderFunctionType::None };

			union
			{
				float f32a{ 0.0f };
				float angle;
				float initmod;  // pulse
			};

			union
			{
				float f32b{ 0.0f };
				float speed;
			};

			union
			{
				std::uint32_t          u32a{ 0 };
				EffectShaderPulseFlags pulseFlags;
			};

		private:
			configLUIDTag_t uniqueID;

			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& type;
				a_ar& f32a;
				a_ar& f32b;
				a_ar& u32a;
			}
		};

		using configEffectShaderFunctionList_t = stl::boost_vector<configEffectShaderFunction_t>;

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configEffectShaderFunction_t,
	::IED::Data::configEffectShaderFunction_t::Serialization::DataVersion1);
