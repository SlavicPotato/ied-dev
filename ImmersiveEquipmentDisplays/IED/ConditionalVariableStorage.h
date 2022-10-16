#pragma once

namespace IED
{

	enum class ConditionalVariableType : std::uint32_t
	{
		kInt32 = 0,
		kFloat = 1
	};

	struct conditionalVariableStorage_t
	{
		friend class boost::serialization::access;

	public:
		enum Serialization : unsigned int
		{
			DataVersion1 = 1
		};

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
			default:
				return false;
			}
		}

		template <class T>
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
		}

		ConditionalVariableType type;

		union
		{
			std::uint8_t bytes[8]{ 0 };
			std::int32_t i32;
			float        f32;
		};

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& type;
			a_ar& bytes;
		}
	};

	using conditionalVariableMap_t =
		stl::unordered_map<
			stl::fixed_string,
			std::pair<
				conditionalVariableStorage_t,
				bool>,
			std::hash<stl::fixed_string>,
			std::equal_to<stl::fixed_string>,
#if defined(IED_USE_MIMALLOC_COLLECTOR)
			stl::mi_allocator<
				std::pair<
					const stl::fixed_string,
					std::pair<
						conditionalVariableStorage_t,
						bool>>>
#else
			stl::container_allocator<
				std::pair<
					const stl::fixed_string,
					std::pair<
						conditionalVariableStorage_t,
						bool>>>
#endif

			>;

}

BOOST_CLASS_VERSION(
	::IED::conditionalVariableStorage_t,
	::IED::conditionalVariableStorage_t::Serialization::DataVersion1);
