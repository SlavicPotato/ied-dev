#pragma once

#include "Controls/UICollapsibles.h"
#include "Widgets/Filters/UIGenericFilter.h"
#include "Widgets/Lists/UIActorListWidget.h"

#include "Window/UIWindow.h"
#include "Window/UIWindowBase.h"

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
			public UIWindow,
			public UIWindowBase,
			UICollapsibles,
			UIActorList<ActorSkeletonData>,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID = "ied_skexp";

		public:
			UISkeletonExplorer(Controller& a_controller);

			void Draw();
			void OnOpen();
			void OnClose();
			void Reset();

		private:
			void DrawOptions();
			void DrawFilterTree();
			void DrawSkeletonDataHeader(const ActorSkeletonData& a_data);
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