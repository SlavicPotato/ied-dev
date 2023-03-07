/*
	A very hacky implementation that:
		1.) allows temporary outfit forms to be created and modified in-game
		2.) makes the forms persist in the save

	Writes data directly to the save, absolutely not to be included in a public release!

*/

#include "pch.h"

#if defined(IED_ENABLE_OUTFIT) && defined(IED_ENABLE_OUTFIT_FORM_MANAGER)

#	include "PersistentOutfitFormManager.h"

#	include <ext/JITASM.h>
#	include <ext/SKSEMessagingHandler.h>

namespace IED
{
	namespace OM
	{
		PersistentOutfitFormManager PersistentOutfitFormManager::m_Instance;

		namespace detail
		{
			static constexpr bool is_valid_outfit_item(const TESForm* a_item)
			{
				if (!a_item || a_item->formID.IsTemporary())
				{
					return false;
				}

				switch (a_item->formType)
				{
				case TESObjectARMO::kTypeID:
				case TESLevItem::kTypeID:
					return true;
				default:
					return false;
				}
			}
		}

		void PersistentOutfitFormManager::Install()
		{
			Install_WriteHook();
			Install_ReadHook();
			//Install_CleanupFormsHook();

			//auto& seh = SKSESerializationEventHandler::GetSingleton();

			//seh.GetDispatcher<SKSESerializationEvent>().AddSink(this);

			//SKSEMessagingHandler::GetSingleton().AddSink(this);

			/*struct Assembly : JITASM::JITASM
		{
			Assembly(std::uintptr_t a_targetAddr) :
				JITASM(ISKSE::GetLocalTrampoline())
			{
				Xbyak::Label retnLabel;

				constexpr std::size_t size = 0x5;

				db(reinterpret_cast<Xbyak::uint8*>(a_targetAddr), size);
				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(a_targetAddr + size);
			}
		};

		LogPatchBegin();
		{
			Assembly code(m_renumberform.get() + 0x9);
			m_renumberform_o = code.get<decltype(m_renumberform_o)>();

			ISKSE::GetBranchTrampoline().Write5Branch(
				m_renumberform.get() + 0x9,
				std::uintptr_t(renumberform_hook));
		}
		LogPatchEnd();*/
		}

		void PersistentOutfitFormManager::Install_WriteHook()
		{
			struct Assembly : JITASM::JITASM
			{
				Assembly(std::uintptr_t a_targetAddr) :
					JITASM(ISKSE::GetLocalTrampoline())
				{
					Xbyak::Label retnLabel;

					constexpr std::size_t size = 0x5;

					db(reinterpret_cast<Xbyak::uint8*>(a_targetAddr), size);
					jmp(ptr[rip + retnLabel]);

					L(retnLabel);
					dq(a_targetAddr + size);
				}
			};

			LogPatchBegin();
			{
				Assembly code(m_PersistentFormManager_WriteSaveBuffer.get());
				m_PersistentFormManager_WriteSaveBuffer_o = code.get<decltype(m_PersistentFormManager_WriteSaveBuffer_o)>();

				ISKSE::GetBranchTrampoline().Write5Branch(
					m_PersistentFormManager_WriteSaveBuffer.get(),
					std::uintptr_t(PersistentFormManager_WriteSaveBuffer_Hook));
			}
			LogPatchEnd();
		}

		void PersistentOutfitFormManager::Install_ReadHook()
		{
			struct Assembly : JITASM::JITASM
			{
				Assembly(std::uintptr_t a_targetAddr) :
					JITASM(ISKSE::GetLocalTrampoline())
				{
					Xbyak::Label retnLabel;

					constexpr std::size_t size = 0x7;

					db(reinterpret_cast<Xbyak::uint8*>(a_targetAddr), size);
					jmp(ptr[rip + retnLabel]);

					L(retnLabel);
					dq(a_targetAddr + size);
				}
			};

			LogPatchBegin();
			{
				Assembly code(m_PersistentFormManager_ReadSaveBuffer.get());
				m_PersistentFormManager_ReadSaveBuffer_o = code.get<decltype(m_PersistentFormManager_ReadSaveBuffer_o)>();

				ISKSE::GetBranchTrampoline().Write6Branch(
					m_PersistentFormManager_ReadSaveBuffer.get(),
					std::uintptr_t(PersistentFormManager_ReadSaveBuffer_Hook));
			}
			LogPatchEnd();
		}

