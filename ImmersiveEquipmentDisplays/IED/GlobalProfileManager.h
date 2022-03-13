#pragma once

#include "IED/Parsers/JSONConfigCustomHolderParser.h"
#include "IED/Parsers/JSONConfigNodeOverrideHolderParser.h"
#include "IED/Parsers/JSONConfigSlotHolderParser.h"
#include "IED/Parsers/JSONFormFilterBaseParser.h"

#include "IED/Profile/Manager.h"

namespace IED
{
	using SlotProfile         = Profile<Data::configSlotHolder_t>;
	using CustomProfile       = Profile<Data::configCustomHolder_t>;
	using NodeOverrideProfile = Profile<Data::configNodeOverrideHolder_t>;
	using FormFilterProfile   = Profile<Data::configFormFilterBase_t>;

	class GlobalProfileManager
	{
		class ProfileManagerSlot :
			public ProfileManager<SlotProfile>
		{
		public:
			FN_NAMEPROC("ProfileManagerSlot");

		private:
			using ProfileManager<SlotProfile>::ProfileManager;
		};

		class ProfileManagerCustom :
			public ProfileManager<CustomProfile>
		{
		public:
			FN_NAMEPROC("ProfileManagerCustom");

		private:
			using ProfileManager<CustomProfile>::ProfileManager;
		};

		class ProfileManagerNodeOverride :
			public ProfileManager<NodeOverrideProfile>
		{
		public:
			FN_NAMEPROC("ProfileManagerNodeOverride");

		private:
			using ProfileManager<NodeOverrideProfile>::ProfileManager;
		};

		class ProfileManagerFormFilter :
			public ProfileManager<FormFilterProfile>
		{
		public:
			FN_NAMEPROC("ProfileManagerFormFilter");

		private:
			using ProfileManager<FormFilterProfile>::ProfileManager;
		};

	public:
		template <class T>
		[[nodiscard]] inline static constexpr auto& GetSingleton() noexcept
		{
			if constexpr (std::is_same_v<T, SlotProfile>)
			{
				return m_slotManager;
			}
			else if constexpr (std::is_same_v<T, CustomProfile>)
			{
				return m_customManager;
			}
			else if constexpr (std::is_same_v<T, NodeOverrideProfile>)
			{
				return m_nodeOverrideManager;
			}
			else if constexpr (std::is_same_v<T, FormFilterProfile>)
			{
				return m_formFilterManager;
			}
			else
			{
				static_assert(false, "Unrecognized profile");
			}
		}

	private:
		static ProfileManagerSlot         m_slotManager;
		static ProfileManagerCustom       m_customManager;
		static ProfileManagerNodeOverride m_nodeOverrideManager;
		static ProfileManagerFormFilter   m_formFilterManager;
	};

}