#include "pch.h"

#include "IFormDatabase.h"

#include <ext/TESWeather.h>

namespace IED
{
	auto IFormDatabase::GetFormDatabase()
		-> result_type
	{
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

	template <class T, class Tf>
	constexpr void IFormDatabase::Populate(
		data_type&             a_data,
		const RE::BSTArray<T>& a_fromData)
	{
		using form_type = stl::strip_type<Tf>;

		auto  r      = a_data.try_emplace(form_type::kTypeID);
		auto& holder = r.first->second;

		if (r.second)
		{
			holder.reserve(a_fromData.size());
		}

		for (auto& e : a_fromData)
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
				GetFormFlags(form),
				GetFormName(form));
		}

		std::sort(
			holder.begin(),
			holder.end(),
			[](auto& a_rhs, auto& a_lhs) {
				return a_rhs.formid < a_lhs.formid;
			});
	}

	template <class T, class Tf>
	constexpr void IFormDatabase::Populate2(
		data_type&             a_data,
		const RE::BSTArray<T>& a_fromData,
		std::uint32_t          a_type,
		Tf                     a_func)
	{
		using form_type = stl::strip_type<T>;

		auto  r      = a_data.try_emplace(a_type);
		auto& holder = r.first->second;

		if (r.second)
		{
			holder.reserve(a_fromData.size());
		}

		for (auto& e : a_fromData)
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
				GetFormFlags(e),
				GetFormName(e));
		}

		std::sort(
			holder.begin(),
			holder.end(),
			[](auto& a_rhs, auto& a_lhs) {
				return a_rhs.formid < a_lhs.formid;
			});
	}

	auto IFormDatabase::Create()
		-> result_type
	{
		auto result = std::make_shared<data_type>();

		if (auto dh = DataHandler::GetSingleton())
		{
			Populate(*result, dh->weapons);
			Populate(*result, dh->armors);
			Populate(*result, dh->miscObjects);
			Populate(*result, dh->arrSLGM);
			Populate(*result, dh->arrLIGH);
			Populate(*result, dh->arrSTAT);
			Populate(*result, dh->keys);
			Populate(*result, dh->books);
			Populate(*result, dh->ingredients);
			Populate(*result, dh->potions);
			Populate(*result, dh->scrolls);
			Populate(*result, dh->ammo);
			Populate(*result, dh->keywords);
			Populate(*result, dh->npcs);
			Populate(*result, dh->arrFLOR);
			Populate(*result, dh->arrFURN);
			Populate(*result, dh->spells);
			Populate(*result, dh->races);
			Populate(*result, dh->arrARTO);
			Populate(*result, dh->arrTREE);
			Populate(*result, dh->arrACTI);
			Populate(*result, dh->arrTACT);
			Populate(*result, dh->arrSNDR);
			Populate(*result, dh->quests);
			Populate(*result, dh->arrGRAS);
			Populate(*result, dh->arrANIO);
			Populate(*result, dh->arrLCTN);
			Populate(*result, dh->arrWRLD);
			Populate(*result, dh->arrPACK);
			Populate(*result, dh->arrSHOU);
			Populate(*result, dh->arrFACT);
			Populate(*result, dh->arrCSTY);
			Populate(*result, dh->arrCLAS);
			Populate(*result, dh->arrWTHR);
			Populate(*result, dh->arrGLOB);
			Populate(*result, dh->arrPROJ);
			Populate(*result, dh->arrEffectSettings);
			//Populate(*result, dh->arrIDLE);
			Populate<TESForm*, BGSMovableStatic*>(*result, dh->arrMSTT);

			Populate2(
				*result,
				dh->armors,
				EXTRA_TYPE_ARMOR,
				[](const auto& a_armor) {
					return !a_armor->IsShield();
				});
		}

		return result;
	}

}