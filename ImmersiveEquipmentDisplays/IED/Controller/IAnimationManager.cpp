#include "pch.h"

#include "IAnimationManager.h"

#include "ActorAnimationState.h"

#include "IED/Util/PEXReader.h"

#include <ext/StrHelpers.h>

namespace IED
{

	inline static constexpr bool is_shield(TESForm* a_form) noexcept
	{
		return a_form && a_form->IsArmor() && static_cast<TESObjectARMO*>(a_form)->IsShield();
	}

	void IAnimationManager::InitializeAnimationStrings()
	{
		if (!m_strings)
		{
			m_strings = std::make_unique<const AnimStringHolder>();
		}
	}

	void IAnimationManager::ExtractAnimationInfoFromPEX(AnimationGroupInfo& a_out)
	{
		PEXReader reader;

		reader.Open(FNIS_AA2_PEX_PATH);
		reader.ReadData();

		auto& st = reader.GetStringTable();
		if (st.size() != 224)
		{
			throw std::exception("unexpected string table size");
		}

		// validate the ranges first

		auto modIdBegin = st.begin() + 33;
		auto modIdEnd   = st.begin() + 63;

		for (auto it = modIdBegin; it != modIdEnd; ++it)
		{
			if (it->size() != 3)
			{
				throw std::exception("unexpected mod prefix string length");
			}
		}

		auto setsBegin = st.begin() + 89;
		auto setsEnd   = st.begin() + 217;

		for (auto it = setsBegin; it != setsEnd; ++it)
		{
			if (it->size() != 6)
			{
				throw std::exception("unexpected aa set string length");
			}
		}

		std::int32_t numMods = std::stol(st[72]);

		if (!numMods)
		{
			throw std::exception("no animation mods found");
		}

		if (numMods < 1 || numMods > 30)
		{
			throw std::exception("numMods out of range");
		}

		modIdEnd = modIdBegin + numMods;

		std::int32_t numSets = std::stol(st[73]);

		if (!numSets)
		{
			throw std::exception("no aa sets found");
		}

		if (numSets < 0 || numSets > 128)
		{
			throw std::exception("numSets out of range");
		}

		if (st[74].size() != 6)
		{
			throw std::exception("unexpected crc string length");
		}

		std::int32_t crc = std::stol(st[74]);

		// find the mod id
		std::int32_t modId = -1;
		std::int32_t i     = 0;

		std::string modPrefix("xpe");

		for (auto it = modIdBegin; it != modIdEnd; ++it)
		{
			if (StrHelpers::iequal(modPrefix, *it))
			{
				modId = i;
				break;
			}

			i++;
		}

		if (modId < 0)
		{
			throw std::exception("xpe mod prefix not found");
		}

		std::int32_t groupBaseSword  = -1;
		std::int32_t groupBaseAxe    = -1;
		std::int32_t groupBaseDagger = -1;
		std::int32_t groupBaseMace   = -1;

		std::uint8_t presenceFlags = 0;

		setsEnd = setsBegin + numSets;

		for (auto it = setsBegin; it != setsEnd; ++it)
		{
			std::int32_t data = std::stol(*it);

			auto prefix = data / 10000;
			if (prefix == modId)
			{
				auto c     = data - prefix * 10000;
				auto group = c / 100;
				auto base  = c - group * 100;

				//_DMESSAGE("%d", group);

				switch (group)
				{
				case 37:
					groupBaseSword = base;
					presenceFlags |= 0x1;
					break;
				case 40:
					groupBaseAxe = base;
					presenceFlags |= 0x2;
					break;
				case 43:
					groupBaseDagger = base;
					presenceFlags |= 0x4;
					break;
				case 45:
					groupBaseMace = base;
					presenceFlags |= 0x8;
					break;
				}

				if ((presenceFlags & 0xF) == 0xF)
				{
					break;
				}
			}
		}

		if (groupBaseSword < 0 ||
		    groupBaseAxe < 0 ||
		    groupBaseDagger < 0 ||
		    groupBaseMace < 0)
		{
			throw std::exception("one or more group base values could not be determined");
		}

		a_out.crc = crc;
		a_out.set_base(AnimationWeaponType::Sword, groupBaseSword);
		a_out.set_base(AnimationWeaponType::Axe, groupBaseAxe);
		a_out.set_base(AnimationWeaponType::Dagger, groupBaseDagger);
		a_out.set_base(AnimationWeaponType::Mace, groupBaseMace);
	}