		/*void PersistentOutfitFormManager::Install_CleanupFormsHook()
		{
			struct Assembly : JITASM::JITASM
			{
				Assembly(std::uintptr_t a_targetAddr) :
					JITASM(ISKSE::GetLocalTrampoline())
				{
					Xbyak::Label retnLabel;

					constexpr std::size_t size = 0x5;

					db(reinterpret_cast<Xbyak::uint8*>(a_targetAddr), size);
					jmp(ptr[rip + retnLabel]);

					L(retnLabel);
					dq(a_targetAddr + size);
				}
			};

			LogPatchBegin();
			{
				Assembly code(m_PersistentFormManager_CleanupForms.get());
				m_PersistentFormManager_CleanupForms_o = code.get<decltype(m_PersistentFormManager_CleanupForms_o)>();

				ISKSE::GetBranchTrampoline().Write5Branch(
					m_PersistentFormManager_CleanupForms.get(),
					std::uintptr_t(PersistentFormManager_CleanupFormsHook_Hook));
			}
			LogPatchEnd();
		}*/

		BGSOutfitHolderPtr PersistentOutfitFormManager::CreateForm(
			const stl::fixed_string& a_name)
		{
			auto holder = stl::make_smart<BGSOutfitHolder>(a_name.c_str());

			ASSERT(holder->get());

			const stl::lock_guard lock(m_lock);

			const auto r = m_forms.try_emplace(holder->get_name().c_str(), std::move(holder));

			if (!r.second)
			{
				return nullptr;
			}

			return r.first->second;
		}

		BGSOutfitHolderPtr PersistentOutfitFormManager::GetHolder(const stl::fixed_string& a_id) const
		{
			const stl::lock_guard lock(m_lock);

			auto it = m_forms.find(a_id);
			return it != m_forms.end() ? it->second : BGSOutfitHolderPtr();
		}

		bool PersistentOutfitFormManager::RemoveForm(
			const stl::fixed_string& a_id,
			Game::FormID&            a_removedFormID)
		{
			BGSOutfitHolderPtr out;

			if (EraseEntry(a_id, out))
			{
				a_removedFormID = out->get()->formID;

				RemoveNPCOutfits(out->get());

				return true;
			}
			else
			{
				return false;
			}
		}

		void PersistentOutfitFormManager::ClearForms()
		{
			BGSOutfitHolderPtr out;

			while (EraseFirstEntry(out))
			{
				RemoveNPCOutfits(out->get());
			}
		}

		bool PersistentOutfitFormManager::RemoveOutfitItem(
			const stl::fixed_string& a_id,
			std::uint32_t            a_index)
		{
			return ModifyEntry(
				a_id,
				[&](const BGSOutfitHolderPtr& a_holder) {
					{
						auto& e = a_holder->get()->outfitItems;

						if (a_index >= e.size())
						{
							return false;
						}

						e.erase(e.begin() + a_index);
					}

					RefreshNPCOutfits(a_holder->get());

					return true;
				});
		}

		bool PersistentOutfitFormManager::AddOutfitItem(
			const stl::fixed_string& a_id,
			Game::FormID             a_item)
		{
			const auto form = a_item.Lookup();
			if (!detail::is_valid_outfit_item(form))
			{
				return false;
			}

			return ModifyEntry(
				a_id,
				[&](const BGSOutfitHolderPtr& a_holder) {
					{
						auto& e = a_holder->get()->outfitItems;

						auto it = std::find(e.begin(), e.end(), form);
						if (it != e.end())
						{
							return false;
						}

						e.emplace_back(form);
					}

					RefreshNPCOutfits(a_holder->get());

					return true;
				});
		}

		bool PersistentOutfitFormManager::AddOutfitItemList(
			const stl::fixed_string&                          a_id,
			const stl::vector<Data::configFormZeroMissing_t>& a_list)
		{
			stl::vector<TESForm*> forms;
			forms.reserve(a_list.size());

			for (auto& e : a_list)
			{
				const auto form = e.Lookup();
				if (!detail::is_valid_outfit_item(form))
				{
					return false;
				}

				forms.emplace_back(form);
			}

			return ModifyEntry(
				a_id,
				[&](const BGSOutfitHolderPtr& a_holder) {
					bool result = false;

					{
						auto& list = a_holder->get()->outfitItems;

						for (auto& e : forms)
						{
							auto it = std::find(list.begin(), list.end(), e);
							if (it == list.end())
							{
								list.emplace_back(e);
								result = true;
							}
						}
					}

					if (result)
					{
						RefreshNPCOutfits(a_holder->get());
					}

					return true;
				});
		}

