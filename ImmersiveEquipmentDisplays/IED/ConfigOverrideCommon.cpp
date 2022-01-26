#include "pch.h"

#include "ConfigOverrideCommon.h"

#include "GlobalProfileManager.h"

namespace IED
{
	namespace Data
	{
		Game::FormID resolve_form(Game::FormID a_form)
		{
			auto intfc = ISKSE::GetSingleton().GetInterface<SKSESerializationInterface>();

			Game::FormID tmp;
			if (ISKSE::ResolveFormID(intfc, a_form, tmp))
			{
				if (auto form = tmp.Lookup())
				{
					if (form->IsDeleted())
					{
						gLog.Warning("%s: form %.8X is deleted", __FUNCTION__, a_form.get());
					}
				}
				else
				{
					gLog.Debug("%s: form %.8X not found", __FUNCTION__, a_form.get());
				}

				return tmp;
			}
			else
			{
				gLog.Error("%s: failed resolving %.8X", __FUNCTION__, a_form.get());
				return {};
			}
		}

		TESForm* configCachedForm_t::lookup_form(
			Game::FormID a_form) noexcept
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
			case TESSoulGem::kTypeID:
			case TESKey::kTypeID:
			case TESAmmo::kTypeID:
			case TESQuest::kTypeID:
			case TESRace::kTypeID:
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