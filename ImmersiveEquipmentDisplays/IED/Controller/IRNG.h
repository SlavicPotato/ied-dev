#pragma once

#include <ext/IRandom.h>

namespace IED
{
	class IRNG
	{
	public:
		IRNG();

		float GetRandomPercent();

	protected:
		RandomNumberGeneratorBase     m_rngBase;
		RandomNumberGenerator3<float> m_rng1;
	};
}
