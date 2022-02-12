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

	public:
		ItemCandidateCollector(
			Actor* a_actor);

		void Run(
			TESContainer& a_container,
			EntryDataList* a_dataList);

		SKMP_FORCEINLINE bool Accept(TESContainer::Entry* entry);
		SKMP_FORCEINLINE bool Accept(InventoryEntryData* a_entryData);

		void GenerateSlotCandidates(bool a_checkFav);

		SlotItemCandidates m_slotResults[stl::underlying(Data::ObjectType::kMax)];

		Data::collectorData_t m_data;

	private:
		SKMP_FORCEINLINE bool CheckForm(TESForm* a_form);

		bool m_isPlayer;
	};

}  // namespace IED