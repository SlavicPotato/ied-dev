#pragma once

#include "IED/Parsers/JSONConfigCustomHolderParser.h"
#include "IED/Parsers/JSONConfigSlotHolderParser.h"
#include "IED/Parsers/JSONConfigNodeOverrideHolderParser.h"
#include "IED/Parsers/JSONFormFilterBaseParser.h"
#include "IED/Profile/Manager.h"

namespace IED
{
	using SlotProfile = Profile<Data::configSlotHolder_t>;
	using CustomProfile = Profile<Data::configCustomHolder_t>;
	using NodeOverrideProfile = Profile<Data::configNodeOverrideHolder_t>;
	using FormFilterProfile = Profile<Data::configFormFilterBase_t>;

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
		
		class ProfileManagerNodeOverrides :
			public ProfileManager<NodeOverrideProfile>
		{
		public:
			FN_NAMEPROC("ProfileManagerNodeOverrides");

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
		template <class T, std::enable_if_t<std::is_same_v<T, SlotProfile>, int> = 0>
		[[nodiscard]] static constexpr ProfileManager<T>& GetSingleton() noexcept
		{
			return m_slotManager;
		}

		template <class T, std::enable_if_t<std::is_same_v<T, CustomProfile>, int> = 0>
		[[nodiscard]] static constexpr ProfileManager<T>& GetSingleton() noexcept
		{
			return m_customManager;
		}
		
		template <class T, std::enable_if_t<std::is_same_v<T, NodeOverrideProfile>, int> = 0>
		[[nodiscard]] static constexpr ProfileManager<T>& GetSingleton() noexcept
		{
			return m_nodeOverrideManager;
		}
		
		template <class T, std::enable_if_t<std::is_same_v<T, FormFilterProfile>, int> = 0>
		[[nodiscard]] static constexpr ProfileManager<T>& GetSingleton() noexcept
		{
			return m_formFilterManager;
		}

	private:
		static ProfileManagerSlot m_slotManager;
		static ProfileManagerCustom m_customManager;
		static ProfileManagerNodeOverrides m_nodeOverrideManager;
		static ProfileManagerFormFilter m_formFilterManager;
	};

}  // namespace IED