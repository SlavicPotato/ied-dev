#include "pch.h"

#include "IFormDatabase.h"

#if defined(IED_ENABLE_OUTFIT) && defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
#	include "IED/OM/PersistentOutfitFormManager.h"
#endif

#include <ext/TESWeather.h>

namespace IED
{
	IFormDatabase IFormDatabase::m_Instance;

	void IFormDatabase::QueueGetFormDatabase(
		form_db_get_func_t a_func)
	{
		ITaskPool::AddTask([this, func = std::move(a_func)] {
			func(GetDatabase());
		});
	}

	auto IFormDatabase::GetDatabase()
		-> result_type
	{
		if (auto data = m_data.lock())
		{
			return data;
		}
		else
		{
			return CreateOrGetDatabase();
		}
	}

	namespace detail
	{
		template <class T, class _Form = T>
		static void populate(
			IFormDatabase::container_type& a_data,
			const RE::BSTArray<T>&         a_formData)
		{
			using form_type = stl::strip_type<_Form>;

			auto  r      = a_data.try_emplace(form_type::kTypeID);
			auto& holder = r.first->second;

			if (r.second)
			{
				holder.reserve(a_formData.size());
			}

			for (auto& e : a_formData)
			{
				auto form = static_cast<form_type*>(e);

				if (!form)
				{
					continue;
				}

				if (form->formType != form_type::kTypeID)
				{
					continue;
				}

				if constexpr (std::is_same_v<form_type, TESObjectARMO>)
				{
					if (!form->IsShield())
					{
						continue;
					}
				}
				else if constexpr (std::is_same_v<form_type, TESObjectLIGH>)
				{
					if (!form->CanCarry())
					{
						continue;
					}
				}

				holder.emplace_back(
					e->formID,
					IFormCommon::GetFormFlags(form),
					IFormCommon::GetFormName(form));
			}

			std::sort(
				holder.begin(),
				holder.end(),
				[](auto& a_rhs, auto& a_lhs) {
					return a_rhs.formid < a_lhs.formid;
				});
		}

		template <class T, class _Form = T, class Tf>
		static void populate(
			IFormDatabase::container_type& a_data,
			const RE::BSTArray<T>&         a_formData,
			std::uint32_t                  a_type,
			Tf                             a_func)
		{
			using form_type = stl::strip_type<_Form>;

			auto  r      = a_data.try_emplace(a_type);
			auto& holder = r.first->second;

			if (r.second)
			{
				holder.reserve(a_formData.size());
			}

			for (auto& e : a_formData)
			{
				if (!e)
				{
					continue;
				}

				if (e->formType != form_type::kTypeID)
				{
					continue;
				}

				if (!a_func(e))
				{
					continue;
				}

				holder.emplace_back(
					e->formID,
					IFormCommon::GetFormFlags(e),
					IFormCommon::GetFormName(e));
			}

			std::sort(
				holder.begin(),
				holder.end(),
				[](auto& a_rhs, auto& a_lhs) {
					return a_rhs.formid < a_lhs.formid;
				});
		}
	}

	auto IFormDatabase::CreateOrGetDatabase()
		-> result_type
	{
		const stl::lock_guard lock(m_lock);

		if (auto data = m_data.lock())
		{
			return data;
		}
		else
		{
			auto p = Create();
			m_data = p;
			return p;
		}
	}

	auto IFormDatabase::Create()
		-> result_type
	{
		auto result = std::make_shared<container_type>();

		if (auto dh = DataHandler::GetSingleton())
		{
			detail::populate(*result, dh->weapons);
			detail::populate(*result, dh->armors);
			detail::populate(*result, dh->miscObjects);
			detail::populate(*result, dh->arrSLGM);
			detail::populate(*result, dh->arrLIGH);
			detail::populate(*result, dh->arrSTAT);
			detail::populate(*result, dh->keys);
			detail::populate(*result, dh->books);
			detail::populate(*result, dh->ingredients);
			detail::populate(*result, dh->potions);
			detail::populate(*result, dh->scrolls);
			detail::populate(*result, dh->ammo);
			detail::populate(*result, dh->keywords);
			detail::populate(*result, dh->npcs);
			detail::populate(*result, dh->arrFLOR);
			detail::populate(*result, dh->arrFURN);
			detail::populate(*result, dh->spells);
			detail::populate(*result, dh->races);
			detail::populate(*result, dh->arrARTO);
			detail::populate(*result, dh->arrTREE);
			detail::populate(*result, dh->arrACTI);
			detail::populate(*result, dh->arrTACT);
			detail::populate(*result, dh->arrSNDR);
			detail::populate(*result, dh->quests);
			detail::populate(*result, dh->arrGRAS);
			detail::populate(*result, dh->arrANIO);
			detail::populate(*result, dh->arrLCTN);
			detail::populate(*result, dh->arrWRLD);
			detail::populate(*result, dh->arrPACK);
			detail::populate(*result, dh->arrSHOU);
			detail::populate(*result, dh->arrFACT);
			detail::populate(*result, dh->arrCSTY);
			detail::populate(*result, dh->arrCLAS);
			detail::populate(*result, dh->arrWTHR);
			detail::populate(*result, dh->arrGLOB);
			detail::populate(*result, dh->arrPROJ);
			detail::populate(*result, dh->arrEffectSettings);
			detail::populate(*result, dh->arrPERK);
			detail::populate(*result, dh->arrHAZD);
			detail::populate(*result, dh->arrEQUP);
			detail::populate(*result, dh->arrLGTM);

#if defined(IED_ENABLE_OUTFIT)

			detail::populate(*result, dh->arrOTFT);

#	if defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
			{
				auto  r      = result->try_emplace(BGSOutfit::kTypeID);
				auto& holder = r.first->second;

				const auto& sm   = OM::PersistentOutfitFormManager::GetSingleton();
				const auto  lock = sm.GetScopedLock();

				for (auto& e : sm.GetForms())
				{
					const auto form = e.second->get();

					holder.emplace_back(
						form->formID,
						IFormCommon::GetFormFlags(form),
						IFormCommon::GetFormName(form));
				}
			}
#	endif

			detail::populate(*result, dh->arrLVLI);

#endif

			//Populate(*result, dh->arrIDLE);
			detail::populate<TESForm*, BGSMovableStatic*>(*result, dh->arrMSTT);

			detail::populate(
				*result,
				dh->armors,
				EXTRA_TYPE_ARMOR,
				[](const auto& a_armor) {
					return !a_armor->IsShield();
				});

			detail::populate(
				*result,
				dh->arrLIGH,
				EXTRA_TYPE_LIGHT,
				[](const auto& a_light) {
					return !a_light->CanCarry();
				});
		}

		return result;
	}
}