		bool PersistentOutfitFormManager::ClearOutfitList(
			const stl::fixed_string& a_id)
		{
			return ModifyEntry(
				a_id,
				[&](const BGSOutfitHolderPtr& a_holder) {
					{
						auto& list = a_holder->get()->outfitItems;

						if (list.empty())
						{
							return false;
						}

						list.clear();
					}

					RefreshNPCOutfits(a_holder->get());

					return true;
				});
		}

		bool PersistentOutfitFormManager::UpdateOutfitItem(
			const stl::fixed_string& a_id,
			std::uint32_t            a_index,
			Game::FormID             a_newItem)
		{
			const auto form = a_newItem.Lookup();
			if (!detail::is_valid_outfit_item(form))
			{
				return false;
			}

			return ModifyEntry(
				a_id,
				[&](const BGSOutfitHolderPtr& a_holder) {
					{
						auto& e = a_holder->get()->outfitItems;

						auto it = std::find(e.begin(), e.end(), form);
						if (it != e.end())
						{
							return false;
						}

						if (a_index >= e.size())
						{
							return false;
						}

						e[a_index] = form;
					}

					RefreshNPCOutfits(a_holder->get());

					return true;
				});
		}

		bool PersistentOutfitFormManager::ApplyFormsFromList(
			const Data::OM::configOutfitFormList_t& a_list)
		{
			ClearForms();

			bool result = true;

			for (auto& e : a_list)
			{
				auto form = CreateForm(e.first.c_str());

				if (!form)
				{
					result = false;
					continue;
				}

				if (!AddOutfitItemList(e.first, e.second.items))
				{
					result = false;
				}
			}

			return result;
		}

		void PersistentOutfitFormManager::ToggleRead() noexcept
		{
			m_flags.fetch_xor(0x1);
		}

		void PersistentOutfitFormManager::ToggleWrite() noexcept
		{
			m_flags.fetch_xor(0x2);
		}

		bool PersistentOutfitFormManager::GetReadEnabled() const noexcept
		{
			return static_cast<bool>(m_flags.load() & 0x1);
		}

		bool PersistentOutfitFormManager::GetWriteEnabled() const noexcept
		{
			return static_cast<bool>(m_flags.load() & 0x2);
		}

		Data::OM::configOutfitFormList_t PersistentOutfitFormManager::MakeFormList() const
		{
			Data::OM::configOutfitFormList_t result;

			const auto lock1 = GetScopedLock();

			const auto& forms = GetForms();

			result.reserve(forms.size());

			for (auto& e : forms)
			{
				auto r = result.try_emplace(e.first);

				const auto& list = e.second->get()->outfitItems;

				r.first->second.items.reserve(list.size());

				for (auto& f : list)
				{
					r.first->second.items.emplace_back(f ? f->formID : Game::FormID{});
				}
			}

			return result;
		}

		stl::vector<stl::fixed_string> PersistentOutfitFormManager::GetIDs() const
		{
			stl::vector<stl::fixed_string> result;

			const auto lock = GetScopedLock();

			result.reserve(m_forms.size());

			std::transform(
				m_forms.begin(),
				m_forms.end(),
				std::back_inserter(result),
				[](auto& a_v) {
					return a_v.first;
				});

			return result;
		}

		bool PersistentOutfitFormManager::EraseEntry(
			const stl::fixed_string& a_id,
			BGSOutfitHolderPtr&      a_erased)
		{
			const stl::lock_guard lock(m_lock);

			auto it = m_forms.find(a_id);
			if (it == m_forms.end())
			{
				return false;
			}

			a_erased = std::move(it->second);

			m_forms.erase(it);

			return true;
		}

		bool PersistentOutfitFormManager::EraseFirstEntry(
			BGSOutfitHolderPtr& a_erased)
		{
			const stl::lock_guard lock(m_lock);

			auto it = m_forms.begin();
			if (it == m_forms.end())
			{
				return false;
			}

			a_erased = std::move(it->second);

			m_forms.erase(it);

			return true;
		}

		void PersistentOutfitFormManager::RemoveNPCOutfits(BGSOutfit* a_outfit)
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

					const auto actor = e.second->As<Actor>();
					if (!actor)
					{
						continue;
					}

					const auto npc = actor->GetActorBase();
					if (!npc)
					{
						continue;
					}

					if (npc->defaultOutfit == a_outfit)
					{
						if (!actor->formID)
						{
							npc->defaultOutfit = nullptr;
							npc->RemoveChange(TESNPC::ChangeFlags::kDefaultOutfit);
						}
						else
						{
							Game::ClearOutfit(actor, false);
						}
					}

