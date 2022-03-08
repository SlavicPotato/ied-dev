#pragma once

#include "IED/UI/Profile/UIProfileSelectorBase.h"

namespace IED
{
	namespace UI
	{
		template <class T, class P>
		class UIProfileSelectorWidget :
			public UIProfileSelectorBase<T, P>
		{
		public:
			using UIProfileSelectorBase<T, P>::UIProfileSelectorBase;

		protected:
			virtual ProfileManager<P>& GetProfileManager() const override;
		};

		template <class T, class P>
		ProfileManager<P>& UIProfileSelectorWidget<T, P>::GetProfileManager() const
		{
			return GlobalProfileManager::GetSingleton<P>();
		}

	}
}