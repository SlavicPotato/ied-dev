#include "pch.h"

#include "BGSOutfitHolder.h"

#include "ConfigStoreOutfit.h"

#include <ext/GarbageCollector.h>

namespace IED
{
	namespace OM
	{
		BGSOutfitHolder::BGSOutfitHolder()
		{
			const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<BGSOutfit>();
			m_form             = factory ? factory->Create() : nullptr;
		}

		BGSOutfitHolder::BGSOutfitHolder(
			Game::FormID             a_formid,
			RE::BSTArray<TESForm*>&& a_items,
			BSFixedString&&          a_name)
		{
			if (const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<BGSOutfit>())
			{
				if (const auto form = factory->Create())
				{
					form->SetFormID(a_formid, true);
					form->outfitItems = std::move(a_items);
					m_name            = std::move(a_name);

					m_form = form;
				}
			}
		}

		BGSOutfitHolder::BGSOutfitHolder(
			BGSOutfit*               a_form,
			RE::BSTArray<TESForm*>&& a_items,
			BSFixedString&&          a_name)
		{
			a_form->outfitItems = std::move(a_items);
			m_name              = std::move(a_name);
			m_form              = a_form;
		}

		BGSOutfitHolder::~BGSOutfitHolder()
		{
			if (m_form)
			{
				FailsafeClearOutfitForms();
				RE::GarbageCollector::GetSingleton()->QueueForm(m_form);
				m_form = nullptr;
			}
		}

		void BGSOutfitHolder::on_load_discard()
		{
			if (m_form)
			{
				FailsafeClearOutfitForms();

				auto sm = PersistentFormManager::GetSingleton();
				sm->discardedForms.emplace(m_form);

				m_form = nullptr;
			}
		}

		void BGSOutfitHolder::FailsafeClearOutfitForms()
		{
			const stl::lock_guard lock(*Game::g_formTableLock);

			if (const auto map = *Game::g_formTable)
			{
				for (const auto& e : *map)
				{
					if (!e.second)
					{
						continue;
					}

					TESNPC* npc = nullptr;

					switch (e.second->formType)
					{
					case Actor::kTypeID:
						npc = static_cast<Actor*>(e.second)->GetActorBase();
						break;
					case TESNPC::kTypeID:
						npc = static_cast<TESNPC*>(e.second);
						break;
					}

					if (!npc)
					{
						continue;
					}

					if (npc->defaultOutfit == m_form)
					{
						npc->defaultOutfit = nullptr;
					}

					if (npc->sleepOutfit == m_form)
					{
						npc->sleepOutfit = nullptr;
					}
				}
			}
		}
	}
}