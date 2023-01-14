#pragma once

#include "CollectorData.h"
#include "Data.h"

#include "Controller/ControllerCommon.h"

namespace IED
{
	class Controller;

	EntryDataList* GetEntryDataList(const Actor* const a_actor) noexcept;

	struct SlotCandidate
	{
		const Data::CollectorData::ItemData* item;
		std::uint32_t                        extra;
		std::uint32_t                        rating;
	};

	using SlotItemCandidates = std::vector<SlotCandidate>;

	using SlotResults = std::array<SlotItemCandidates, stl::underlying(Data::ObjectType::kMax)>;

	struct InventoryInfoCollector
	{
	public:
		InventoryInfoCollector(
			SlotResults&                            a_slotResults,
			Data::CollectorData::container_type&    a_idt,
			Data::CollectorData::eq_container_type& a_eqt,
			const Actor* const                      a_actor) noexcept;

	private:
		void Run(
			const TESContainer&  a_container,
			EntryDataList* const a_dataList) noexcept;

		void ProcessList(const TESContainer& a_container) noexcept;
		void ProcessList(EntryDataList* const a_dataList) noexcept;
		void PostProcess() noexcept;

		SKMP_FORCEINLINE void Process(const TESContainer::Entry* entry) noexcept;
		SKMP_FORCEINLINE void Process(const InventoryEntryData* a_entryData) noexcept;

	public:
		void GenerateSlotCandidates(const bool a_isPlayer, const bool a_checkFav) noexcept;

		[[nodiscard]] constexpr auto& GetCandidates(Data::ObjectType a_type) noexcept
		{
			assert(a_type < Data::ObjectType::kMax);
			return slotResults[stl::underlying(a_type)];
		}

		Data::CollectorData data;
		SlotResults&        slotResults;
	};

	using UseCountContainer = stl::flat_map<
		Game::FormID,
		std::uint32_t>;

}