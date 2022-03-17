#pragma once

#include "IED/UI/UILocalizationInterface.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		class UICommonModals :
			public virtual UILocalizationInterface
		{
		public:
			using cm_func_t = std::function<bool()>;

			enum class ModalStatus
			{
				kNoAction,
				kReject,  // default for message dialog when no action
				kAccept
			};

			UICommonModals(Localization::ILocalization& a_localization);

			ModalStatus TextInputDialog(
				const char*                 a_name,
				const char*                 a_text,
				ImGuiInputTextFlags         a_flags,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				char*                       a_buf,
				std::size_t                 a_size);

			template <class... Args>
			ModalStatus TextInputDialog(
				const char*                 a_name,
				const char*                 a_text,
				ImGuiInputTextFlags         a_flags,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				char*                       a_buf,
				std::size_t                 a_size,
				Args... args);

			ModalStatus MessageDialog(
				const char*                 name,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				const char*                 text);

			template <class... Args>
			ModalStatus MessageDialog(
				const char*                 name,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				const char*                 text,
				Args... args);

			ModalStatus ConfirmDialog(
				const char*                 name,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				const char*                 text);

			template <class... Args>
			ModalStatus ConfirmDialog(
				const char*                 name,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				const char*                 text,
				Args... args);

			template <class... Args>
			ModalStatus CustomDialog(
				const char*      name,
				const cm_func_t& a_func);
		};

		template <class... Args>
		auto UICommonModals::TextInputDialog(
			const char*                 a_name,
			const char*                 a_text,
			ImGuiInputTextFlags         a_flags,
			const cm_func_t&            a_func,
			const std::optional<float>& a_twsz,
			char*                       a_buf,
			std::size_t                 a_size,
			Args... args)
			-> ModalStatus
		{
			ModalStatus ret{ ModalStatus::kNoAction };

			auto& io = ImGui::GetIO();

			ImGui::SetNextWindowPos(
				{ io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f },
				ImGuiCond_Appearing,
				{ 0.5f, 0.5f });

			if (ImGui::BeginPopupModal(
					a_name,
					nullptr,
					ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * (a_twsz ? *a_twsz : 25.0f));
				ImGui::Text(a_text, args...);
				ImGui::PopTextWrapPos();
				ImGui::Spacing();

				if (ImGui::IsWindowAppearing())
					ImGui::SetKeyboardFocusHere();

				if (ImGui::InputText(
						"##text_input",
						a_buf,
						a_size,
						a_flags))
				{
					ImGui::CloseCurrentPopup();
					ret = ModalStatus::kAccept;
				}

				ImGui::Spacing();
				ImGui::Separator();

				bool disabled = false;

				if (a_func)
				{
					ImGui::PushID("extra");
					disabled = !a_func();
					ImGui::PopID();

					ImGui::Separator();
				}

				if (a_buf[0] == 0)
				{
					disabled = true;
				}

				UICommon::PushDisabled(disabled);

				if (ImGui::Button(
						LS(CommonStrings::OK, "ctl_1"),
						{ 120.f, 0.f }))
				{
					ImGui::CloseCurrentPopup();
					ret = ModalStatus::kAccept;
				}

				UICommon::PopDisabled(disabled);

				ImGui::SameLine();

				ImGui::SetItemDefaultFocus();
				if (ImGui::Button(
						LS(CommonStrings::Cancel, "ctl_2"),
						{ 120.f, 0.f }))
				{
					ImGui::CloseCurrentPopup();
					ret = ModalStatus::kReject;
				}

				ImGui::EndPopup();
			}

			return ret;
		}

		template <class... Args>
		auto UICommonModals::MessageDialog(
			const char*                 a_name,
			const cm_func_t&            a_func,
			const std::optional<float>& a_twsz,
			const char*                 a_text,
			Args... args)
			-> ModalStatus
		{
			ModalStatus ret{ ModalStatus::kNoAction };

			auto& io = ImGui::GetIO();

			ImGui::SetNextWindowPos(
				{ io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f },
				ImGuiCond_Appearing,
				{ 0.5f, 0.5f });

			if (ImGui::BeginPopupModal(
					a_name,
					nullptr,
					ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * (a_twsz ? *a_twsz : 25.0f));
				ImGui::Text(a_text, args...);
				ImGui::PopTextWrapPos();
				ImGui::Spacing();
				ImGui::Separator();

				if (a_func)
				{
					ImGui::PushID("extra");
					a_func();
					ImGui::PopID();

					ImGui::Separator();
				}

				ImGui::SetItemDefaultFocus();
				if (ImGui::Button(
						LS(CommonStrings::OK, "ctl_1"),
						{ 120.0f, 0.0f }))
				{
					ImGui::CloseCurrentPopup();
					ret = ModalStatus::kReject;
				}

				ImGui::EndPopup();
			}

			return ret;
		}

		template <class... Args>
		auto UICommonModals::ConfirmDialog(
			const char*                 name,
			const cm_func_t&            a_func,
			const std::optional<float>& a_twsz,
			const char*                 text,
			Args... args)
			-> ModalStatus
		{
			ModalStatus ret{ ModalStatus::kNoAction };

			auto& io = ImGui::GetIO();

			ImGui::SetNextWindowPos(
				{ io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f },
				ImGuiCond_Appearing,
				{ 0.5f, 0.5f });

			if (ImGui::BeginPopupModal(
					name,
					nullptr,
					ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * (a_twsz ? *a_twsz : 25.0f));
				ImGui::Text(text, args...);
				ImGui::PopTextWrapPos();
				ImGui::Spacing();
				ImGui::Separator();

				bool disabled = false;

				if (a_func)
				{
					ImGui::PushID("extra");
					disabled = !a_func();
					ImGui::PopID();

					ImGui::Separator();
				}

				UICommon::PushDisabled(disabled);

				bool r = ImGui::Button(
					LS(CommonStrings::OK, "ctl_1"),
					{ 120.0f, 0.0f });

				UICommon::PopDisabled(disabled);

				if (r)
				{
					ImGui::CloseCurrentPopup();
					ret = ModalStatus::kAccept;
				}

				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if (ImGui::Button(
						LS(CommonStrings::Cancel, "ctl_2"),
						{ 120.0f, 0.0f }))
				{
					ImGui::CloseCurrentPopup();
					ret = ModalStatus::kReject;
				}

				ImGui::EndPopup();
			}

			return ret;
		}

		template <class... Args>
		auto UICommonModals::CustomDialog(
			const char*      name,
			const cm_func_t& a_func)
			-> ModalStatus
		{
			ModalStatus ret{ ModalStatus::kNoAction };

			auto& io = ImGui::GetIO();

			ImGui::SetNextWindowPos(
				{ io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f },
				ImGuiCond_Appearing,
				{ 0.5f, 0.5f });

			if (ImGui::BeginPopupModal(
					name,
					nullptr,
					ImGuiWindowFlags_AlwaysAutoResize))
			{
				bool close = false;

				if (a_func)
				{
					ImGui::PushID("extra");
					close = a_func();
					ImGui::PopID();
				}

				if (close)
				{
					ImGui::CloseCurrentPopup();
					ret = ModalStatus::kReject;
				}

				ImGui::EndPopup();
			}

			return ret;
		}
	}
}
