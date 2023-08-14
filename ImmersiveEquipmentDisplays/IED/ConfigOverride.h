#pragma once

#include "ConfigEquipmentOverrideCondition.h"

namespace IED
{
	namespace Data
	{

		enum class ConfigOverrideFlags : std::uint32_t
		{
			kNone = 0,

			kIsGroup  = 1u << 0,
			kContinue = 1u << 1
		};

		DEFINE_ENUM_CLASS_BITWISE(ConfigOverrideFlags);

		template <class T>
		struct configOverride_t;

		template <class T>
		using configOverrideList_t = stl::boost_vector<configOverride_t<T>>;

		struct equipmentOverrideConditionSet_t;

		template <class T>
		struct configOverride_t :
			public T
		{
			friend class boost::serialization::access;

		public:
			using base_value_type = T;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2,
			};

			configOverride_t() = default;

			template <class Td, class Ts>
			configOverride_t(
				Td&& a_config,
				Ts&& a_desc)  //
				requires(
							std::is_constructible_v<T, Td &&> &&
							std::is_constructible_v<std::string, Ts &&>)
				:
				T(std::forward<Td>(a_config)),
				description(std::forward<Ts>(a_desc))
			{
			}

			template <class Tf>
			void visit(Tf a_func)
			{
				a_func(*this);

				for (auto& e : group)
				{
					a_func(e);
				}
			}

			stl::flag<ConfigOverrideFlags>  overrideFlags{ ConfigOverrideFlags::kNone };
			equipmentOverrideConditionSet_t conditions;
			std::string                     description;
			configOverrideList_t<T>         group;

		protected:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<T&>(*this);
				a_ar& overrideFlags.value;
				a_ar& conditions.list;
				a_ar& description;

				if (a_version >= DataVersion2)
				{
					a_ar& group;
				}
			}
		};

	}
}

BOOST_CLASS_TEMPLATE_VERSION(
	class T,
	::IED::Data::configOverride_t<T>,
	::IED::Data::configOverride_t<T>::Serialization::DataVersion2);
