#pragma once

#if defined(IED_ENABLE_OUTFIT) && defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
#	include "BGSOutfitHolder.h"

#	include "ConfigOutfitForm.h"

#	include <ext/SKSESerializationEventHandler.h>

namespace RE
{
	class PersistentFormManager;
}

namespace IED
{
	namespace OM
	{
		class PersistentOutfitFormManager :
			//public ::Events::EventSink<SKSESerializationEvent>,
			//public ::Events::EventSink<SKSEMessagingEvent>,
			ILog
		{
			class IOBufferDummy;

		public:
			using container_type         = stl::unordered_map<stl::fixed_string, BGSOutfitHolderPtr>;
			//using discard_container_type = stl::unordered_map<TESForm*, BGSOutfitHolderPtr>;

			[[nodiscard]] static constexpr auto& GetSingleton() noexcept
			{
				return m_Instance;
			}

			void               Install();
			BGSOutfitHolderPtr CreateForm(const stl::fixed_string& a_name);
			BGSOutfitHolderPtr GetHolder(const stl::fixed_string& a_id) const;
			bool               RemoveForm(const stl::fixed_string& a_id, Game::FormID& a_removedFormID);
			void               ClearForms();
			bool               RemoveOutfitItem(const stl::fixed_string& a_id, std::uint32_t a_index);
			bool               AddOutfitItem(const stl::fixed_string& a_id, Game::FormID a_item);
			bool               AddOutfitItemList(const stl::fixed_string& a_id, const stl::vector<Data::configFormZeroMissing_t>& a_list);
			bool               ClearOutfitList(const stl::fixed_string& a_id);
			bool               UpdateOutfitItem(const stl::fixed_string& a_id, std::uint32_t a_index, Game::FormID a_newItem);
			bool               ApplyFormsFromList(const Data::OM::configOutfitFormList_t& a_list);

			[[nodiscard]] auto GetScopedLock() const noexcept
			{
				return stl::lock_guard(m_lock);
			}

			[[nodiscard]] constexpr auto& GetForms() const noexcept
			{
				return m_forms;
			}

			void ToggleRead() noexcept;
			void ToggleWrite() noexcept;

			[[nodiscard]] bool GetReadEnabled() const noexcept;
			[[nodiscard]] bool GetWriteEnabled() const noexcept;

			Data::OM::configOutfitFormList_t MakeFormList() const;
			stl::vector<stl::fixed_string>   GetIDs() const;

		private:
			FN_NAMEPROC("PersistentOutfitFormManager");

			void Install_WriteHook();
			void Install_ReadHook();
			//void Install_CleanupFormsHook();

			bool EraseEntry(const stl::fixed_string& a_id, BGSOutfitHolderPtr& a_erased);
			bool EraseFirstEntry(BGSOutfitHolderPtr& a_erased);

			template <class Tf>
			bool ModifyEntry(
				const stl::fixed_string& a_entryid,
				Tf                       a_func);

			static void RemoveNPCOutfits(BGSOutfit* a_outfit);
			static void RefreshNPCOutfits(BGSOutfit* a_match);

			//virtual void Receive(const SKSESerializationEvent& a_evn) override;
			//virtual void Receive(const SKSEMessagingEvent& a_evn) override;

			//static void RevertHandler(SKSESerializationInterface* a_intfc);

			static void PersistentFormManager_WriteSaveBuffer_Hook(PersistentFormManager* a_formManager, IOBufferDummy* a_buffer);
			static void PersistentFormManager_ReadSaveBuffer_Hook(PersistentFormManager* a_formManager, IOBufferDummy* a_buffer);
			//static void PersistentFormManager_CleanupFormsHook_Hook(PersistentFormManager* a_formManager);

			//static void renumberform_hook(void* a1, TESForm *a2);

			inline static const auto m_PersistentFormManager_WriteSaveBuffer = IAL::Address<std::uintptr_t>(35260, 0);
			inline static const auto m_PersistentFormManager_ReadSaveBuffer  = IAL::Address<std::uintptr_t>(35261, 0);
			//inline static const auto m_PersistentFormManager_CleanupForms    = IAL::Address<std::uintptr_t>(35262, 0);

			//inline static const auto m_renumberform = IAL::Address<std::uintptr_t>(34665, 0);

			decltype(&PersistentFormManager_WriteSaveBuffer_Hook)  m_PersistentFormManager_WriteSaveBuffer_o{ nullptr };
			decltype(&PersistentFormManager_ReadSaveBuffer_Hook)   m_PersistentFormManager_ReadSaveBuffer_o{ nullptr };
			//decltype(&PersistentFormManager_CleanupFormsHook_Hook) m_PersistentFormManager_CleanupForms_o{ nullptr };
			//decltype(&renumberform_hook)                          m_renumberform_o{ nullptr };

			using BGSSaveGameBuffer_unk_16_t                  = void (*)(IOBufferDummy* a_buffer, std::uint32_t a_size);
			inline static const auto BGSSaveGameBuffer_unk_16 = IAL::Address<BGSSaveGameBuffer_unk_16_t>(35161, 0);

			using BGSSaveGameBuffer_WriteFormID_t                  = void (*)(IOBufferDummy* a_buffer, TESForm* a_form);
			inline static const auto BGSSaveGameBuffer_WriteFormID = IAL::Address<BGSSaveGameBuffer_WriteFormID_t>(35158, 0);

			using BGSLoadFormBuffer_unk_5_t                  = std::uint32_t (*)(IOBufferDummy* a_buffer);
			inline static const auto BGSLoadFormBuffer_unk_5 = IAL::Address<BGSLoadFormBuffer_unk_5_t>(35111, 0);

			using BGSSaveGameBuffer_ReadFormIdFromBuffer_t        = bool (*)(IOBufferDummy* a_buffer, Game::FormID& a_out);
			inline static const auto BGSLoadFormBuffer_ReadFormId = IAL::Address<BGSSaveGameBuffer_ReadFormIdFromBuffer_t>(35107, 0);

			inline static const auto unk142F266F8 = IAL::Address<void**>(516851, 0);

			using sub_14057BAD0_t                  = void (*)(void* a_unk, Game::FormID a_formid);
			inline static const auto sub_14057BAD0 = IAL::Address<sub_14057BAD0_t>(34661, 0);

			using RenumberForm_t                  = void (*)(void* a_unk, TESForm* a_form);
			inline static const auto RenumberForm = IAL::Address<RenumberForm_t>(34665, 0);

			using BGSLoadGameBuffer_ReadBSFixedString_t                  = void (*)(IOBufferDummy* a_buffer, BSFixedString& a_out);
			inline static const auto BGSLoadGameBuffer_ReadBSFixedString = IAL::Address<BGSLoadGameBuffer_ReadBSFixedString_t>(35109, 0);

			using BGSSaveGameBuffer_WriteBSFixedString_t                  = void (*)(IOBufferDummy* a_buffer, const char* a_fixedStringData);
			inline static const auto BGSSaveGameBuffer_WriteBSFixedString = IAL::Address<BGSSaveGameBuffer_WriteBSFixedString_t>(35159, 0);

			void WriteForms(IOBufferDummy* a_buffer) const;
			void ReadForms(PersistentFormManager* a_pfm, IOBufferDummy* a_buffer);

			container_type               m_forms;
			//discard_container_type       m_discardedForms;
			std::atomic<std::uint32_t>   m_flags = 0x3;
			mutable stl::recursive_mutex m_lock;

			static PersistentOutfitFormManager m_Instance;
		};

		template <class Tf>
		inline bool PersistentOutfitFormManager::ModifyEntry(
			const stl::fixed_string& a_entryid,
			Tf                       a_func)
		{
			const auto ptr = GetHolder(a_entryid);
			if (ptr)
			{
				return a_func(ptr);
			}
			else
			{
				return false;
			}
		}

	}
}

#endif