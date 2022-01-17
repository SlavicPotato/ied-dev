#pragma once

#include "CollectorData.h"
#include "CommonParams.h"
#include "ConfigOverrideBaseValues.h"
#include "ConfigOverrideCommon.h"
#include "ConfigOverrideEquipment.h"
#include "ConfigOverrideTransform.h"
#include "NodeDescriptor.h"

namespace IED
{
	struct objectEntrySlot_t;

	struct formSlotPair_t
	{
		TESForm* form;
		Data::ObjectSlotExtra slot;
	};

	struct processParams_t;

	namespace Data
	{
		struct configBaseFilters_t
		{
		private:
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configFormFilter_t actorFilter;
			configFormFilter_t npcFilter;
			configFormFilter_t raceFilter;

		private:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& actorFilter;
				ar& npcFilter;
				ar& raceFilter;
			}
		};

		struct configBaseFiltersHolder_t
		{
		private:
			friend class boost::serialization::access;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			configBaseFiltersHolder_t() = default;

			configBaseFiltersHolder_t(
				const configBaseFiltersHolder_t& a_rhs)
			{
				__copy(a_rhs);
			}

			configBaseFiltersHolder_t(configBaseFiltersHolder_t&&) = default;

			configBaseFiltersHolder_t& operator=(
				const configBaseFiltersHolder_t& a_rhs)
			{
				__copy(a_rhs);
				return *this;
			}

			configBaseFiltersHolder_t& operator=(configBaseFiltersHolder_t&&) = default;

			bool run_filters(const processParams_t& a_params) const;

			std::unique_ptr<configBaseFilters_t> filters;

		private:
			void __copy(const configBaseFiltersHolder_t& a_rhs)
			{
				if (a_rhs.filters)
				{
					if (filters)
					{
						*filters = *a_rhs.filters;
					}
					else
					{
						filters = std::make_unique<configBaseFilters_t>(*a_rhs.filters);
					}
				}
				else
				{
					filters.reset();
				}
			}

			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& filters;
			}
		};

		struct configBase_t :
			public configBaseValues_t,
			public configBaseFiltersHolder_t
		{
		private:
			friend class boost::serialization::access;

			using slot_container_type = objectEntrySlot_t[stl::underlying(Data::ObjectSlot::kMax)];

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1
			};

			equipmentOverrideList_t equipmentOverrides;

			const equipmentOverride_t* get_equipment_override(
				const collectorData_t& a_data,
				CommonParams& a_params) const;

			const equipmentOverride_t* get_equipment_override(
				const collectorData_t& a_data,
				const slot_container_type& a_slots,
				CommonParams& a_params) const;

			const equipmentOverride_t* get_equipment_override(
				const collectorData_t& a_data,
				const formSlotPair_t& a_checkForm,
				CommonParams& a_params) const;

		private:
			static constexpr bool match_equipped_type(
				const collectorData_t& a_data,
				const equipmentOverrideCondition_t& a_match);

			static bool match_carried_type(
				const collectorData_t& a_data,
				const equipmentOverrideCondition_t& a_match);

			static bool match_equipped_form(
				const collectorData_t& a_data,
				const equipmentOverrideCondition_t& a_match);
			
			static bool match_carried_form(
				const collectorData_t& a_data,
				const equipmentOverrideCondition_t& a_match);

			static constexpr bool match(
				const collectorData_t& a_data,
				const equipmentOverrideCondition_t& a_match,
				CommonParams& a_params);

			static constexpr bool match(
				const collectorData_t& a_data,
				const equipmentOverrideConditionList_t& a_matches,
				CommonParams& a_params,
				bool a_default);

			static constexpr bool match(
				const collectorData_t& a_cdata,
				const slot_container_type& a_data,
				const equipmentOverrideCondition_t& a_match,
				CommonParams& a_params);

			static constexpr bool match(
				const collectorData_t& a_cdata,
				const slot_container_type& a_data,
				const equipmentOverrideConditionList_t& a_matches,
				CommonParams& a_params,
				bool a_default);

			static constexpr bool match(
				const collectorData_t& a_data,
				const equipmentOverrideCondition_t& a_match,
				const formSlotPair_t& a_checkForm,
				CommonParams& a_params);

			static constexpr bool match(
				const collectorData_t& a_data,
				const equipmentOverrideConditionList_t& a_matches,
				const formSlotPair_t& a_checkForm,
				CommonParams& a_params,
				bool a_default);

			static bool has_keyword_equipped(
				const configCachedForm_t& a_keyword,
				const collectorData_t& a_data);
			
			static bool has_keyword_carried(
				const configCachedForm_t& a_keyword,
				const collectorData_t& a_data);

			static bool has_keyword(
				const configCachedForm_t& a_keyword,
				const slot_container_type& a_data);

			static bool has_keyword(
				const configCachedForm_t& a_keyword,
				TESForm* a_form);

			static bool has_keyword_equipped(
				const configCachedForm_t& a_keyword,
				ObjectSlotExtra a_type,
				const collectorData_t& a_data);
			
			static bool has_keyword_carried(
				const configCachedForm_t& a_keyword,
				ObjectTypeExtra a_type,
				const collectorData_t& a_data);

			static bool has_keyword(
				const configCachedForm_t& a_keyword,
				ObjectSlot a_type,
				const slot_container_type& a_data);

			static TESForm* match_slot_form(
				const slot_container_type& a_data,
				const equipmentOverrideCondition_t& a_match);

		public:
			bool has_equipment_override_form(Game::FormID a_formid) const;
			bool has_equipment_override_type(ObjectTypeExtra a_type) const;

		protected:
			template <class Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ar& static_cast<configBaseValues_t&>(*this);
				ar& static_cast<configBaseFiltersHolder_t&>(*this);
				ar& equipmentOverrides;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	IED::Data::configBaseFilters_t,
	IED::Data::configBaseFilters_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configBaseFiltersHolder_t,
	IED::Data::configBaseFiltersHolder_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	IED::Data::configBase_t,
	IED::Data::configBase_t::Serialization::DataVersion1);
