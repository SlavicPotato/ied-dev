#pragma once

#include "CollectorData.h"
#include "ConfigBaseValues.h"
#include "ConfigData.h"
#include "ConfigEffectShader.h"
#include "ConfigEquipment.h"
#include "ConfigTransform.h"
#include "FormSlotPair.h"
#include "NodeDescriptor.h"

namespace IED
{
	struct ObjectEntrySlot;
	struct CommonParams;
	struct ProcessParams;

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
				ProcessParams& a_params) const noexcept;

			static const equipmentOverride_t* get_equipment_override(
				ProcessParams&                 a_params,
				const equipmentOverrideList_t& a_list) noexcept;

			const equipmentOverride_t* get_equipment_override_fp(
				const FormSlotPair& a_checkForm,
				ProcessParams&      a_params) const noexcept;

			static const equipmentOverride_t* get_equipment_override_fp(
				const FormSlotPair&            a_checkForm,
				ProcessParams&                 a_params,
				const equipmentOverrideList_t& a_list) noexcept;

			const equipmentOverride_t* get_equipment_override_sfp(
				const FormSlotPair& a_checkForm,
				ProcessParams&      a_params) const noexcept;

			static const equipmentOverride_t* get_equipment_override_sfp(
				const FormSlotPair&            a_checkForm,
				ProcessParams&                 a_params,
				const equipmentOverrideList_t& a_list) noexcept;

			const configEffectShaderHolder_t* get_effect_shader(
				ProcessParams& a_params) const noexcept;

			const configEffectShaderHolder_t* get_effect_shader_fp(
				const FormSlotPair& a_checkForm,
				ProcessParams&      a_params) const noexcept;

			const configEffectShaderHolder_t* get_effect_shader_sfp(
				const FormSlotPair& a_checkForm,
				ProcessParams&      a_params) const noexcept;

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