					if (npc->sleepOutfit == a_outfit)
					{
						if (!actor->formID)
						{
							npc->sleepOutfit = nullptr;
							npc->RemoveChange(TESNPC::ChangeFlags::kSleepOutfit);
						}
						else
						{
							Game::ClearOutfit(actor, true);
						}
					}
				}

				for (const auto& e : *map)
				{
					if (!e.second)
					{
						continue;
					}

					const auto npc = e.second->As<TESNPC>();
					if (!npc)
					{
						continue;
					}

					if (npc->defaultOutfit == a_outfit)
					{
						npc->defaultOutfit = nullptr;
						npc->RemoveChange(TESNPC::ChangeFlags::kDefaultOutfit);
					}

					if (npc->sleepOutfit == a_outfit)
					{
						npc->sleepOutfit = nullptr;
						npc->RemoveChange(TESNPC::ChangeFlags::kSleepOutfit);
					}
				}
			}
		}

		void PersistentOutfitFormManager::RefreshNPCOutfits(BGSOutfit* a_match)
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

					const auto actor = e.second->As<Actor>();
					if (!actor || !actor->formID)
					{
						continue;
					}

					const auto npc = actor->GetActorBase();
					if (!npc)
					{
						continue;
					}

					if (npc->defaultOutfit == a_match ||
					    npc->sleepOutfit == a_match)
					{
						Game::RemoveOutfitItems(actor, nullptr);
						Game::unk1D8D10(actor, false);

						if (!actor->IsDisabled())
						{
							Game::EquipOutfit(actor, a_match, false);
						}
					}
				}
			}
		}

		/*void PersistentOutfitFormManager::Receive(const SKSESerializationEvent& a_evn)
		{
			switch (a_evn.type)
			{
			case SKSESerializationEventType::kRevert:
				RevertHandler(a_evn.intfc);
				break;
			}
		}*/

		/*void PersistentOutfitFormManager::Receive(const SKSEMessagingEvent& a_evn)
		{
			switch (a_evn.message->type)
			{
			case SKSEMessagingInterface::kMessage_PostLoadGame:

				auto& sm = OM::PersistentOutfitFormManager::GetSingleton();

				const auto lock = sm.GetScopedLock();

				for (auto& e : sm.GetForms())
				{
					ASSERT(e.first == (*e.second)->formID);
				}

				break;
			}
		}*/

		/*void PersistentOutfitFormManager::RevertHandler([[maybe_unused]] SKSESerializationInterface* a_intfc)
		{
			const stl::lock_guard lock(m_Instance.m_lock);

			m_Instance.m_forms.clear();
		}*/

		void PersistentOutfitFormManager::PersistentFormManager_WriteSaveBuffer_Hook(
			PersistentFormManager* a_formManager,
			IOBufferDummy*         a_buffer)
		{
			m_Instance.m_PersistentFormManager_WriteSaveBuffer_o(a_formManager, a_buffer);
			m_Instance.WriteForms(a_buffer);
		}

		void PersistentOutfitFormManager::PersistentFormManager_ReadSaveBuffer_Hook(
			PersistentFormManager* a_formManager,
			IOBufferDummy*         a_buffer)
		{
			m_Instance.m_PersistentFormManager_ReadSaveBuffer_o(a_formManager, a_buffer);

			m_Instance.ReadForms(a_formManager, a_buffer);

			/*if (auto p = PersistentFormManager::GetSingleton())
			{
				_DMESSAGE("%u", p->weaponEnchants.size());
				_DMESSAGE("%u", p->armorEnchants.size());
				_DMESSAGE("%u", p->poisons.size());
				_DMESSAGE("%u", p->potions.size());
				_DMESSAGE("%u", p->discardedForms.size());

				for (auto& e : p->discardedForms)
				{
					_DMESSAGE("%.8X | %hhu | %s", e->formID, e->formType, e->GetName());
				}
			}*/
		}

		/*void PersistentOutfitFormManager::PersistentFormManager_CleanupFormsHook_Hook(PersistentFormManager* a_formManager)
		{
			m_Instance.m_PersistentFormManager_CleanupForms_o(a_formManager);
			m_Instance.CleanupDiscardedForms();
		}*/

		/*void PersistentOutfitFormManager::renumberform_hook(void* a1, TESForm* a2)
		{
			auto tmp = a2->formID;

			m_Instance.m_renumberform_o(a1, a2);

			if (a2->formType == BGSOutfit::kTypeID)
			{
				_DMESSAGE("renumber outcome: %.8X [%.2X] -> %.8X", tmp, a2->formType, a2->formID);
			}
		}*/

		void PersistentOutfitFormManager::WriteForms(IOBufferDummy* a_buffer) const
		{
			if (!GetWriteEnabled())
			{
				return;
			}

			Debug("write");

			const stl::lock_guard lock(m_lock);

			auto size = static_cast<std::uint32_t>(std::min(m_forms.size(), 1073741823ui64));

			BGSSaveGameBuffer_unk_16(a_buffer, size);

			for (auto& e : m_forms)
			{
				BGSSaveGameBuffer_WriteFormID(a_buffer, e.second->get());
				BGSSaveGameBuffer_WriteBSFixedString(a_buffer, e.second->get_name().__ptr());

				const auto form = e.second->get();

				const auto numitems = form->outfitItems.size();

				Debug("storing %.8X [%u]", form->formID, numitems);

				BGSSaveGameBuffer_unk_16(a_buffer, numitems);

				for (auto& f : form->outfitItems)
				{
					//Debug("%.8X: %.8X", e.second->get()->formID, f ? f->formID.get() : 0);

					BGSSaveGameBuffer_WriteFormID(a_buffer, f);
				}

				if (!--size)
				{
					break;
				}
			}
		}

		void PersistentOutfitFormManager::ReadForms(PersistentFormManager* a_pfm, IOBufferDummy* a_buffer)
		{
			if (!GetReadEnabled())
			{
				return;
			}

			Debug("read");

			const stl::lock_guard lock(m_lock);

			for (auto& e : m_forms)
			{
				e.second->on_load_discard();
			}

			m_forms.clear();

			const auto size = BGSLoadFormBuffer_unk_5(a_buffer);

			//Debug("sz: %u", size);

			for (std::uint32_t i = 0; i < size; i++)
			{
				Game::FormID formid;

				BGSLoadFormBuffer_ReadFormId(a_buffer, formid);

				BSFixedString name;

				BGSLoadGameBuffer_ReadBSFixedString(a_buffer, name);

				const auto numItems = BGSLoadFormBuffer_unk_5(a_buffer);

				Debug("read %.8X [%u] [%s]", formid.get(), numItems, name.c_str());

				RE::BSTArray<TESForm*> outfitItems;

				outfitItems.reserve(numItems);

				for (std::uint32_t j = 0; j < numItems; j++)
				{
					Game::FormID itemID;

					BGSLoadFormBuffer_ReadFormId(a_buffer, itemID);

					if (itemID.IsTemporary())
					{
						continue;
					}

					const auto itemForm = itemID.Lookup();

					if (detail::is_valid_outfit_item(itemForm))
					{
						outfitItems.emplace_back(itemForm);
					}
					else
					{
						Warning("%.8X: invalid item form %.8X", formid.get(), itemID.get());
					}
				}

				if (!formid || !formid.IsTemporary())
				{
					continue;
				}

				BGSOutfitHolderPtr holder;

				const auto existingForm = formid.Lookup();

				if (existingForm && existingForm->formType == BGSOutfit::kTypeID)
				{
					/*
					auto it = m_discardedForms.find(existingForm);
					if (it != m_discardedForms.end())
					{
						holder = it->second;

						holder->get()->outfitItems = std::move(outfitItems);
						holder->set_name(std::move(name));
					}
					else
					{
						holder = stl::make_smart<BGSOutfitHolder>(
							static_cast<BGSOutfit*>(existingForm),
							std::move(outfitItems),
							std::move(name));
					}					
					*/

					holder = stl::make_smart<BGSOutfitHolder>(
						static_cast<BGSOutfit*>(existingForm),
						std::move(outfitItems),
						std::move(name));

					a_pfm->discardedForms.erase(existingForm);
				}
				else
				{
					if (existingForm)
					{
						RenumberForm(*unk142F266F8, existingForm);

						Debug(
							"renumber outcome: %.8X [%.2X] -> %.8X | %p",
							formid,
							existingForm->formType,
							existingForm->formID.get(),
							existingForm);
					}

					sub_14057BAD0(*unk142F266F8, formid);

					holder = stl::make_smart<BGSOutfitHolder>(
						formid,
						std::move(outfitItems),
						std::move(name));
				}

				ASSERT(holder->get());

				m_forms.emplace(holder->get_name().c_str(), std::move(holder));
			}
		}

		/*void PersistentOutfitFormManager::CleanupDiscardedForms()
		{
			const stl::lock_guard lock(m_lock);

			m_discardedForms.clear();
		}*/
	}
}

#endif