	void IAnimationManager::SetAnimationInfo(const AnimationGroupInfo& a_in)
	{
		m_groupInfo = a_in;
	}

	void IAnimationManager::UpdateAA(
		Actor*               a_actor,
		ActorAnimationState& a_state)
	{
		if (!m_groupInfo)
		{
			return;
		}

		auto pm = a_actor->processManager;
		if (!pm)
		{
			return;
		}

		a_state.flags.clear(ActorAnimationState::Flags::kNeedUpdate);

		auto type = GetObjectType(pm->equippedObject[ActorProcessManager::kEquippedHand_Right]);

		switch (type)
		{
		case AnimationWeaponType::Sword:
			{
				auto objLeft = pm->equippedObject[ActorProcessManager::kEquippedHand_Left];
				auto leftID  = GetObjectType(objLeft);

				std::int32_t animVar;

				switch (a_state.get_placement(AnimationWeaponSlot::Sword))
				{
				case WeaponPlacementID::OnBack:

					if ((leftID == AnimationWeaponType::Sword &&
					     a_state.get_placement(AnimationWeaponSlot::SwordLeft) == WeaponPlacementID::OnBack) ||
					    ((leftID == AnimationWeaponType::Dagger &&
					      a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::OnBack) ||
					     (leftID == AnimationWeaponType::Axe &&
					      a_state.get_placement(AnimationWeaponSlot::AxeLeft) == WeaponPlacementID::OnBack)) ||
					    is_shield(objLeft))
					{
						animVar = 2;
					}
					else
					{
						animVar = 1;
					}
					break;

				default:

					if (leftID == AnimationWeaponType::Sword &&
					    a_state.get_placement(AnimationWeaponSlot::SwordLeft) == WeaponPlacementID::Default)
					{
						animVar = 0;
					}
					else if (is_shield(objLeft))
					{
						animVar = 3;
					}
					else
					{
						animVar = -1;
					}

					break;
				}

				SetAnimationVar(
					a_actor,
					a_state,
					type,
					animVar);
			}
			break;
		case AnimationWeaponType::Dagger:

			{
				auto leftID = GetObjectType(pm->equippedObject[ActorProcessManager::kEquippedHand_Left]);

				std::int32_t animVar;

				switch (a_state.get_placement(AnimationWeaponSlot::Dagger))
				{
				case WeaponPlacementID::Ankle:

					animVar = leftID == AnimationWeaponType::Dagger &&
					                  a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::Ankle ?
					              4 :
                                  3;

					break;

				case WeaponPlacementID::OnBackHip:

					animVar = leftID == AnimationWeaponType::Dagger &&
					                  a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::OnBackHip ?
					              2 :
                                  1;

					break;

				default:

					animVar = leftID == AnimationWeaponType::Dagger &&
					                  a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::Default ?
					              0 :
                                  -1;

					break;
				}

				SetAnimationVar(
					a_actor,
					a_state,
					type,
					animVar);
			}

			break;
		case AnimationWeaponType::Axe:

			{
				auto objLeft = pm->equippedObject[ActorProcessManager::kEquippedHand_Left];
				auto leftID  = GetObjectType(objLeft);

				std::int32_t animVar;

				switch (a_state.get_placement(AnimationWeaponSlot::Axe))
				{
				case WeaponPlacementID::OnBack:

					if ((leftID == AnimationWeaponType::Axe &&
					     a_state.get_placement(AnimationWeaponSlot::AxeLeft) == WeaponPlacementID::OnBack) ||
					    ((leftID == AnimationWeaponType::Dagger &&
					      a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::OnBack) ||
					     (leftID == AnimationWeaponType::Sword &&
					      a_state.get_placement(AnimationWeaponSlot::SwordLeft) == WeaponPlacementID::OnBack)) ||
					    is_shield(objLeft))
					{
						animVar = 2;
					}
					else
					{
						animVar = 1;
					}
					break;

				default:

					if (leftID == AnimationWeaponType::Axe &&
					    a_state.get_placement(AnimationWeaponSlot::AxeLeft) == WeaponPlacementID::Default)
					{
						animVar = 0;
					}
					else if (is_shield(objLeft))
					{
						animVar = 3;
					}
					else
					{
						animVar = -1;
					}

					break;
				}

				SetAnimationVar(
					a_actor,
					a_state,
					type,
					animVar);
			}
			break;
		case AnimationWeaponType::Mace:

			{
				auto leftID = GetObjectType(pm->equippedObject[ActorProcessManager::kEquippedHand_Left]);

				std::int32_t animVar =
					leftID == AnimationWeaponType::Mace &&
							a_state.get_placement(AnimationWeaponSlot::MaceLeft) == WeaponPlacementID::Default ?
						0 :
                        -1;

				SetAnimationVar(
					a_actor,
					a_state,
					type,
					animVar);
			}

			break;
		}
	}

