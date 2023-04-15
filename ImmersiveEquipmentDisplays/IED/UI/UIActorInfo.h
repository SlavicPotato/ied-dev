#pragma once

#include "Controls/UICollapsibles.h"

#include "Widgets/Form/UIFormInfoTooltipWidget.h"
#include "Widgets/Lists/UIActorListWidget.h"

#include "UIContext.h"
#include "Window/UIWindow.h"

#include "UILocalizationInterface.h"

#include "IED/ActorActiveEffectInfo.h"
#include "IED/ActorBipedInfo.h"
#include "IED/ActorFactionInfo.h"
#include "IED/ActorInventoryInfo.h"
#include "IED/ActorPerkInfo.h"
#include "IED/ActorValueInfo.h"
#include "IED/ActorWeaponNodeInfo.h"
#include "IED/ConditionalVariableStorage.h"

namespace IED
{
	class Controller;
	class ActorObjectHolder;

	namespace UI
	{
		struct ActorInfoAggregate :
			stl::intrusive_ref_counted
		{
			SKMP_REDEFINE_NEW_PREF();

			stl::mutex              lock;
			long long               lastUpdate{ 0 };
			bool                    initialized{ false };
			bool                    succeeded{ false };
			Game::FormID            actor;
			actorInfoEntry_t        entry;
			actorInventoryInfo_t    inventory;
			actorFactionInfo_t      factions;
			actorActiveEffectInfo_t effects;
			actorPerkInfo_t         perks;
			actorValueInfo_t        actorValues;
			actorWeaponNodeInfo_t   weaponNodes;
			actorBipedInfo_t        biped;
		};

		struct ActorInfoData
		{
			ActorInfoData() :
				data(stl::make_smart_for_overwrite<ActorInfoAggregate>())
			{
			}

			stl::smart_ptr<ActorInfoAggregate> data;
		};

		class UIActorInfo :
			public UIContext,
			public UIWindow,
			UIActorList<ActorInfoData>,
			UIFormInfoTooltipWidget,
			UICollapsibles
		{
			static constexpr auto WINDOW_ID = "ied_ainfo";

			enum class TabItem
			{
				kNone,

				kInventory,
				kFactions,
				kActiveEffects,
				kVariables,
				kActorState,
				kPerks,
				kActorValues,
				kSkeleton,
				kBiped
			};

		public:
			static constexpr auto CHILD_ID = ChildWindowID::kUIActorInfo;

			UIActorInfo(Controller& a_controller);

			void Draw() override;
			void OnOpen() override;
			void OnClose() override;
			void Reset() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			void DrawActorInfo();

			void DrawTabBar(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawInventoryTabItem(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawFactionsTabItem(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawPerkTabItem(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawAVTabItem(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawSkeletonTabItem(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawBipedTabItem(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawEffectsTabItem(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawVariablesTabItem(
				Game::FormID a_handle);

			void DrawActorStateTabItem(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			template <class T>
			bool DrawActorInfoLineFormStringPair(
				T                                           a_label,
				const std::pair<Game::FormID, std::string>& a_data);

			template <class T>
			bool DrawActorInfoLineForm(
				T            a_label,
				Game::FormID a_data);

			void DrawActorInfoContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawActorInfoContentsFirst(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawActorInfoContentsSecond(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawActorInfoContentsThird(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawFactionTreeContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawPerkTreeContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawAVTreeContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawSkeletonTreeContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);
			
			void DrawBipedTreeContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawInventoryTreeContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawEffectTreeContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawVariableTreeContents(
				Game::FormID                    a_handle,
				const conditionalVariableMap_t& a_data);

			void DrawActorStateContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawFilterTree(UIGenericFilter& a_filter);

			void DrawFactionEntries(
				Game::FormID              a_handle,
				const ActorInfoAggregate& a_data);

			void DrawPerkEntries(
				Game::FormID              a_handle,
				const ActorInfoAggregate& a_data);

			void DrawActorValueEntries(
				Game::FormID              a_handle,
				const ActorInfoAggregate& a_data);

			void DrawSkeletonEntries(
				Game::FormID              a_handle,
				const ActorInfoAggregate& a_data);
			
			void DrawBipedEntries(
				Game::FormID              a_handle,
				const ActorInfoAggregate& a_data);

			void DrawSkeletonWeaponNodeTab(
				const char*               a_strid,
				const ActorInfoAggregate& a_data);

			void DrawSkeletonCMENodeTab(
				const char*               a_strid,
				const ActorObjectHolder&  a_holder,
				const ActorInfoAggregate& a_data);

			void DrawSkeletonMOVNodeTab(
				const char*               a_strid,
				const ActorObjectHolder&  a_holder,
				const ActorInfoAggregate& a_data);

			void DrawInventoryEntries(
				Game::FormID              a_handle,
				const ActorInfoAggregate& a_data);

			void DrawEffectFormCell(
				const actorActiveEffectInfo_t::formEntry_t& a_entry,
				bool                                        a_type);

			void DrawEffectEntries(
				Game::FormID              a_handle,
				const ActorInfoAggregate& a_data);

			void DrawVariableEntries(
				Game::FormID                    a_handle,
				const conditionalVariableMap_t& a_data);

			void DrawInventoryBaseTree(
				Game::FormID                      a_handle,
				const actorInventoryInfo_t::Base& a_data);

			void DrawInventoryBase(
				Game::FormID                      a_handle,
				const actorInventoryInfo_t::Base& a_data);

			void DrawInventoryExtraListsTree(
				Game::FormID                      a_handle,
				const ActorInfoAggregate&         a_info,
				const actorInventoryInfo_t::Base& a_data);

			void DrawInventoryExtraLists(
				Game::FormID                      a_handle,
				const ActorInfoAggregate&         a_info,
				const actorInventoryInfo_t::Base& a_data);

			void UpdateActorInfoData(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			virtual ActorInfoData GetData(Game::FormID a_handle) override;

			virtual void ListResetAllValues(Game::FormID a_handle);

			virtual void OnListOptionsChange() override;

			virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void                         OnCollapsibleStatesUpdate();

			void QueueInfoUpdate(Game::FormID a_handle, const ActorInfoData& a_data);

			UIGenericFilter m_invFilter;
			UIGenericFilter m_skelNodeFilter;

			TabItem m_currentTab{ TabItem::kNone };

			char m_buffer[256]{ 0 };

			Controller& m_controller;
		};
	}
}