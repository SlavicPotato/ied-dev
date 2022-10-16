#pragma once

#include "ConfigCommon.h"
#include "ConfigEquipment.h"

#include "ConditionalVariableStorage.h"

namespace IED
{
	namespace Data
	{

		enum class ConditionalVariableFlags : std::uint32_t
		{
			kNone = 0,
		};

		DEFINE_ENUM_CLASS_BITWISE(ConditionalVariableFlags);

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
				ConditionalVariableType  a_type,
				const stl::fixed_string& a_name) :
				storage{ a_type },
				name(a_name)
			{
			}

			inline constexpr configConditionalVariable_t(
				ConditionalVariableType a_type,
				stl::fixed_string&&     a_name) :
				storage{ a_type },
				name(std::move(a_name))
			{
			}

			[[nodiscard]] inline constexpr bool operator==(
				const configConditionalVariable_t& a_rhs) const noexcept
			{
				return name == a_rhs.name && storage == a_rhs.storage;
			}

			stl::flag<ConditionalVariableFlags> flags{ ConditionalVariableFlags::kNone };
			stl::fixed_string                   name;
			conditionalVariableStorage_t        storage;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& name;
				a_ar& storage;
			}
		};

		enum class ConditionalVariablesEntryFlags : std::uint32_t
		{
			kNone = 0,

			kIsGroup = 1u << 0
		};

		DEFINE_ENUM_CLASS_BITWISE(ConditionalVariablesEntryFlags);

		struct configConditionalVariablesEntry_t;

		using configConditionalVariablesEntryList_t = stl::boost_vector<configConditionalVariablesEntry_t>;

		using configConditionalVariablesList_t = stl::boost_vector<configConditionalVariable_t>;

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
				const std::string& a_desc) :
				desc(a_desc)
			{
			}

			stl::flag<ConditionalVariablesEntryFlags> flags{ ConditionalVariablesEntryFlags::kNone };
			stl::fixed_string                         desc;
			configConditionalVariablesList_t          vars;
			equipmentOverrideConditionList_t          conditions;
			configConditionalVariablesEntryList_t     group;

		private:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& flags.value;
				a_ar& desc;
				a_ar& vars;
				a_ar& conditions;
				a_ar& group;
			}
		};

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
	::IED::Data::configConditionalVariable_t,
	::IED::Data::configConditionalVariable_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configConditionalVariablesEntry_t,
	::IED::Data::configConditionalVariablesEntry_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configConditionalVariablesHolder_t,
	::IED::Data::configConditionalVariablesHolder_t::Serialization::DataVersion1);
