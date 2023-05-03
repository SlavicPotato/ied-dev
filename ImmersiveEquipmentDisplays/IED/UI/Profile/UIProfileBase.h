#pragma once

#include "IED/Profile/Manager.h"
#include "IED/StringHolder.h"

#include "IED/UI/Modals/UICommonModals.h"
#include "IED/UI/PopupQueue/UIPopupQueue.h"
#include "IED/UI/UILocalizationInterface.h"

#include "UIProfileStrings.h"

namespace IED
{
	namespace UI
	{
		template <class T>
		class UIProfileBase :
			protected UICommonModals,
			protected Events::EventSink<ProfileManagerEvent<T>>
		{
		public:
			void InitializeProfileBase();

			const T* GetCurrentProfile() const;

			virtual ~UIProfileBase() noexcept = default;

		protected:
			static constexpr auto POPUP_NEW_ID = "popup_new";

			UIProfileBase() = default;

			void DrawCreateNew(std::unique_ptr<typename T::base_type> a_data = {});

			virtual ProfileManager<T>& GetProfileManager() const         = 0;
			virtual UIPopupQueue&      GetPopupQueue_ProfileBase() const = 0;

			virtual bool AllowCreateNew() const;
			virtual bool AllowSave() const;
			virtual void OnItemSelected(const stl::fixed_string& a_item);

			virtual void OnProfileAdd(const stl::fixed_string& a_name, T& a_profile);
			virtual void OnProfileSave(const stl::fixed_string& a_name, T& a_profile);
			virtual void OnProfileDelete(const stl::fixed_string& a_item);
			virtual void OnProfileRename(
				const stl::fixed_string& a_oldName,
				const stl::fixed_string& a_newName);

			virtual void Receive(const ProfileManagerEvent<T>& a_evn) override;

			void SetSelected(const stl::fixed_string& a_item);
			void SetSelected(stl::fixed_string&& a_item);

			struct
			{
				std::string                      new_input;
				std::optional<stl::fixed_string> selected;
				except::descriptor               lastException;
			} m_state;
		};

		template <class T>
		void UIProfileBase<T>::InitializeProfileBase()
		{
			GetProfileManager().AddSink(this);
		}

		template <class T>
		const T* UIProfileBase<T>::GetCurrentProfile() const
		{
			if (!m_state.selected)
			{
				return nullptr;
			}

			auto& data = GetProfileManager();

			auto it = data.Find(*m_state.selected);
			if (it != data.End())
			{
				return std::addressof(it->second);
			}

			return nullptr;
		}

		template <class T>
		void UIProfileBase<T>::DrawCreateNew(
			std::unique_ptr<typename T::base_type> a_data)
		{
			if (TextInputDialog(
					UIL::LS(UIProfileStrings::NewProfile, POPUP_NEW_ID),
					"%s",
					ImGuiInputTextFlags_EnterReturnsTrue,
					{},
					{},
					false,
					m_state.new_input,
					UIL::LS(UIProfileStrings::ProfileNamePrompt)) != ModalStatus::kAccept)
			{
				return;
			}

			if (m_state.new_input.empty())
			{
				return;
			}

			auto& pm = GetProfileManager();

			const auto profile =
				a_data ?
					std::make_unique<T>(std::move(*a_data)) :
					std::make_unique<T>();

			const bool createResult =
				pm.CreateProfile(m_state.new_input, *profile, true);

			m_state.new_input.clear();

			if (!createResult)
			{
				m_state.lastException = pm.GetLastException();

				GetPopupQueue_ProfileBase().push(
					UIPopupType::Message,
					UIL::LS(CommonStrings::Error),
					"%s\n\n%s",
					UIL::LS(UIProfileStrings::CreateError),
					pm.GetLastException().what());

				return;
			}

			const auto name(profile->Name());

			if (pm.AddProfile(std::move(*profile)))
			{
				SetSelected(std::move(name));
			}
			else
			{
				m_state.lastException = pm.GetLastException();

				GetPopupQueue_ProfileBase().push(
					UIPopupType::Message,
					UIL::LS(CommonStrings::Error),
					"%s\n\n%s",
					UIL::LS(UIProfileStrings::AddError),
					pm.GetLastException().what());
			}
		}

		template <class T>
		bool UIProfileBase<T>::AllowCreateNew() const
		{
			return true;
		}

		template <class T>
		bool UIProfileBase<T>::AllowSave() const
		{
			return true;
		}

		template <class T>
		void UIProfileBase<T>::OnItemSelected(
			const stl::fixed_string& a_item)
		{
		}

		template <class T>
		void UIProfileBase<T>::OnProfileAdd(
			const stl::fixed_string& a_name,
			T&                       a_profile)
		{
		}

		template <class T>
		void UIProfileBase<T>::OnProfileSave(
			const stl::fixed_string& a_name,
			T&                       a_profile)
		{
		}

		template <class T>
		void UIProfileBase<T>::OnProfileDelete(
			const stl::fixed_string& a_item)
		{
		}

		template <class T>
		void UIProfileBase<T>::OnProfileRename(
			const stl::fixed_string& a_oldName,
			const stl::fixed_string& a_newName)
		{
		}

		template <class T>
		void UIProfileBase<T>::Receive(const ProfileManagerEvent<T>& a_evn)
		{
			switch (a_evn.m_type)
			{
			case ProfileManagerEvent<T>::EventType::kProfileAdd:
				OnProfileAdd(*a_evn.m_profile, *a_evn.m_data);
				break;
			case ProfileManagerEvent<T>::EventType::kProfileDelete:
				OnProfileDelete(*a_evn.m_profile);
				if (m_state.selected == *a_evn.m_profile)
				{
					m_state.selected.reset();
				}
				break;
			case ProfileManagerEvent<T>::EventType::kProfileSave:
				OnProfileSave(*a_evn.m_profile, *a_evn.m_data);
				break;
			case ProfileManagerEvent<T>::EventType::kProfileRename:
				OnProfileRename(*a_evn.m_oldProfile, *a_evn.m_profile);
				if (m_state.selected == *a_evn.m_oldProfile)
				{
					m_state.selected = *a_evn.m_profile;
				}
				break;
			}
		}

		template <class T>
		void UIProfileBase<T>::SetSelected(const stl::fixed_string& a_item)
		{
			m_state.selected = a_item;
			OnItemSelected(*m_state.selected);
		}

		template <class T>
		void UIProfileBase<T>::SetSelected(stl::fixed_string&& a_item)
		{
			m_state.selected = std::move(a_item);
			OnItemSelected(*m_state.selected);
		}

	}
}