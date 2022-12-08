#include "pch.h"

#include "UILocalizationInterface.h"

#include "IED/Main.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		Localization::ILocalization& UILocalizationInterface::GetLocalizationInterface() noexcept
		{
			return static_cast<Localization::ILocalization&>(*Initializer::GetController());
		}
	}
}