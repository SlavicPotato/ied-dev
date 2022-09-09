#include "pch.h"

#include "IRNG.h"

namespace IED
{
	IRNG::IRNG() :
		m_rng1(0.0f, 100.0f)
	{
	}

	float IRNG::GetRandomPercent()
	{
		try
		{
			return m_rng1.Get(m_rngBase);
		}
		catch (...)
		{
			return 0.0f;
		}
	}
}