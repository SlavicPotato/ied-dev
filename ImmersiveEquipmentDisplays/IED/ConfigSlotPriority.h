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

			constexpr configSlotPriority_t() noexcept
			{
				using enum_type = std::underlying_type_t<ObjectType>;

				for (enum_type i = 0; i < stl::underlying(ObjectType::kMax); i++)
				{
					order[i] = static_cast<Data::ObjectType>(i);
				}
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

			inline constexpr void clear() noexcept
			{
				*this = {};
			}

			constexpr bool validate() const
			{
				auto tmp = order;

				assert(!tmp.empty());

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

			SKMP_REDEFINE_NEW_PREF();

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
