#pragma once

#include "Localization/ILocalization.h"

namespace IED
{
	namespace UI
	{
		class UILocalizationInterface
		{
		public:
			template <class Te>
			static SKMP_NOINLINE constexpr auto& L(Te a_id)
			{
				return GetLocalizationInterface().L(a_id);
			}

			template <class Te>
			static SKMP_NOINLINE constexpr auto LS(Te a_id)
			{
				return GetLocalizationInterface().L(a_id).c_str();
			}

			template <class Te, std::uint32_t _NumHash = 2>
			static SKMP_NOINLINE constexpr const char* LS(
				Te          a_id,
				const char* a_im_id) noexcept
			{
				return LMKID<_NumHash>(LS(a_id), a_im_id);
			}

			template <class Te>
			static SKMP_NOINLINE constexpr bool LCG_BM(
				Te          a_id,
				const char* a_im_id)
			{
				return ImGui::BeginMenu(LMKID<2>(LS(a_id), a_im_id));
			}

			template <class Te>
			static SKMP_NOINLINE constexpr bool LCG_MI(
				Te          a_id,
				const char* a_im_id)
			{
				return ImGui::MenuItem(LMKID<2>(LS(a_id), a_im_id));
			}

			template <std::uint32_t _NumHash>
			static inline const char* LMKID(
				const char* a_str,
				const char* a_im_id) noexcept
			{
				auto& buffer = GetLocalizationInterface().m_scBuffer1;

				static_assert(sizeof(buffer) > _NumHash);

				constexpr auto d = sizeof(buffer) - _NumHash - 1;

				const auto mslen = d - std::min(std::strlen(a_im_id), d);

				std::uint32_t i = 0;

				for (; i < mslen; i++)
				{
					auto c = a_str[i];
					if (!c || (c == '#' && a_str[i + 1] == '#'))
					{
						break;
					}

					buffer[i] = c;
				}

				for (std::uint32_t j = 0; j < _NumHash; j++)
				{
					buffer[i++] = '#';
				}

				for (auto p = a_im_id; i < sizeof(buffer) - 1; p++, i++)
				{
					if (const auto c = *p)
					{
						buffer[i] = c;
					}
					else
					{
						break;
					}
				}

				assert(i < sizeof(buffer));

				buffer[i] = 0;

				return buffer;
			}

			static Localization::ILocalization& GetLocalizationInterface() noexcept;
		};

		using UIL = UILocalizationInterface;
	}
}