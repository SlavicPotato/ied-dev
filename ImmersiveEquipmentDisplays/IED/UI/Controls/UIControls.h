#pragma once

namespace IED
{
	namespace UI
	{
		class UIControls
		{
		protected:
			template <class... Args>
			bool SliderFloat(
				const char* a_label,
				float* a_value,
				float a_min,
				float a_max,
				Args... a_args) const;

			template <class... Args>
			bool SliderFloat3(
				const char* a_label,
				float* a_value,
				float a_min,
				float a_max,
				Args... a_args) const;

			template <class... Args>
			bool SliderInt(
				const char* a_label,
				int* a_value,
				int a_min,
				int a_max,
				Args... a_args)
				const;

			template <class... Args>
			bool Checkbox(Args... a_args) const;

			bool ColorEdit4(
				const char* a_label,
				float (&a_col)[4],
				ImGuiColorEditFlags a_flags = 0);

		private:
			virtual void OnControlValueChange() const;
		};

		template <class... Args>
		bool UIControls::SliderFloat(
			const char* a_label,
			float* a_value,
			float a_min,
			float a_max,
			Args... a_args) const
		{
			bool res = ImGui::SliderFloat(
				a_label,
				a_value,
				a_min,
				a_max,
				std::forward<Args>(a_args)...);

			if (res)
			{
				*a_value = std::clamp(*a_value, a_min, a_max);

				OnControlValueChange();
			}

			return res;
		}

		template <class... Args>
		bool UIControls::SliderFloat3(
			const char* a_label,
			float* a_value,
			float a_min,
			float a_max,
			Args... a_args) const
		{
			bool res = ImGui::SliderFloat3(
				a_label,
				a_value,
				a_min,
				a_max,
				std::forward<Args>(a_args)...);

			if (res)
			{
				for (std::uint32_t i = 0; i < 3; i++)
				{
					a_value[i] = std::clamp(a_value[i], a_min, a_max);
				}

				OnControlValueChange();
			}

			return res;
		}

		template <class... Args>
		bool UIControls::SliderInt(
			const char* a_label,
			int* a_value,
			int a_min,
			int a_max,
			Args... a_args)
			const
		{
			bool res = ImGui::SliderInt(
				a_label,
				a_value,
				a_min,
				a_max,
				std::forward<Args>(a_args)...);

			if (res)
			{
				*a_value = std::clamp(*a_value, a_min, a_max);

				OnControlValueChange();
			}

			return res;
		}

		template <class... Args>
		bool UIControls::Checkbox(Args... a_args) const
		{
			bool res = ImGui::Checkbox(std::forward<Args>(a_args)...);

			if (res)
				OnControlValueChange();

			return res;
		}

	}
}