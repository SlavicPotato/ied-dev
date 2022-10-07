#include "pch.h"

#include "UICustomEditorGlobal.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UICustomEditorGlobal::UICustomEditorGlobal(
			Controller& a_controller) :
			UICustomEditorCommon<int>(a_controller),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			UINotificationInterface(a_controller),
			UISettingsInterface(a_controller),
			UITransformSliderWidget(a_controller),
			UIFormTypeSelectorWidget(a_controller),
			UISimpleStringSetWidget(a_controller),
			m_controller(a_controller)
		{
		}

		void UICustomEditorGlobal::EditorInitialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.customEditor.globalSex, false);
		}

		void UICustomEditorGlobal::EditorDraw()
		{
			if (ImGui::BeginChild("custom_editor_global", { -1.0f, 0.0f }))
			{
				DrawCustomEditor(0, m_data);
			}

			ImGui::EndChild();
		}

		void UICustomEditorGlobal::EditorOnOpen()
		{
			UpdateData();
		}

		void UICustomEditorGlobal::EditorOnClose()
		{
			m_data = {};
		}

		void UICustomEditorGlobal::EditorReset()
		{
			UpdateData();
		}

		void UICustomEditorGlobal::EditorQueueUpdateCurrent()
		{
			UpdateData();
		}

		constexpr Data::ConfigClass UICustomEditorGlobal::GetConfigClass() const
		{
			return Data::ConfigClass::Global;
		}

		Data::configCustomHolder_t& UICustomEditorGlobal::GetOrCreateConfigSlotHolder(
			int a_handle) const
		{
			auto& data = m_controller.GetConfigStore().active.custom.GetGlobalData()[0];
			auto& sh   = StringHolder::GetSingleton();

			return data.try_emplace(sh.IED).first->second;
		}

		auto UICustomEditorGlobal::GetCurrentData()
			-> CustomEditorCurrentData
		{
			return { 0, std::addressof(m_data) };
		}

		UIPopupQueue& UICustomEditorGlobal::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UICustomEditorGlobal::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIData::UICollapsibleStates& UICustomEditorGlobal::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			return settings.data.ui.customEditor
			    .colStates[stl::underlying(Data::ConfigClass::Global)];
		}

		void UICustomEditorGlobal::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		Data::SettingHolder::EditorPanelCommon& UICustomEditorGlobal::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.customEditor;
		}

		void UICustomEditorGlobal::OnEditorPanelSettingsChange()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UICustomEditorGlobal::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.customEditor.globalSex != a_newSex)
			{
				store.settings.set(
					store.settings.data.ui.customEditor.globalSex,
					a_newSex);
			}
		}

		void UICustomEditorGlobal::ApplyProfile(
			const profileSelectorParamsCustom_t<int>& a_data,
			const CustomProfile&                      a_profile)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_data.handle);

			a_data.data = a_profile.Data();
			conf        = a_profile.Data();

			UpdateData();

			m_controller.QueueResetCustomAll(
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		void UICustomEditorGlobal::MergeProfile(
			const profileSelectorParamsCustom_t<int>& a_data,
			const CustomProfile&                      a_profile)
		{
			auto& profileData = a_profile.Data();

			for (auto& [i, e] : profileData.data)
			{
				a_data.data.data.insert_or_assign(i, e);
			}

			GetOrCreateConfigSlotHolder(a_data.handle) = a_data.data;

			UpdateData();

			m_controller.QueueResetCustomAll(
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		void UICustomEditorGlobal::OnBaseConfigChange(
			int              a_handle,
			const void*      a_params,
			PostChangeAction a_action)
		{
			auto params = static_cast<const SingleCustomConfigUpdateParams*>(a_params);

			auto& store = m_controller.GetConfigStore();

			UpdateConfig(a_handle, *params, store.settings.data.ui.customEditor.sexSync);

			switch (a_action)
			{
			case PostChangeAction::Evaluate:
				m_controller.QueueEvaluateAll(
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
					ControllerUpdateFlags::kImmediateTransformUpdate);
				break;
			case PostChangeAction::Reset:
				m_controller.QueueResetCustomAll(
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					params->name);
				break;
			case PostChangeAction::UpdateTransform:
				m_controller.QueueUpdateTransformCustomAll(
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					params->name);
				break;
			}
		}

		void UICustomEditorGlobal::OnFullConfigChange(
			int                             a_handle,
			const CustomConfigUpdateParams& a_params)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_handle);

			conf = a_params.data;

			m_controller.QueueResetCustomAll(
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		bool UICustomEditorGlobal::OnCreateNew(
			int                          a_handle,
			const CustomConfigNewParams& a_params)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_handle);

			auto r = conf.data.try_emplace(a_params.name, a_params.entry).second;

			if (r)
			{
				m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
			}

			return r;
		}

		void UICustomEditorGlobal::OnErase(
			int                            a_handle,
			const CustomConfigEraseParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.custom.GetGlobalData()[0];

			if (EraseConfig(data, a_params.name))
			{
				m_controller.QueueResetCustomAll(
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					a_params.name);
			}
		}

		bool UICustomEditorGlobal::OnRename(
			int                             a_handle,
			const CustomConfigRenameParams& a_params)
		{
			if (!DoConfigRename(a_handle, a_params))
			{
				return false;
			}

			m_controller.QueueResetCustomAll(
				GetConfigClass(),
				StringHolder::GetSingleton().IED,
				a_params.oldName);

			return true;
		}

		void UICustomEditorGlobal::UpdateData()
		{
			auto& data = m_controller.GetConfigStore().active.custom.GetGlobalData()[0];
			auto& sh   = StringHolder::GetSingleton();

			auto it = data.find(sh.IED);
			if (it != data.end())
			{
				m_data = it->second;
			}
			else
			{
				m_data = {};
			}
		}
	}
}