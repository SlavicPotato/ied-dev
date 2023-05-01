#pragma once

#include "IED/UI/UILocalizationInterface.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		class UICommonModals
		{
		public:
			using cm_func_t = std::function<bool()>;

			enum class ModalStatus
			{
				kNoAction,
				kReject,  // default for message dialog when no action
				kAccept
			};

			UICommonModals() = default;

			static ModalStatus TextInputDialog(
				const char*                 a_name,
				const char*                 a_text,
				ImGuiInputTextFlags         a_flags,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				bool                        a_allowEmpty,
				std::string&                a_out);

			template <class... Args>
			static ModalStatus TextInputDialog(
				const char*                 a_name,
				const char*                 a_text,
				ImGuiInputTextFlags         a_flags,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				bool                        a_allowEmpty,
				std::string&                a_out,
				Args... args);

			template <class... Args>
			static ModalStatus TextInputMultilineDialog(
				const char*                 a_name,
				const char*                 a_text,
				ImGuiInputTextFlags         a_flags,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				bool                        a_allowEmpty,
				std::string&                a_out,
				Args... args);

			static ModalStatus MessageDialog(
				const char*                 name,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				const char*                 text);

			template <class... Args>
			static ModalStatus MessageDialog(
				const char*                 name,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				const char*                 text,
				Args... args);

			static ModalStatus ConfirmDialog(
				const char*                 name,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				const char*                 text);

			template <class... Args>
			static ModalStatus ConfirmDialog(
				const char*                 name,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				const char*                 text,
				Args... args);

			template <class... Args>
			static ModalStatus CustomDialog(
				const char*      name,
				const cm_func_t& a_func);

		private:
			template <class... Args>
			static ModalStatus TextInputDialogImpl(
				bool                        a_multiLine,
				const char*                 a_name,
				const char*                 a_text,
				ImGuiInputTextFlags         a_flags,
				const cm_func_t&            a_func,
				const std::optional<float>& a_twsz,
				bool                        a_allowEmpty,
				std::string&                a_out,
				Args... args);
		};

		template <class... Args>
		auto UICommonModals::TextInputDialog(
			const char*                 a_name,
			const char*                 a_text,
			ImGuiInputTextFlags         a_flags,
			const cm_func_t&            a_func,
			const std::optional<float>& a_twsz,
			bool                        a_allowEmpty,
			std::string&                a_out,
			Args... a_args)
			-> ModalStatus
		{
			return TextInputDialogImpl(
				false,
				a_name,
				a_text,
				a_flags,
				a_func,
				a_twsz,
				a_allowEmpty,
				a_out,
				a_args...);
		}

		template <class... Args>
		auto UICommonModals::TextInputMultilineDialog(
			const char*                 a_name,
			const char*                 a_text,
			ImGuiInputTextFlags         a_flags,
			const cm_func_t&            a_func,
			const std::optional<float>& a_twsz,
			bool                        a_allowEmpty,
			std::string&                a_out,
			Args... a_args)
			-> ModalStatus
		{
			return TextInputDialogImpl(
				true,
				a_name,
				a_text,
				a_flags,
				a_func,
				a_twsz,
				a_allowEmpty,
				a_out,
				a_args...);
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
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * (a_twsz ? *a_twsz : 50.0f));
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
						UIL::LS(CommonStrings::OK, "ctl_1"),
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
					UIL::LS(CommonStrings::OK, "ctl_1"),
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
						UIL::LS(CommonStrings::Cancel, "ctl_2"),
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

		template <class... Args>
		auto UICommonModals::TextInputDialogImpl(
			bool                        a_multiLine,
			const char*                 a_name,
			const char*                 a_text,
			ImGuiInputTextFlags         a_flags,
			const cm_func_t&            a_func,
			const std::optional<float>& a_twsz,
			bool                        a_allowEmpty,
			std::string&                a_out,
			Args... a_args)
			-> ModalStatus
		{
			ModalStatus ret{ ModalStatus::kNoAction };

			auto& io = ImGui::GetIO();

			ImGui::SetNextWindowPos(
				{ io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f },
				ImGuiCond_Appearing,
				{ 0.5f, 0.5f });

			const auto modalFlags =
				a_multiLine ?
					ImGuiWindowFlags_None :
					ImGuiWindowFlags_AlwaysAutoResize;

			if (ImGui::BeginPopupModal(
					a_name,
					nullptr,
					modalFlags))
			{
				if (a_text && a_text[0] != 0)
				{
					if (modalFlags & ImGuiWindowFlags_AlwaysAutoResize)
					{
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * (a_twsz ? *a_twsz : 25.0f));
					}

					ImGui::Text(a_text, a_args...);

					if (modalFlags & ImGuiWindowFlags_AlwaysAutoResize)
					{
						ImGui::PopTextWrapPos();
					}

					ImGui::Spacing();
				}

				if (ImGui::IsWindowAppearing())
				{
					ImGui::SetKeyboardFocusHere();
				}

				bool inputResult;

				if (a_multiLine)
				{
					const auto offsetY =
						ImGui::GetFontSize() + (ImGui::GetStyle().ItemInnerSpacing.y * 2.f) + 3.f;

					inputResult = ImGui::InputTextMultiline(
						"##text_input_ml",
						std::addressof(a_out),
						{ -1, -offsetY },
						a_flags);
				}
				else
				{
					inputResult = ImGui::InputText(
						"##text_input",
						std::addressof(a_out),
						a_flags);
				}

				if (inputResult)
				{
					ImGui::CloseCurrentPopup();
					ret = ModalStatus::kAccept;
				}

				if (!a_multiLine)
				{
					ImGui::Spacing();
					ImGui::Separator();
				}

				bool disabled = false;

				if (a_func)
				{
					ImGui::PushID("extra");
					disabled = !a_func();
					ImGui::PopID();

					ImGui::Separator();
				}

				if (!a_allowEmpty && a_out.empty())
				{
					disabled = true;
				}

				UICommon::PushDisabled(disabled);

				if (ImGui::Button(
						UIL::LS(CommonStrings::OK, "ctl_1"),
						{ 120.f, 0.f }))
				{
					ImGui::CloseCurrentPopup();
					ret = ModalStatus::kAccept;
				}

				UICommon::PopDisabled(disabled);

				ImGui::SameLine();

				ImGui::SetItemDefaultFocus();
				if (ImGui::Button(
						UIL::LS(CommonStrings::Cancel, "ctl_2"),
						{ 120.f, 0.f }))
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
