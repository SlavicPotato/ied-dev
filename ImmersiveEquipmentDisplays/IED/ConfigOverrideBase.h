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

	namespace Data
	{
		struct configBase_t :
			public configBaseValues_t
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
			configFormFilter_t raceFilter;

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
			SKMP_FORCEINLINE static constexpr bool match_equipped_type(
				const collectorData_t& a_data,
				const equipmentOverrideCondition_t& a_match);

			SKMP_FORCEINLINE static bool match_equipped_form(
				const collectorData_t& a_data,
				const equipmentOverrideCondition_t& a_match);

			SKMP_FORCEINLINE static constexpr bool match(
				const collectorData_t& a_data,
				const equipmentOverrideCondition_t& a_match,
				CommonParams& a_params);

			SKMP_FORCEINLINE static constexpr bool match(
				const collectorData_t& a_data,
				const equipmentOverrideConditionList_t& a_matches,
				CommonParams& a_params,
				bool a_default);

			SKMP_FORCEINLINE static constexpr bool match(
				const collectorData_t& a_cdata,
				const slot_container_type& a_data,
				const equipmentOverrideCondition_t& a_match,
				CommonParams& a_params);

			SKMP_FORCEINLINE static constexpr bool match(
				const collectorData_t& a_cdata,
				const slot_container_type& a_data,
				const equipmentOverrideConditionList_t& a_matches,
				CommonParams& a_params,
				bool a_default);

			SKMP_FORCEINLINE static constexpr bool match(
				const collectorData_t& a_data,
				const equipmentOverrideCondition_t& a_match,
				const formSlotPair_t& a_checkForm,
				CommonParams& a_params);

			SKMP_FORCEINLINE static constexpr bool match(
				const collectorData_t& a_data,
				const equipmentOverrideConditionList_t& a_matches,
				const formSlotPair_t& a_checkForm,
				CommonParams& a_params,
				bool a_default);

			SKMP_FORCEINLINE static bool has_keyword(
				const configCachedForm_t& a_keyword,
				const collectorData_t& a_data);

			SKMP_FORCEINLINE static bool has_keyword(
				const configCachedForm_t& a_keyword,
				const slot_container_type& a_data);

			SKMP_FORCEINLINE static bool has_keyword(
				const configCachedForm_t& a_keyword,
				TESForm* a_form);

			SKMP_FORCEINLINE static bool has_keyword(
				const configCachedForm_t& a_keyword,
				ObjectSlotExtra a_type,
				const collectorData_t& a_data);

			SKMP_FORCEINLINE static bool has_keyword(
				const configCachedForm_t& a_keyword,
				ObjectSlot a_type,
				const slot_container_type& a_data);

			SKMP_FORCEINLINE static TESForm* match_slot_form(
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
				ar& equipmentOverrides;
				ar& raceFilter;
			}
		};

	}
}

BOOST_CLASS_VERSION(
	IED::Data::configBase_t,
	IED::Data::configBase_t::Serialization::DataVersion1);
