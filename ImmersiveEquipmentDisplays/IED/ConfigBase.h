#pragma once

#include "CollectorData.h"
#include "ConfigBaseValues.h"
#include "ConfigData.h"
#include "ConfigEffectShader.h"
#include "ConfigEquipment.h"
#include "ConfigTransform.h"
#include "NodeDescriptor.h"

namespace IED
{
	struct ObjectEntrySlot;

	struct formSlotPair_t
	{
		formSlotPair_t(
			TESForm* a_form) noexcept;

		formSlotPair_t(
			TESForm*              a_form,
			Data::ObjectSlotExtra a_slot) noexcept;

		inline constexpr formSlotPair_t(
			TESForm*              a_form,
			Data::ObjectSlotExtra a_slotex,
			Data::ObjectSlot      a_slot) noexcept :
			form(a_form),
			slot(a_slotex),
			slot2(a_slot)
		{
		}

		TESForm*              form;
		Data::ObjectSlotExtra slot;
		Data::ObjectSlot      slot2;
	};

	struct CommonParams;
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

			bool run_filters(const CommonParams& a_params) const noexcept;

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

		public:
			enum Serialization : unsigned int
			{
				DataVersion1 = 1,
				DataVersion2 = 2,
				DataVersion3 = 3,
			};

			equipmentOverrideList_t equipmentOverrides;
			effectShaderList_t      effectShaders;
			configFixedStringSet_t  hkxFilter;

			const equipmentOverride_t* get_equipment_override(
				processParams_t& a_params) const noexcept;

			static const equipmentOverride_t* get_equipment_override(
				processParams_t&               a_params,
				const equipmentOverrideList_t& a_list) noexcept;

			const equipmentOverride_t* get_equipment_override_fp(
				const formSlotPair_t& a_checkForm,
				processParams_t&      a_params) const noexcept;

			static const equipmentOverride_t* get_equipment_override_fp(
				const formSlotPair_t&          a_checkForm,
				processParams_t&               a_params,
				const equipmentOverrideList_t& a_list) noexcept;

			const equipmentOverride_t* get_equipment_override_sfp(
				const formSlotPair_t& a_checkForm,
				processParams_t&      a_params) const noexcept;

			static const equipmentOverride_t* get_equipment_override_sfp(
				const formSlotPair_t&          a_checkForm,
				processParams_t&               a_params,
				const equipmentOverrideList_t& a_list) noexcept;

			const configEffectShaderHolder_t* get_effect_shader(
				processParams_t& a_params) const noexcept;

			const configEffectShaderHolder_t* get_effect_shader_fp(
				const formSlotPair_t& a_checkForm,
				processParams_t&      a_params) const noexcept;

			const configEffectShaderHolder_t* get_effect_shader_sfp(
				const formSlotPair_t& a_checkForm,
				processParams_t&      a_params) const noexcept;

		private:
			static bool match_equipped_type(
				processParams_t&                    a_params,
				const equipmentOverrideCondition_t& a_match) noexcept;

			static bool match_carried_type(
				const CollectorData&                a_data,
				const equipmentOverrideCondition_t& a_match) noexcept;

			static bool match_equipped_form(
				processParams_t&                    a_params,
				const equipmentOverrideCondition_t& a_match) noexcept;

			static bool match_carried_form(
				const CollectorData&                a_data,
				const equipmentOverrideCondition_t& a_match) noexcept;

			static bool match_equipped(
				const equipmentOverrideCondition_t& a_match,
				processParams_t&                    a_params) noexcept;

			static bool match_equipped_or_slot(
				const equipmentOverrideCondition_t& a_match,
				processParams_t&                    a_params) noexcept;

			template <
				EquipmentOverrideConditionFlags a_maskAll,
				EquipmentOverrideConditionFlags a_maskSlots>
			static bool match_equipped_or_form(
				const equipmentOverrideCondition_t& a_match,
				const formSlotPair_t&               a_checkForm,
				processParams_t&                    a_params) noexcept;

		public:
			static bool do_match(
				const equipmentOverrideConditionList_t& a_matches,
				processParams_t&                        a_params,
				bool                                    a_default) noexcept;

			static bool do_match_fp(
				const equipmentOverrideConditionList_t& a_matches,
				const formSlotPair_t&                   a_checkForm,
				processParams_t&                        a_params,
				bool                                    a_default) noexcept;

			static bool do_match_sfp(
				const equipmentOverrideConditionList_t& a_matches,
				const formSlotPair_t&                   a_checkForm,
				processParams_t&                        a_params,
				bool                                    a_default) noexcept;

			static bool do_match_eos(
				const equipmentOverrideConditionList_t& a_matches,
				processParams_t&                        a_params,
				bool                                    a_default) noexcept;

		private:
			static bool has_keyword_equipped(
				const configCachedForm_t& a_keyword,
				processParams_t&          a_params) noexcept;

			static bool has_keyword_carried(
				const configCachedForm_t& a_keyword,
				const CollectorData&      a_data) noexcept;

			static bool has_keyword_slot(
				const configCachedForm_t& a_keyword,
				CommonParams&             a_params) noexcept;

			static bool has_keyword_equipped(
				const configCachedForm_t& a_keyword,
				ObjectSlotExtra           a_type,
				const CollectorData&      a_data) noexcept;

			static bool has_keyword_carried(
				const configCachedForm_t& a_keyword,
				ObjectTypeExtra           a_type,
				const CollectorData&      a_data) noexcept;

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

					if (a_version >= DataVersion3)
					{
						a_ar& hkxFilter;
					}
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
	::IED::Data::configBase_t::Serialization::DataVersion3);
