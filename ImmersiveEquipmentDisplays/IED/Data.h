#pragma once

#include "Config.h"
#include "ConfigCommon.h"
#include "ConfigOverride.h"

#include <ext/IPluginInfo.h>
#include <ext/Threads.h>

namespace IED
{
	namespace Data
	{
		class ItemData
		{
		public:
			static ObjectSlot GetObjectSlot(TESObjectWEAP* a_form) noexcept;
			static ObjectSlot GetObjectSlotLeft(TESObjectWEAP* a_form) noexcept;
			static ObjectSlot GetObjectSlotLeft(TESObjectLIGH* a_form) noexcept;
			static ObjectSlot GetObjectSlot(TESObjectARMO* a_form) noexcept;
			static ObjectSlot GetObjectSlot(TESAmmo* a_form) noexcept;
			static ObjectSlot GetObjectSlot(TESForm* a_form) noexcept;
			static ObjectSlot GetObjectSlotLeft(TESForm* a_form) noexcept;

			static ObjectType GetItemType(TESObjectARMO* a_form) noexcept;
			static ObjectType GetItemType(TESObjectWEAP* a_form) noexcept;
			static ObjectType GetItemType(TESObjectLIGH* a_form) noexcept;
			static ObjectType GetItemType(TESAmmo* a_form) noexcept;
			static ObjectType GetItemType(TESForm* a_form) noexcept;

			static ObjectTypeExtra GetItemTypeExtra(TESObjectARMO* a_form) noexcept;
			static ObjectTypeExtra GetItemTypeExtra(TESObjectWEAP* a_form) noexcept;
			static ObjectTypeExtra GetItemTypeExtra(TESObjectLIGH* a_form) noexcept;
			static ObjectTypeExtra GetItemTypeExtra(TESForm* a_form) noexcept;

			static ObjectSlotExtra GetItemSlotExtra(TESObjectARMO* a_form) noexcept;
			static ObjectSlotExtra GetItemSlotExtra(TESObjectWEAP* a_form) noexcept;
			static ObjectSlotExtra GetItemSlotExtra(TESObjectLIGH* a_form) noexcept;
			static ObjectSlotExtra GetItemSlotExtra(TESForm* a_form) noexcept;

			static ObjectSlotExtra GetItemSlotLeftExtra(TESObjectWEAP* a_form) noexcept;
			static ObjectSlotExtra GetItemSlotLeftExtra(TESForm* a_form) noexcept;

			// static ObjectType GetTypeFromSlot(ObjectSlot a_slot);
			static ObjectSlot      GetSlotFromType(ObjectType a_type) noexcept;
			static ObjectSlotExtra GetSlotFromTypeExtra(ObjectTypeExtra a_type) noexcept;
			static ObjectTypeExtra GetTypeFromSlotExtra(ObjectSlotExtra a_type) noexcept;

			static ObjectSlot      GetLeftSlot(ObjectSlot a_slot) noexcept;
			static ObjectSlotExtra GetLeftSlotExtra(ObjectSlotExtra a_slot) noexcept;

			static ObjectSlot      ExtraSlotToSlot(ObjectSlotExtra a_slot) noexcept;
			static ObjectSlotExtra SlotToExtraSlot(ObjectSlot a_slot) noexcept;

			static bool IsLeftWeaponSlot(ObjectSlot a_slot) noexcept;
			static bool IsLeftHandExtraSlot(ObjectSlotExtra a_slot) noexcept;

			static NodeDescriptor GetDefaultSlotNode(ObjectSlot a_slot) noexcept;

			// static Biped::BIPED_OBJECT GetBipedObject(ObjectSlot a_slot);

			static TESRace::EquipmentFlag GetRaceEquipmentFlagFromType(ObjectType a_type) noexcept;

			static Biped::BIPED_OBJECT SlotToBipedObject(ObjectSlot a_slot) noexcept;

		private:
		};

		struct raceCacheEntry_t
		{
			std::string              fullname;
			std::string              edid;
			stl::flag<TESRace::Flag> flags;
		};

		class IData
		{
		public:
			[[nodiscard]] static bool PopulateRaceList();
			[[nodiscard]] static bool PopulatePluginInfo();
			[[nodiscard]] static bool PopulateMiscInfo();

			[[nodiscard]] inline constexpr static const auto& GetRaceList() noexcept
			{
				return m_Instance.m_raceList;
			}

			[[nodiscard]] inline constexpr static const auto& GetPluginInfo() noexcept
			{
				return m_Instance.m_pluginInfo;
			}

			[[nodiscard]] inline constexpr static const auto& GetPlayerRefID() noexcept
			{
				return m_Instance.m_playerRefID;
			}

			[[nodiscard]] inline constexpr static const auto& GetPlayerBaseID() noexcept
			{
				return m_Instance.m_playerBaseID;
			}

		private:
			std::unordered_map<Game::FormID, raceCacheEntry_t> m_raceList;
			IPluginInfo                                        m_pluginInfo;

			Game::FormID m_playerRefID{ 0x14 };
			Game::FormID m_playerBaseID{ 0x7 };

			static IData m_Instance;
		};

		const char* GetSlotName(ObjectSlot a_slot);
		const char* GetSlotName(ObjectSlotExtra a_slot);
		const char* GetObjectTypeName(ObjectTypeExtra a_type);
		const char* GetConfigClassName(ConfigClass a_class);

		std::unique_ptr<configSlotHolder_t::data_type>
			CreateDefaultSlotConfig(ObjectSlot a_slot);

		std::unique_ptr<configSlotHolderCopy_t::data_type>
			CreateDefaultSlotConfig(ObjectSlot a_slot, ConfigClass a_class);
	}
}