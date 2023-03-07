#pragma once

#if defined(IED_ENABLE_OUTFIT) && defined(IED_ENABLE_OUTFIT_FORM_MANAGER)

#	include "IED/UI/UIFormLookupInterface.h"
#	include "IED/UI/Widgets/Filters/UIGenericFilter.h"
#	include "IED/UI/Widgets/Form/UIFormPickerWidget.h"
#	include "IED/UI/Widgets/UIDescriptionPopup.h"
#	include "IED/UI/Widgets/UIProfileSelectorWidget.h"

#	include "IED/UI/UIContext.h"
#	include "IED/UI/Window/UIWindow.h"

#	include "IED/UI/UILocalizationInterface.h"

#	include "IED/OM/ConfigOutfitForm.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		namespace OM
		{
			struct profileSelectorParamsOutfitForm_t
			{
			};

			class UIOutfitFormManager :
				public UI::UIContext,
				public UI::UIWindow,
				UIProfileSelectorWidget<
					profileSelectorParamsOutfitForm_t,
					OutfitFormListProfile>,
				UI::UIDescriptionPopupWidget,
				UI::UIFormLookupInterface
			{
				enum class UIOutfitFormManagerContextAction
				{
					None,
					Delete,
					Add
				};

				enum class UIOutfitFormManagerFlags
				{
					kNone        = 0,
					kRefreshData = 1u << 0
				};

				static constexpr auto WINDOW_ID = "ied_ofm";

			public:
				static constexpr auto CHILD_ID = ChildWindowID::kOutfitFormManager;

				UIOutfitFormManager(IED::Controller& a_controller);
				~UIOutfitFormManager();

				void Initialize() override;
				void Draw() override;
				void OnClose() override;

				virtual std::uint32_t GetContextID() override
				{
					return static_cast<std::uint32_t>(CHILD_ID);
				}

			private:
				struct CachedOutfitData :
					stl::intrusive_ref_counted
				{
					struct Entry
					{
						inline Entry(const Game::FormID& a_fid) :
							formid(a_fid)
						{
						}

						Game::FormID                               formid;
						stl::vector<Data::configFormZeroMissing_t> list;
					};

					using container_type = stl::flat_map<stl::fixed_string, Entry>;

					void                  Update();
					static container_type MakeData();

					container_type               data;
					mutable stl::recursive_mutex lock;
				};

				using value_type = typename CachedOutfitData::container_type::value_type;

				void UpdateData();

				void                             DrawEntryPanel();
				void                             DrawEntryList();
				UIOutfitFormManagerContextAction DrawEntryContextMenu(const value_type& a_value);
				UIOutfitFormManagerContextAction DrawOutfitItemEntryContextMenu();
				void                             DrawEntry(const value_type& a_value);
				void                             DrawOutfitList(const value_type& a_value);

				void DrawFilterTree();
				void DrawProfileTree();
				void DrawFormInfoText(Game::FormID a_form);

				void QueueCreateOutfitForm(const std::string& a_name);
				//void QueueRenameOutfit(const std::string& a_id, const std::string& a_name);
				void QueueAddOutfitItem(const std::string& a_id, Game::FormID a_itemid);
				void QueueAddOutfitItemList(const std::string& a_id, stl::vector<Data::configFormZeroMissing_t>&& a_list);
				void QueueRemoveOutfitForm(const std::string& a_id);
				void QueueRemoveOutfitItem(const std::string& a_id, std::uint32_t a_index);
				void QueueClearOutfitList(const std::string& a_id);
				void QueueUpdateOutfitItem(const std::string& a_id, std::uint32_t a_index, Game::FormID a_newItem);
				void QueueApplyFormsFromList(const OutfitFormListProfile::base_type& a_list);

				void DrawMenuBar();
				void DrawFileMenu();
				void DrawActionsMenu();

				virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

				virtual OutfitFormListProfile::base_type GetData(
					const profileSelectorParamsOutfitForm_t& a_data) override;

				virtual void ApplyProfile(
					const profileSelectorParamsOutfitForm_t& a_params,
					const OutfitFormListProfile&             a_profile) override;

				UI::UIFormPickerWidget              m_fp;
				UI::UIGenericFilter                 m_filter;
				Game::FormID                        m_newEntryID;
				Game::FormID                        m_updateItemID;
				bool                                m_usefb{ false };
				stl::smart_ptr<CachedOutfitData>    m_data;
				stl::flag<UIOutfitFormManagerFlags> m_flags{ UIOutfitFormManagerFlags::kNone };

				Controller& m_controller;
			};
		}
	}
}

#endif