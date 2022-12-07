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
			const Data::collectorData_t::itemData_t* item;
			std::uint32_t                            extra;
			std::uint32_t                            rating;
		};

		using storage_type =
#if defined(IED_USE_MIMALLOC_COLLECTOR)
			std::vector<
				item_t,
				stl::mi_allocator<item_t>>
#else
			stl::vector<item_t>
#endif
			;

		storage_type items;
		//std::size_t  reserve{ 0 };
	};

	using SlotResults = std::array<SlotItemCandidates, stl::underlying(Data::ObjectType::kMax)>;

	struct ItemCandidateCollector
	{
	public:
		ItemCandidateCollector(
			SlotResults& a_slotResults,
			Actor*       a_actor);

		void Run(
			TESContainer&                          a_container,
			EntryDataList* a_dataList);

		SKMP_FORCEINLINE void Process(TESContainer::Entry* entry);
		SKMP_FORCEINLINE void Process(InventoryEntryData* a_entryData);

		void GenerateSlotCandidates(bool a_checkFav);

		[[nodiscard]] inline constexpr auto& GetCandidates(Data::ObjectType a_type) noexcept
		{
			assert(a_type < Data::ObjectType::kMax);
			return slotResults[stl::underlying(a_type)].items;
		}

		Data::collectorData_t data;
		SlotResults&          slotResults;

	private:
		SKMP_FORCEINLINE bool CheckForm(TESForm* a_form);

		bool isPlayer;
	};

	using UseCountContainer = stl::map_sa<
		Game::FormID,
		std::uint32_t>;

}