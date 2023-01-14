#pragma once

namespace IED
{
	namespace UI
	{
		class UIDescriptionPopupWidget
		{
		public:
			bool DrawDescriptionPopup(const char* a_label = nullptr, bool a_autoClosePopup = true);

			void ClearDescriptionPopupBuffer();
			void SetDescriptionPopupBuffer(const std::string& a_text);

			constexpr const auto& GetDescriptionPopupBuffer() const noexcept
			{
				return m_buffer;
			}

		private:
			std::string m_buffer;
		};
	}
}