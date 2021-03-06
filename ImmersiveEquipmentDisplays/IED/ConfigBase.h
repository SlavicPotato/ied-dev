#pragma once

#include "CollectorData.h"
#include "CommonParams.h"
#include "ConfigBaseValues.h"
#include "ConfigData.h"
#include "ConfigEffectShader.h"
#include "ConfigEquipment.h"
#include "ConfigTransform.h"
#include "NodeDescriptor.h"

namespace IED
{
	struct objectEntrySlot_t;

	struct formSlotPair_t
	{
		TESForm*              form;
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
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& actorFilter;
				a_ar& npcFilter;
				a_ar& raceFilter;
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
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& filters;
			}
		};

		struct configBase_t :
			configBaseValues_t,
			configBaseFiltersHolder_t
		{
		private:
			friend class boost::serialization::access;

			using slot_container_type = std::array<objectEntrySlot_t, stl::underlying(Data::ObjectSlot::kMax)>;

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2
			};

			equipmentOverrideList_t equipmentOverrides;
			effectShaderList_t      effectShaders;

			const equipmentOverride_t* get_equipment_override(
				const collectorData_t& a_data,
				CommonParams&          a_params) const;

			const equipmentOverride_t* get_equipment_override(
				const collectorData_t& a_data,
				const formSlotPair_t&  a_checkForm,
				CommonParams&          a_params) const;

			const equipmentOverride_t* get_equipment_override(
				const collectorData_t&     a_data,
				const slot_container_type& a_slots,
				const formSlotPair_t&      a_checkForm,
				CommonParams&              a_params) const;

			const configEffectShaderHolder_t* get_effect_shader(
				const collectorData_t& a_data,
				CommonParams&          a_params) const;

			const configEffectShaderHolder_t* get_effect_shader(
				const collectorData_t& a_data,
				const formSlotPair_t&  a_checkForm,
				CommonParams&          a_params) const;

			const configEffectShaderHolder_t* get_effect_shader(
				const collectorData_t&     a_data,
				const slot_container_type& a_slots,
				const formSlotPair_t&      a_checkForm,
				CommonParams&              a_params) const;

		private:
			static constexpr bool match_equipped_type(
				const collectorData_t&              a_data,
				const equipmentOverrideCondition_t& a_match);

			static bool match_carried_type(
				const collectorData_t&              a_data,
				const equipmentOverrideCondition_t& a_match);

			static bool match_equipped_form(
				const collectorData_t&              a_data,
				const equipmentOverrideCondition_t& a_match);

			static bool match_carried_form(
				const collectorData_t&              a_data,
				const equipmentOverrideCondition_t& a_match);

			static constexpr bool match_equipped(
				const collectorData_t&              a_data,
				const equipmentOverrideCondition_t& a_match,
				CommonParams&                       a_params);

			static constexpr bool do_match(
				const collectorData_t&                  a_data,
				const equipmentOverrideConditionList_t& a_matches,
				CommonParams&                           a_params,
				bool                                    a_default);

			static constexpr bool match_equipped_or_slot(
				const collectorData_t&              a_cdata,
				const slot_container_type&          a_data,
				const equipmentOverrideCondition_t& a_match,
				CommonParams&                       a_params);

			template <
				EquipmentOverrideConditionFlags a_maskAll,
				EquipmentOverrideConditionFlags a_maskSlots>
			static constexpr bool match_equipped_or_form(
				const collectorData_t&              a_data,
				const equipmentOverrideCondition_t& a_match,
				const formSlotPair_t&               a_checkForm,
				CommonParams&                       a_params);

		public:
			static bool do_match(
				const collectorData_t&                  a_data,
				const equipmentOverrideConditionList_t& a_matches,
				const formSlotPair_t&                   a_checkForm,
				CommonParams&                           a_params,
				bool                                    a_default);

		private:
			static constexpr bool do_match(
				const collectorData_t&                  a_data,
				const equipmentOverrideConditionList_t& a_matches,
				const slot_container_type&              a_slotData,
				const formSlotPair_t&                   a_checkForm,
				CommonParams&                           a_params,
				bool                                    a_default);

			static bool has_keyword_equipped(
				const configCachedForm_t& a_keyword,
				const collectorData_t&    a_data);

			static bool has_keyword_carried(
				const configCachedForm_t& a_keyword,
				const collectorData_t&    a_data);

			static bool has_keyword(
				const configCachedForm_t&  a_keyword,
				const slot_container_type& a_data);

			static bool has_keyword(
				const configCachedForm_t& a_keyword,
				TESForm*                  a_form);

			static bool has_keyword_equipped(
				const configCachedForm_t& a_keyword,
				ObjectSlotExtra           a_type,
				const collectorData_t&    a_data);

			static bool has_keyword_carried(
				const configCachedForm_t& a_keyword,
				ObjectTypeExtra           a_type,
				const collectorData_t&    a_data);

			static bool has_keyword(
				const configCachedForm_t&  a_keyword,
				ObjectSlot                 a_type,
				const slot_container_type& a_data);

			static TESForm* match_slot_form(
				const slot_container_type&          a_data,
				const equipmentOverrideCondition_t& a_match);

		protected:
			template <class Archive>
			void serialize(Archive& a_ar, const unsigned int a_version)
			{
				a_ar& static_cast<configBaseValues_t&>(*this);
				a_ar& static_cast<configBaseFiltersHolder_t&>(*this);
				a_ar& equipmentOverrides;

				if (a_version >= DataVersion2)
				{
					a_ar& effectShaders;
				}
			}
		};

	}
}

BOOST_CLASS_VERSION(
	::IED::Data::configBaseFilters_t,
	::IED::Data::configBaseFilters_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configBaseFiltersHolder_t,
	::IED::Data::configBaseFiltersHolder_t::Serialization::DataVersion1);

BOOST_CLASS_VERSION(
	::IED::Data::configBase_t,
	::IED::Data::configBase_t::Serialization::DataVersion2);
