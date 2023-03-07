#pragma once

#include "IED/UI/Widgets/UIProfileSelectorWidget.h"

#include "Widgets/UIOutfitEntryEditorWidget.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		namespace OM
		{
			enum class UIOutfitEntryListItemWrapperFlags
			{
				kNone = 0,

				kHandleHasData = 1u << 0
			};

			struct UIOutfitEntryListItemWrapper
			{
				struct nodata_arg_t
				{};

				UIOutfitEntryListItemWrapper() = default;

				UIOutfitEntryListItemWrapper(const entryOutfitData_t& a_data) :
					data(a_data),
					flags(UIOutfitEntryListItemWrapperFlags::kHandleHasData)
				{
				}

				UIOutfitEntryListItemWrapper(entryOutfitData_t&& a_data) :
					data(std::move(a_data)),
					flags(UIOutfitEntryListItemWrapperFlags::kHandleHasData)
				{
				}

				UIOutfitEntryListItemWrapper(nodata_arg_t, const entryOutfitData_t& a_data) :
					data(a_data)
				{
				}

				explicit UIOutfitEntryListItemWrapper(nodata_arg_t, entryOutfitData_t&& a_data) :
					data(std::move(a_data))
				{
				}

				entryOutfitData_t                            data;
				stl::flag<UIOutfitEntryListItemWrapperFlags> flags{ UIOutfitEntryListItemWrapperFlags::kNone };
			};

			template <class _Param>
			class UIOutfitEntryEditorCommon :
				public UIOutfitEntryEditorWidget<_Param>,
				public UIProfileSelectorWidget<
					profileSelectorParamsOutfit_t<_Param>,
					OutfitProfile>
			{
			public:
				UIOutfitEntryEditorCommon(
					Controller& a_controller);

				void DrawOutfitEntryWrapper(
					UIOutfitEntryListItemWrapper& a_data,
					const _Param&                 a_params);

				void UpdateConfig(
					const UIOutfitEntryEditorWidget<_Param>::change_param_type& a_params,
					bool                                                        a_syncSex);

				void EraseConfig(
					const UIOutfitEntryEditorWidget<_Param>::change_param_type& a_params);

				virtual OutfitProfile::base_type GetData(
					const profileSelectorParamsOutfit_t<_Param>& a_data) override;

				virtual void DrawMainHeaderControlsExtra(
					const UIOutfitEntryEditorWidget<_Param>::change_param_type& a_params);

				virtual std::optional<std::reference_wrapper<entryOutfitData_t>> GetEntryData(const _Param& a_params) const         = 0;
				virtual entryOutfitData_t&                                       GetOrCreateEntryData(const _Param& a_params) const = 0;
			};

			template <class _Param>
			UIOutfitEntryEditorCommon<_Param>::UIOutfitEntryEditorCommon(
				Controller& a_controller) :
				UIOutfitEntryEditorWidget<_Param>(a_controller),
				UIProfileSelectorWidget<
					profileSelectorParamsOutfit_t<_Param>,
					OutfitProfile>(
					UIProfileSelectorFlags::kEnableApply)
			{
			}

			template <class _Param>
			void UIOutfitEntryEditorCommon<_Param>::DrawOutfitEntryWrapper(
				UIOutfitEntryListItemWrapper& a_data,
				const _Param&                 a_params)
			{
				UIOutfitEntryEditorWidgetParamWrapper<_Param> wrapper{
					a_params,
					!a_data.flags.test(UIOutfitEntryListItemWrapperFlags::kHandleHasData)
				};

				this->DrawOutfitEntryWidget(a_data.data, wrapper);
			}

			template <class _Param>
			void UIOutfitEntryEditorCommon<_Param>::UpdateConfig(
				const UIOutfitEntryEditorWidget<_Param>::change_param_type& a_params,
				bool                                                        a_syncSex)
			{
				auto& src = a_params.data.data;
				auto& dst = GetOrCreateEntryData(a_params.params.actual);

				if (a_syncSex)
				{
					a_params.data.data(Data::GetOppositeSex(a_params.sex)) = src(a_params.sex);
					dst.data                                               = src;
				}
				else
				{
					dst.data(a_params.sex) = src(a_params.sex);
				}
			}

			template <class _Param>
			void UIOutfitEntryEditorCommon<_Param>::EraseConfig(
				const UIOutfitEntryEditorWidget<_Param>::change_param_type& a_params)
			{
			}

			template <class _Param>
			OutfitProfile::base_type UIOutfitEntryEditorCommon<_Param>::GetData(
				const profileSelectorParamsOutfit_t<_Param>& a_data)
			{
				return a_data.data;
			}

			template <class _Param>
			void UIOutfitEntryEditorCommon<_Param>::DrawMainHeaderControlsExtra(
				const UIOutfitEntryEditorWidget<_Param>::change_param_type& a_params)
			{
				ImGui::Separator();

				if (this->TreeEx(
						"tree_prof",
						false,
						"%s",
						UIL::LS(CommonStrings::Profile)))
				{
					ImGui::Spacing();

					profileSelectorParamsOutfit_t params{
						a_params.params.actual,
						a_params.data
					};

					ImGui::PushItemWidth(ImGui::GetFontSize() * -15.5f);

					this->DrawProfileSelector(params);

					ImGui::PopItemWidth();

					ImGui::Spacing();

					ImGui::TreePop();
				}
			}

		}
	}
}