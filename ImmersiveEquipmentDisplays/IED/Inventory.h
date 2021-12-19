#pragma once

#include "CollectorData.h"
#include "Controller/ControllerCommon.h"
#include "Data.h"

namespace IED
{
	class Controller;

	EntryDataList* GetEntryDataList(Actor* a_actor);

	struct SlotItemCandidates
	{
		/*SlotItemCandidates()
		{
			m_items.reserve(100);
		}*/

		struct item_t
		{
			TESForm* form;
			std::uint16_t damage;
			std::int64_t extra;
			Data::collectorData_t::itemData_t* item;
		};

		using storage_type = std::vector<item_t>;

		storage_type m_items;
	};

	struct ItemCandidateCollector
	{
	private:
		struct slots_t
		{
			Data::ObjectSlot slot;
			Data::ObjectSlot slotLeft;
		};

	public:
		ItemCandidateCollector(
			Actor* a_actor,
			TESRace *a_race);

		void Run(
			TESContainer& a_container,
			EntryDataList* a_dataList);

		SKMP_FORCEINLINE bool Accept(TESContainer::ConfigEntry* entry);
		SKMP_FORCEINLINE bool Accept(InventoryEntryData* a_entryData);

		void GenerateSlotCandidates();

		SlotItemCandidates m_slotResults[stl::underlying(Data::ObjectType::kMax)];

		/*inline void DecrementSharedCount(Game::FormID a_form, std::int64_t a_mod)
		{
			auto it = m_data.find(a_form);
			if (it != m_data.end())
			{
				it->second.sharedCount -= a_mod;
			}
		}*/

		Data::collectorData_t m_data;

	private:
		SKMP_FORCEINLINE bool CheckForm(TESForm* a_form);
		//SKMP_FORCEINLINE bool AddEquippedForms(TESForm* a_form, InventoryEntryData* a_entryData);

		bool m_isPlayer;
		ActorProcessManager* m_pm;
	};

	struct EquippedFormCollector
	{
		EquippedFormCollector(Actor* a_actor, TESRace* a_race);

		bool Accept(InventoryEntryData* a_entryData);

		Data::collectorData_t m_data;

	private:
		ActorProcessManager* m_pm;
	};

	struct EquippedArmorCollector
	{
		EquippedArmorCollector(Actor* a_actor, TESRace* a_race);

		void Run();

		SKMP_FORCEINLINE bool Accept(InventoryEntryData* a_entryData);

		Data::collectorData_t m_data;
	};

	/*struct ItemCounter
	{
		void Count(TESContainer& a_container, EntryDataList* a_dataList);

		bool Accept(TESContainer::ConfigEntry* entry);
		bool Accept(InventoryEntryData* a_entryData);

		struct entry_t
		{
			std::int64_t count;
			TESForm* form;
		};

		std::unordered_map<Game::FormID, entry_t> m_data;

	private:
		void ProcessForm(TESForm* a_form, std::int64_t a_count);
	};*/

	/*struct CustomItemCounter
    {
        CustomItemCounter(
            Actor *a_actor,
            const Data::ItemData::storage_type_custom&a_data);

        bool Accept(TESContainer::ConfigEntry* entry);
        bool Accept(InventoryEntryData* a_entryData);

        struct entry_t
        {
            std::list<const Data::ItemData::storage_type_custom::value_type*>
   entries; std::int64_t count{ 0 }; bool equipped{ false };
        };

        std::unordered_map<Game::FormID, entry_t> m_data;

    private:

        void ProcessForm(TESForm* a_form, std::int64_t a_count);
        static bool IsEquipped(InventoryEntryData* a_entryData);

    };*/

}  // namespace IED