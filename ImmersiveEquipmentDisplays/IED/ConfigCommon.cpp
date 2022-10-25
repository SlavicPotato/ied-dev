#include "pch.h"

#include "ConfigCommon.h"

#include "GlobalProfileManager.h"

namespace IED
{
	namespace Data
	{
		inline static bool should_discard_form(TESForm* a_form)
		{
			if (!a_form->IsDeleted())
			{
				return false;
			}

			// discard deleted temp references
			auto refr = ::RTTI<TESObjectREFR>()(a_form);
			if (refr && refr->formID.IsTemporary())
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		Game::FormID resolve_form(Game::FormID a_form)
		{
			auto intfc = ISKSE::GetSingleton().GetInterface<SKSESerializationInterface>();

			Game::FormID tmp;
			if (ISKSE::ResolveFormID(intfc, a_form, tmp))
			{
				if (auto form = tmp.Lookup())
				{
					if (should_discard_form(form))
					{
						gLog.Warning(
							"%s: discarding deleted form %.8X [%hhu]",
							__FUNCTION__,
							a_form.get(),
							form->formType);

						return {};
					}
				}

				return tmp;
			}
			else
			{
				gLog.Error("%s: failed resolving %.8X", __FUNCTION__, a_form.get());
				return {};
			}
		}

		Game::FormID resolve_form_zero_missing(Game::FormID a_form)
		{
			auto intfc = ISKSE::GetSingleton().GetInterface<SKSESerializationInterface>();

			Game::FormID tmp;
			if (ISKSE::ResolveFormID(intfc, a_form, tmp))
			{
				if (auto form = tmp.Lookup())
				{
					if (form->IsDeleted())
					{
						gLog.Warning(
							"%s: discarding deleted form %.8X [%hhu]",
							__FUNCTION__,
							a_form.get(),
							form->formType);

						return {};
					}
				}
				else
				{
					gLog.Debug("%s: form %.8X not found", __FUNCTION__, a_form.get());
					return {};
				}

				return tmp;
			}
			else
			{
				gLog.Error("%s: failed resolving %.8X", __FUNCTION__, a_form.get());
				return {};
			}
		}

		void configCachedForm_t::zero_missing_or_deleted()
		{
			auto f = id.Lookup();
			if (!f || f->IsDeleted())
			{
				id = {};
			}
		}

		TESForm* configCachedForm_t::lookup_form(
			Game::FormID a_form)
		{
			if (!a_form || a_form.IsTemporary())
			{
				return nullptr;
			}

			auto form = a_form.Lookup();
			if (!form)
			{
				return nullptr;
			}

			if (form->IsDeleted())
			{
				return nullptr;
			}

			switch (form->formType)
			{
			case TESObjectMISC::kTypeID:
			case TESObjectSTAT::kTypeID:
			case BGSMovableStatic::kTypeID:
			case TESObjectTREE::kTypeID:
			case TESGrass::kTypeID:
			case TESObjectWEAP::kTypeID:
			case TESObjectBOOK::kTypeID:
			case TESObjectACTI::kTypeID:
			case BGSTalkingActivator::kTypeID:
			case TESObjectARMO::kTypeID:
			case TESObjectLIGH::kTypeID:
			case TESObjectANIO::kTypeID:
			case TESObjectDOOR::kTypeID:
			case TESSoulGem::kTypeID:
			case TESKey::kTypeID:
			case TESAmmo::kTypeID:
			case TESQuest::kTypeID:
			case TESRace::kTypeID:
			case TESNPC::kTypeID:
			case TESFlora::kTypeID:
			case TESFurniture::kTypeID:
			case BGSArtObject::kTypeID:
			case IngredientItem::kTypeID:
			case AlchemyItem::kTypeID:
			case ScrollItem::kTypeID:
			case BGSStaticCollection::kTypeID:
			case BGSKeyword::kTypeID:
			case SpellItem::kTypeID:
			case BGSHeadPart::kTypeID:
			case BGSExplosion::kTypeID:
			case BGSMaterialObject::kTypeID:
			case BGSLocation::kTypeID:
			case TESWorldSpace::kTypeID:
			case TESPackage::kTypeID:
			case TESShout::kTypeID:
			case TESFaction::kTypeID:
			case TESCombatStyle::kTypeID:
			case TESClass::kTypeID:
			case RE::TESWeather::kTypeID:
			case TESGlobal::kTypeID:
				return form;
			default:
				return nullptr;
			}
		}

		bool configFormFilter_t::test(Game::FormID a_form) const
		{
			if (filterFlags.test(FormFilterFlags::kUseProfile))
			{
				auto& pmd = GlobalProfileManager::GetSingleton<FormFilterProfile>().Data();

				auto it = pmd.find(profile.name);
				if (it == pmd.end())
				{
					return true;
				}

				return it->second.Data().test(a_form);
			}
			else
			{
				return configFormFilterBase_t::test(a_form);
			}
		}

	}
}