	void IAnimationManager::ResetAA(
		Actor*               a_actor,
		ActorAnimationState& a_state)
	{
		using enum_type = std::underlying_type_t<AnimationWeaponType>;

		for (enum_type i = 0; i < stl::underlying(AnimationWeaponType::Max); i++)
		{
			auto id = static_cast<AnimationWeaponType>(i);

			SetAnimationVar(a_actor, a_state, id, -1);
			a_state.get(id).animVar = -1;
		}
	}

	void IAnimationManager::SetAnimationVar(
		Actor*               a_actor,
		ActorAnimationState& a_state,
		AnimationWeaponType  a_id,
		std::int32_t         a_value)
	{
		auto& entry = a_state.get(a_id);

		auto value = m_groupInfo->get_value(a_id, a_value);

		if (entry.animVar != value)
		{
			entry.animVar = value;

			auto& se = m_strings->get_eqp(a_id);

			a_actor->animGraphHolder.SetVariableOnGraphsInt(se.name, value);

			// these can probably be set once after actor load
			a_actor->animGraphHolder.SetVariableOnGraphsInt(m_strings->FNISaa_crc, m_groupInfo->crc);
			a_actor->animGraphHolder.SetVariableOnGraphsInt(se.crc, m_groupInfo->crc);

			//gLog.Debug("%.8X: %s: %d", a_actor->formID, se.name.c_str(), value);
		}
	}

	AnimationWeaponType IAnimationManager::GetObjectType(TESForm* a_object)
	{
		if (!a_object)
		{
			return AnimationWeaponType::None;
		}

		auto weap = a_object->As<TESObjectWEAP>();
		if (!weap)
		{
			return AnimationWeaponType::None;
		}

		switch (weap->type())
		{
		case TESObjectWEAP::GameData::kType_OneHandSword:
			return AnimationWeaponType::Sword;
		case TESObjectWEAP::GameData::kType_OneHandDagger:
			return AnimationWeaponType::Dagger;
		case TESObjectWEAP::GameData::kType_OneHandAxe:
			return AnimationWeaponType::Axe;
		case TESObjectWEAP::GameData::kType_OneHandMace:
			return AnimationWeaponType::Mace;
		default:
			return AnimationWeaponType::None;
		}
	}

	IAnimationManager::AnimStringHolder::AnimStringHolder() :
		eqp{
			{ "FNISaa_1hmeqp", "FNISaa_1hmeqp_crc" },
			{ "FNISaa_axeeqp", "FNISaa_axeeqp_crc" },
			{ "FNISaa_dageqp", "FNISaa_dageqp_crc" },
			{ "FNISaa_maceqp", "FNISaa_maceqp_crc" },
		}
	{
	}

}