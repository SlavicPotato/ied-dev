#include "pch.h"

#include "UIClipboard.h"

#include "IED/ConfigCustom.h"

namespace IED
{
	namespace UI
	{
		UIClipboard UIClipboard::m_Instance;

		const Data::configCustom_t& UIClipboard::get_value(
			const Data::configCustomNameValue_t& a_data) noexcept
		{
			return a_data.data(a_data.sex);
		}

		void UIClipboard::Clear()
		{
			m_Instance.m_data.reset();
		}

	}
}