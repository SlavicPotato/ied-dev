#pragma once

#include "Controls/UICollapsibles.h"
#include "UIMiscTextInterface.h"
#include "Widgets/Filters/UIGenericFilter.h"
#include "Widgets/Lists/UIActorListWidget.h"

#include "UIContext.h"
#include "Window/UIWindow.h"

#include "UILocalizationInterface.h"

#include "IED/ISkeletonInfo.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		struct ActorSkeletonData
		{
			ActorSkeletonData() :
				data(std::make_shared<SkeletonInfoLookupResult::element_type>())
			{
			}

			SkeletonInfoLookupResult data;
		};

		class UISkeletonExplorer :
			public UIContext,
			public UIWindow,
			UIActorList<ActorSkeletonData>,
			UICollapsibles,
			UIMiscTextInterface
		{
			static constexpr auto WINDOW_ID = "ied_skexp";

		public:
			static constexpr auto CHILD_ID = ChildWindowID::kUISkeletonExplorer;

			UISkeletonExplorer(Controller& a_controller);

			void Draw() override;
			void OnOpen() override;
			void OnClose() override;
			void Reset() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			void DrawOptions();
			void DrawFilterTree();
			void DrawSkeletonDataHeader(const SI_Root& a_data);
			void DrawSkeletonTreePanel();
			void DrawSkeletonTree(Game::FormID a_handle, const ActorSkeletonData& a_data);
			void UpdateSkeletonData(Game::FormID a_handle, const ActorSkeletonData& a_data);
			void DrawRoot(Game::FormID a_handle, const SI_Root& a_data);
			void DrawObject(const SI_Root& a_data, const SI_NiObject& a_object, std::uint32_t a_index, bool a_disableFilter);
			void DrawChildNodes(const SI_Root& a_data, const SI_NiObject& a_object, bool a_disableFilter);
			void DrawObjectInfo(const SI_NiObject& a_data, bool a_drawWorld);

			virtual ActorSkeletonData GetData(Game::FormID a_handle) override;

			virtual void ListResetAllValues(Game::FormID a_handle);

			virtual void OnListOptionsChange() override;

			virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void                         OnCollapsibleStatesUpdate();

			UIGenericFilter m_nodeFilter;

			Controller& m_controller;
		};

	}
}