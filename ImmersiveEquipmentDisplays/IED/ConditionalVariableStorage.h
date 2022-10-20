#pragma once

#include "ConfigCommon.h"

namespace IED
{

	enum class ConditionalVariableType : std::uint32_t
	{
		kInt32 = 0,
		kFloat = 1,
		kForm  = 2
	};

	struct conditionalVariableValue_t
	{
		friend class boost::serialization::access;

	public:
		enum Serialization : unsigned int
		{
			DataVersion1 = 1
		};

		conditionalVariableValue_t() noexcept = default;

		inline constexpr conditionalVariableValue_t(
			Game::FormID a_form) noexcept :
			form(a_form)
		{
		}

		union
		{
			std::uint8_t primitives[8]{ 0 };
			std::int32_t i32;
			float        f32;
		};

		Data::configCachedForm_t form;

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& primitives;
			a_ar& form;
		}
	};

	struct conditionalVariableStorage_t :
		conditionalVariableValue_t
	{
		friend class boost::serialization::access;

	public:
		enum Serialization : unsigned int
		{
			DataVersion1 = 1
		};

		conditionalVariableStorage_t() noexcept = default;

		inline constexpr conditionalVariableStorage_t(
			ConditionalVariableType a_type) noexcept :
			type(a_type)
		{
		}

		inline constexpr conditionalVariableStorage_t(
			Game::FormID a_form) noexcept :
			conditionalVariableValue_t(a_form),
			type(ConditionalVariableType::kForm)
		{
		}

		[[nodiscard]] inline constexpr bool operator==(
			const conditionalVariableStorage_t& a_rhs) const noexcept
		{
			if (type != a_rhs.type)
			{
				return false;
			}

			switch (type)
			{
			case ConditionalVariableType::kInt32:
				return i32 == a_rhs.i32;
			case ConditionalVariableType::kFloat:
				return f32 == a_rhs.f32;
			case ConditionalVariableType::kForm:
				return form == a_rhs.form;
			default:
				return false;
			}
		}

		/*template <class T>
		inline constexpr bool set_value(T a_value) noexcept
		{
			bool result;

			if constexpr (
				std::is_integral_v<T> &&
				std::is_signed_v<T> &&
				sizeof(T) <= sizeof(std::int32_t))
			{
				result = type == ConditionalVariableType::kInt32;
				if (result)
				{
					i32 = static_cast<std::int32_t>(a_value);
				}
			}
			else if constexpr (std::is_floating_point_v<T>)
			{
				result = type == ConditionalVariableType::kFloat;
				if (result)
				{
					f32 = static_cast<float>(a_value);
				}
			}
			else
			{
				static_assert(false, "invalid value type");
			}

			return result;
		}*/

		ConditionalVariableType type{ ConditionalVariableType::kInt32 };

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& static_cast<conditionalVariableValue_t&>(*this);
			a_ar& type;
		}
	};

	using conditionalVariableMap_t =
		stl::unordered_map<
			stl::fixed_string,
			conditionalVariableStorage_t>;

}

BOOST_CLASS_VERSION(
	::IED::conditionalVariableValue_t,
	::IED::conditionalVariableValue_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::conditionalVariableStorage_t,
	::IED::conditionalVariableStorage_t::Serialization::DataVersion1);
