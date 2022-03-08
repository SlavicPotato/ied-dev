#pragma once

namespace IED
{
	namespace UI
	{
		class UIDescriptionPopupWidget
		{
		public:
			bool DrawDescriptionPopup(const char* a_label = nullptr);

			void ClearDescriptionPopupBuffer();
			void SetDescriptionPopupBuffer(const std::string& a_text);

			inline constexpr const auto& GetDescriptionPopupBuffer() const noexcept
			{
				return m_buffer;
			}

		private:
			std::string m_buffer;
		};
	}
}