#pragma once

#include "Localization/ILocalization.h"

namespace IED
{
	namespace UI
	{
		class UILocalizationInterface
		{
		protected:
			UILocalizationInterface(
				Localization::ILocalization& a_localization) :
				m_localization(a_localization)
			{
			}

			template <class Te>
			inline constexpr auto& L(Te a_id) const
			{
				return m_localization.L(a_id);
			}

			template <class Te>
			inline constexpr auto LS(Te a_id) const
			{
				return m_localization.L(a_id).c_str();
			}

			template <class Te, std::uint32_t _NumHash = 2>
			inline constexpr const char* LS(
				Te a_id,
				const char* a_im_id) const noexcept
			{
				return LMKID<_NumHash>(LS(a_id), a_im_id);
			}

			template <class Te>
			inline constexpr bool LCG_BM(
				Te a_id,
				const char* a_im_id) const
			{
				return ImGui::BeginMenu(LMKID<2>(LS(a_id), a_im_id));
			}

			template <class Te>
			inline constexpr bool LCG_MI(
				Te a_id,
				const char* a_im_id) const
			{
				return ImGui::MenuItem(LMKID<2>(LS(a_id), a_im_id));
			}

			template <std::uint32_t _NumHash>
			const char* LMKID(
				const char* a_str,
				const char* a_im_id) const noexcept
			{
				auto& buffer = m_localization.m_scBuffer1;

				static_assert(sizeof(buffer) > _NumHash);

				constexpr auto d = sizeof(buffer) - _NumHash - 1;

				auto mslen = d - std::min(std::strlen(a_im_id), d);

				std::size_t i = 0;

				for (; i < mslen; i++)
				{
					auto c = a_str[i];
					if (!c || c == 0x23)
					{
						break;
					}

					buffer[i] = c;
				}

				for (std::uint32_t j = 0; j < _NumHash; j++)
				{
					buffer[i++] = 0x23;
				}

				for (auto p = a_im_id; i < sizeof(buffer) - 1; p++, i++)
				{
					auto c = *p;
					if (!c)
					{
						break;
					}
					buffer[i] = c;
				}

				buffer[i] = 0;

				return buffer;
			}

		private:

			Localization::ILocalization& m_localization;
		};
	}
}