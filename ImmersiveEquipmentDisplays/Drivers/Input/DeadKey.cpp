#include "pch.h"

#include "DeadKey.h"

namespace IED
{
	DeadKey& DeadKey::operator=(WCHAR a_code) noexcept
	{
		m_code = a_code;
		m_comb = translate(a_code);

		return *this;
	}

	WCHAR DeadKey::translate(WCHAR a_code) noexcept
	{
		switch (a_code)
		{
		case 0x5E:  // Circumflex accent:
			return 0x302;
		case 0x02C7:  // Caron
			return 0x30C;
		case 0x2D8:  // Breve
			return 0x306;
		case 0x2DB:  // Ogonek
			return 0x328;
		case 0x2D9:  // Dot above
			return 0x0307;
		case 0x2DD:  // Double acute accent
			return 0x30B;
		case 0x60:  // Grave accent:
			return 0x300;
		case 0xA8:  // Diaeresis:
			return 0x308;
		case 0xB0:  // Degree
			return 0x030A;
		case 0xB4:  // Acute accent:
			return 0x301;
		case 0xB8:  // Cedilla:
			return 0x327;
		default:
			return a_code;
		}
	}
}