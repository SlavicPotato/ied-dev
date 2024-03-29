#pragma once

#include "IED/Parsers/JSONConfigConditionalVariablesHolderParser.h"
#include "IED/Parsers/JSONConfigCustomHolderParser.h"
#include "IED/Parsers/JSONConfigKeybindEntryHolderParser.h"
#include "IED/Parsers/JSONConfigNodeOverrideHolderParser.h"
#include "IED/Parsers/JSONConfigSlotHolderParser.h"
#include "IED/Parsers/JSONFormFilterBaseParser.h"

#include "IED/OM/Parsers/JSONConfigOutfitEntryHolderParser.h"
#include "IED/OM/Parsers/JSONConfigOutfitFormListParser.h"


#include "IED/Profile/Manager.h"

#include "IED/D3D/D3DAssets.h"

namespace IED
{
	using SlotProfile           = Profile<Data::configSlotHolder_t>;
	using CustomProfile         = Profile<Data::configCustomHolder_t>;
	using NodeOverrideProfile   = Profile<Data::configNodeOverrideHolder_t>;
	using FormFilterProfile     = Profile<Data::configFormFilterBase_t>;
	using CondVarProfile        = Profile<Data::configConditionalVariablesHolder_t>;
	using OutfitProfile         = Profile<Data::OM::configOutfitEntryHolder_t>;
	using OutfitFormListProfile = Profile<Data::OM::configOutfitFormList_t>;
	using KeybindProfile        = Profile<Data::configKeybindEntryHolder_t>;

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

		class ProfileManagerModel :
			public ProfileManager<ModelProfile>
		{
		public:
			FN_NAMEPROC("ModelDataManager");

		private:
			using ProfileManager<ModelProfile>::ProfileManager;
		};

		class ProfileManagerCondVar :
			public ProfileManager<CondVarProfile>
		{
		public:
			FN_NAMEPROC("ProfileManagerVariables");

		private:
			using ProfileManager<CondVarProfile>::ProfileManager;
		};

		class ProfileManagerOutfit :
			public ProfileManager<OutfitProfile>
		{
		public:
			FN_NAMEPROC("ProfileManagerOutfit");

		private:
			using ProfileManager<OutfitProfile>::ProfileManager;
		};

		class ProfileManagerOutfitFormList :
			public ProfileManager<OutfitFormListProfile>
		{
		public:
			FN_NAMEPROC("ProfileManagerOutfitFormList");

		private:
			using ProfileManager<OutfitFormListProfile>::ProfileManager;
		};

		class ProfileManagerKeybind :
			public ProfileManager<KeybindProfile>
		{
		public:
			FN_NAMEPROC("ProfileManagerKeybind");

		private:
			using ProfileManager<KeybindProfile>::ProfileManager;
		};
		
	public:
		template <class T>
		[[nodiscard]] static constexpr auto& GetSingleton() noexcept
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
			else if constexpr (std::is_same_v<T, ModelProfile>)
			{
				return m_modelManager;
			}
			else if constexpr (std::is_same_v<T, CondVarProfile>)
			{
				return m_condVarManager;
			}
			else if constexpr (std::is_same_v<T, OutfitProfile>)
			{
				return m_outfitManager;
			}
			else if constexpr (std::is_same_v<T, OutfitFormListProfile>)
			{
				return m_outfitFormListManager;
			}
			else if constexpr (std::is_same_v<T, KeybindProfile>)
			{
				return m_keybindManager;
			}
			else
			{
				HALT("Unrecognized profile");
			}
		}

	private:
		static ProfileManagerSlot           m_slotManager;
		static ProfileManagerCustom         m_customManager;
		static ProfileManagerNodeOverride   m_nodeOverrideManager;
		static ProfileManagerFormFilter     m_formFilterManager;
		static ProfileManagerModel          m_modelManager;
		static ProfileManagerCondVar        m_condVarManager;
		static ProfileManagerOutfit         m_outfitManager;
		static ProfileManagerOutfitFormList m_outfitFormListManager;
		static ProfileManagerKeybind        m_keybindManager;
	};

}