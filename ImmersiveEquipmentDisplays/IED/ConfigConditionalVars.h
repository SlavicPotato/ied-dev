#pragma once

#include "ConfigCommon.h"
#include "ConfigEquipment.h"
#include "ConfigLastEquipped.h"

#include "ConditionalVariableStorage.h"

namespace IED
{
	namespace Data
	{

		enum class ConditionalVariableValueDataFlags : std::uint32_t
		{
			kNone = 0,

			kLastEquipped = 1u << 0,
		};

		DEFINE_ENUM_CLASS_BITWISE(ConditionalVariableValueDataFlags);

		struct configConditionalVariableValueData_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configConditionalVariableValueData_t() noexcept = default;

			inline constexpr configConditionalVariableValueData_t(
				ConditionalVariableType a_type) noexcept :
				value(a_type)
			{
			}

			stl::flag<ConditionalVariableValueDataFlags> flags{ ConditionalVariableValueDataFlags::kNone };
			conditionalVariableStorage_t                 value;
			Data::configLastEquipped_t                   lastEquipped;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& value;
				a_ar& lastEquipped;
			}
		};

		enum class ConditionalVariableFlags : std::uint32_t
		{
			kNone = 0,

			kIsGroup  = 1u << 0,
			kContinue = 1u << 1,
		};

		DEFINE_ENUM_CLASS_BITWISE(ConditionalVariableFlags);

		struct configConditionalVariable_t;

		using configConditionalVariablesList_t = stl::boost_vector<configConditionalVariable_t>;

		struct configConditionalVariable_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configConditionalVariable_t() noexcept = default;

			inline constexpr configConditionalVariable_t(
				const stl::fixed_string& a_desc) noexcept :
				desc(a_desc)
			{
			}

			inline constexpr configConditionalVariable_t(
				stl::fixed_string&& a_desc) noexcept :
				desc(std::move(a_desc))
			{
			}

			stl::flag<ConditionalVariableFlags>  flags{ ConditionalVariableFlags::kNone };
			stl::fixed_string                    desc;
			configConditionalVariableValueData_t value;
			equipmentOverrideConditionList_t     conditions;
			configConditionalVariablesList_t     group;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& desc;
				a_ar& value;
				a_ar& conditions;
				a_ar& group;
			}
		};

		enum class ConditionalVariablesEntryFlags : std::uint32_t
		{
			kNone = 0,
		};

		DEFINE_ENUM_CLASS_BITWISE(ConditionalVariablesEntryFlags);

		struct configConditionalVariablesEntry_t
		{
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configConditionalVariablesEntry_t() noexcept = default;

			inline constexpr configConditionalVariablesEntry_t(
				ConditionalVariableType a_type) noexcept :
				defaultValue(a_type)
			{
			}

			stl::flag<ConditionalVariablesEntryFlags> flags{ ConditionalVariablesEntryFlags::kNone };
			configConditionalVariableValueData_t      defaultValue;
			configConditionalVariablesList_t          vars;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& defaultValue;
				a_ar& vars;
			}
		};

		using configConditionalVariablesEntryList_t =
			stl::map_sa<
				stl::fixed_string,
				configConditionalVariablesEntry_t,
				stl::fixed_string_less_equal_ptr,
				stl::boost_container_allocator<
					std::pair<
						stl::fixed_string,
						configConditionalVariablesEntry_t>>>;

		using configConditionalVariablesEntryListValue_t = configConditionalVariablesEntryList_t::value_type;

		enum class ConditionalVariablesHolderFlags : std::uint32_t
		{
			kNone = 0,
		};

		DEFINE_ENUM_CLASS_BITWISE(ConditionalVariablesHolderFlags);

		struct configConditionalVariablesHolder_t
		{
			friend class boost::serialization::access;

		public:
			using data_type = configConditionalVariablesEntryList_t;

			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			inline constexpr void clear() noexcept
			{
				data.clear();
			}

			inline constexpr bool empty() const noexcept
			{
				return data.empty();
			}

			template <class Tf>
			inline constexpr void visit(Tf a_func)
			{
				for (auto& e : data)
				{
					a_func(e);
				}
			}

			stl::flag<ConditionalVariablesHolderFlags> flags{ ConditionalVariablesHolderFlags::kNone };
			data_type                                  data;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& data;
			}
		};
	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configConditionalVariableValueData_t,
	::IED::Data::configConditionalVariableValueData_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configConditionalVariable_t,
	::IED::Data::configConditionalVariable_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configConditionalVariablesEntry_t,
	::IED::Data::configConditionalVariablesEntry_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configConditionalVariablesHolder_t,
	::IED::Data::configConditionalVariablesHolder_t::Serialization::DataVersion1);
