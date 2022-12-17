#pragma once

#include "ConfigData.h"

namespace IED
{
	namespace Data
	{
		enum class SlotPriorityFlags : std::uint32_t
		{
			kNone = 0,

			kAccountForEquipped = 1u << 0
		};

		DEFINE_ENUM_CLASS_BITWISE(SlotPriorityFlags);

		namespace detail
		{
			inline constexpr auto make_type_index_array() noexcept
			{
				return stl::make_array<
					ObjectType,
					stl::underlying(Data::ObjectType::kMax)>([&]<std::size_t I>() {
					return static_cast<Data::ObjectType>(I);
				});
			}
		}

		struct configSlotPriority_t
		{
			friend class boost::serialization::access;

		public:
			using order_data_type = std::array<
				ObjectType,
				stl::underlying(ObjectType::kMax)>;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			constexpr configSlotPriority_t() noexcept :
				order{ detail::make_type_index_array() }
			{
			}

			inline constexpr ObjectType translate_type_safe(
				std::underlying_type_t<ObjectType> a_in) const noexcept
			{
				assert(a_in < stl::underlying(ObjectType::kMax));

				auto result = order[a_in];
				if (result >= ObjectType::kMax)
				{
					result = static_cast<ObjectType>(a_in);
				}
				return result;
			}
			
			inline constexpr ObjectType translate_type_safe(
				ObjectType a_in) const noexcept
			{
				return translate_type_safe(a_in);
			}

			inline constexpr void clear()  //
				noexcept(std::is_nothrow_move_assignable_v<configSlotPriority_t>&&
			                 std::is_nothrow_default_constructible_v<configSlotPriority_t>)
			{
				*this = {};
			}

			constexpr bool validate() const
			{
				auto tmp = order;

				std::sort(tmp.begin(), tmp.end());

				auto last = *tmp.begin();
				if (last >= ObjectType::kMax)
				{
					return false;
				}

				for (auto it = tmp.cbegin() + 1; it != tmp.cend(); ++it)
				{
					if (*it == last)
					{
						return false;
					}

					last = *it;

					if (last >= ObjectType::kMax)
					{
						return false;
					}
				}

				return true;
			}

			inline static constexpr auto DEFAULT_FLAGS = SlotPriorityFlags::kAccountForEquipped;

			stl::flag<SlotPriorityFlags> flags{ DEFAULT_FLAGS };
			std::uint32_t                limit{ stl::underlying(ObjectType::kMax) };
			order_data_type              order;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& limit;
				a_ar& order;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configSlotPriority_t,
	::IED::Data::configSlotPriority_t::Serialization::DataVersion1);
