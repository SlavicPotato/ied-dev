#include "pch.h"

#include "IAnimationManager.h"

#include "IED/SettingHolder.h"
#include "IED/Util/PEXReader.h"

namespace IED
{
	IAnimationManager::IAnimationManager(
		Data::SettingHolder& a_settings) :
		m_settings(a_settings)
	{
	}

	void IAnimationManager::InitializeAnimationStrings()
	{
		if (!m_strings)
		{
			m_strings = std::make_unique<AnimStringHolder>();
		}
	}

	AnimationGroupInfo IAnimationManager::ExtractAnimationInfoFromPEX()
	{
		AnimationGroupInfo result;

		PEXReader reader;

		reader.Open(FNIS_AA2_PEX_PATH);
		reader.ReadData();

		// validate data first

		auto& st = reader.GetStringTable();
		if (st.size() != 224)
		{
			throw std::length_error("unexpected string table size");
		}

		auto modIdBegin = st.begin() + 33;
		auto modIdEnd   = st.begin() + 63;

		for (auto it = modIdBegin; it != modIdEnd; ++it)
		{
			if (it->size() != 3)
			{
				throw std::length_error("mod prefix strlen != 3");
			}
		}

		auto setsBegin = st.begin() + 89;
		auto setsEnd   = st.begin() + 217;

		for (auto it = setsBegin; it != setsEnd; ++it)
		{
			if (it->size() != 6)
			{
				throw std::length_error("aa set strlen != 6");
			}
		}

		std::int32_t numMods = std::stoi(st[72]);

		if (!numMods)
		{
			throw std::length_error("numMods == 0");
		}

		if (numMods < 0 || numMods > 30)
		{
			throw std::out_of_range("numMods out of range");
		}

		modIdEnd = modIdBegin + numMods;

		std::int32_t numSets = std::stoi(st[73]);

		if (!numSets)
		{
			throw std::length_error("numSets == 0");
		}

		if (numSets < 0 || numSets > 128)
		{
			throw std::out_of_range("numSets out of range");
		}

		if (st[74].size() != 6)
		{
			throw std::length_error("unexpected crc string length");
		}

		// attempt to extract the info

		result.crc = std::stoi(st[74]);

		std::int32_t modId = -1;
		std::int32_t i     = 0;

		// find the mod id
		for (auto it = modIdBegin; it != modIdEnd; ++it, i++)
		{
			if (_stricmp(it->data(), "xpe") == 0)
			{
				modId = i;
				break;
			}
		}

		if (modId < 0)
		{
			throw std::runtime_error("mod prefix not found (xpe)");
		}

		setsEnd = setsBegin + numSets;

		stl::flag<PresenceFlags> presenceFlags{ PresenceFlags::kNone };

		// find ag base values
		for (auto it = setsBegin; it != setsEnd; ++it)
		{
			const auto data = std::stoi(*it);

			const auto prefix = data / 10000;

			if (prefix == modId)
			{
				const auto c     = data - prefix * 10000;
				const auto group = c / 100;
				const auto base  = c - group * 100;

				switch (group)
				{
				case 4:
					result.set_base_extra(AnimationExtraGroup::BowIdle, base);
					break;
				case 31:
					result.set_base_extra(AnimationExtraGroup::BowAttack, base);
					break;
				case 37:
					result.set_base(AnimationWeaponType::Sword, base);
					presenceFlags.set(PresenceFlags::kSword);
					break;
				case 38:
					result.set_base(AnimationWeaponType::TwoHandedAxe, base);
					presenceFlags.set(PresenceFlags::k2hSword);
					break;
				case 39:
					result.set_base(AnimationWeaponType::TwoHandedSword, base);
					presenceFlags.set(PresenceFlags::k2hAxe);
					break;
				case 40:
					result.set_base(AnimationWeaponType::Axe, base);
					presenceFlags.set(PresenceFlags::kAxe);
					break;
				case 41:
					result.set_base(AnimationWeaponType::Bow, base);
					presenceFlags.set(PresenceFlags::kBow);
					break;
				case 43:
					result.set_base(AnimationWeaponType::Dagger, base);
					presenceFlags.set(PresenceFlags::kDagger);
					break;
				case 45:
					result.set_base(AnimationWeaponType::Mace, base);
					presenceFlags.set(PresenceFlags::kMace);
					break;
				}

				if (presenceFlags.test(PresenceFlags::kAll))
				{
					break;
				}
			}
		}

		if (!presenceFlags.test(PresenceFlags::kAll))
		{
			throw std::runtime_error("one or more ag base values not found");
		}

		return result;
	}

	inline static constexpr bool is_shield(TESForm* a_form) noexcept
	{
		return a_form && a_form->IsArmor() && static_cast<TESObjectARMO*>(a_form)->IsShield();
	}

	inline static constexpr bool shield_on_back_enabled(
		Actor*                                     a_actor,
		PluginInterfaceHolder<PluginInterfaceSDS>* a_holder) noexcept
	{
		PluginInterfaceSDS* intfc;
		a_holder->GetPluginInterface(intfc);

		if (!intfc)
		{
			return true;
		}

		return intfc->GetShieldOnBackEnabled(a_actor);
	}

	bool IAnimationManager::should_select_back_left_anim(
		AnimationWeaponType  a_leftID,
		ActorAnimationState& a_state,
		TESForm*             a_objLeft,
		Actor*               a_actor) noexcept
	{
		return (a_leftID == AnimationWeaponType::Sword &&
		        a_state.get_placement(AnimationWeaponSlot::SwordLeft) == WeaponPlacementID::OnBack) ||
		       (a_leftID == AnimationWeaponType::Dagger &&
		        a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::OnBack) ||
		       (a_leftID == AnimationWeaponType::Axe &&
		        a_state.get_placement(AnimationWeaponSlot::AxeLeft) == WeaponPlacementID::OnBack) ||
		       (is_shield(a_objLeft) && shield_on_back_enabled(a_actor, this));
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

		auto type = GetObjectType(pm->equippedObject[ActorProcessManager::kEquippedHand_Right]);

		std::int32_t animVar;

		switch (type)
		{
		case AnimationWeaponType::Sword:
			{
				auto objLeft = pm->equippedObject[ActorProcessManager::kEquippedHand_Left];
				auto leftID  = GetObjectType(objLeft);

				switch (a_state.get_placement(AnimationWeaponSlot::Sword))
				{
				case WeaponPlacementID::OnBack:

					if (should_select_back_left_anim(leftID, a_state, objLeft, a_actor))
					{
						animVar = 2;
					}
					else if (
						leftID == AnimationWeaponType::Dagger &&
						a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::OnBackHip)
					{
						animVar = 4;
					}
					else
					{
						animVar = 1;
					}

					break;

				default:

					if (leftID == AnimationWeaponType::Sword)
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
				auto objLeft = pm->equippedObject[ActorProcessManager::kEquippedHand_Left];
				auto leftID  = GetObjectType(objLeft);

				switch (a_state.get_placement(AnimationWeaponSlot::Dagger))
				{
				case WeaponPlacementID::OnBackHip:

					if (leftID == AnimationWeaponType::Dagger &&
					    a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::OnBackHip)
					{
						animVar = 2;
					}
					else if (should_select_back_left_anim(leftID, a_state, objLeft, a_actor))
					{
						animVar = 5;
					}
					else
					{
						animVar = 1;
					}

					break;

				case WeaponPlacementID::OnBack:  // no anim, use back hip

					if ((leftID == AnimationWeaponType::Sword &&
					     a_state.get_placement(AnimationWeaponSlot::SwordLeft) == WeaponPlacementID::OnBack) ||
					    (leftID == AnimationWeaponType::Dagger &&
					     (a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::OnBackHip ||
					      a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::OnBack)) ||
					    (leftID == AnimationWeaponType::Axe &&
					     a_state.get_placement(AnimationWeaponSlot::AxeLeft) == WeaponPlacementID::OnBack) ||
					    (is_shield(objLeft) && shield_on_back_enabled(a_actor, this)))
					{
						animVar = 2;
					}
					else
					{
						animVar = 1;
					}

					break;

				case WeaponPlacementID::Ankle:

					animVar = leftID == AnimationWeaponType::Dagger &&
					                  a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::Ankle ?
					              4 :
                                  3;

					break;

				default:

					animVar = leftID == AnimationWeaponType::Dagger ?
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

				switch (a_state.get_placement(AnimationWeaponSlot::Axe))
				{
				case WeaponPlacementID::OnBack:

					if (should_select_back_left_anim(leftID, a_state, objLeft, a_actor))
					{
						animVar = 2;
					}
					else if (
						leftID == AnimationWeaponType::Dagger &&
						a_state.get_placement(AnimationWeaponSlot::DaggerLeft) == WeaponPlacementID::OnBackHip)
					{
						animVar = 4;
					}
					else
					{
						animVar = 1;
					}

					break;

				default:

					if (leftID == AnimationWeaponType::Axe)
					{
						animVar = 0;
					}
					else if (is_shield(objLeft) && shield_on_back_enabled(a_actor, this))
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
				auto objLeft = pm->equippedObject[ActorProcessManager::kEquippedHand_Left];
				auto leftID  = GetObjectType(objLeft);

				if (leftID == AnimationWeaponType::Mace)
				{
					animVar = 0;
				}
				else if (is_shield(objLeft) && shield_on_back_enabled(a_actor, this))
				{
					animVar = 1;
				}
				else
				{
					animVar = -1;
				}

				SetAnimationVar(
					a_actor,
					a_state,
					type,
					animVar);
			}

			break;

		case AnimationWeaponType::TwoHandedSword:
		case AnimationWeaponType::TwoHandedAxe:

			animVar =
				a_state.get_placement(AnimationWeaponSlot::TwoHanded) == WeaponPlacementID::AtHip ?
					0 :
                    -1;

			SetAnimationVar(
				a_actor,
				a_state,
				type,
				animVar);

			break;

		case AnimationWeaponType::Bow:

			animVar =
				a_state.get_placement(AnimationWeaponSlot::Quiver) == WeaponPlacementID::Frostfall ?
					0 :
                    -1;

			SetAnimationVar(
				a_actor,
				a_state,
				type,
				animVar);

			break;
		}
	}

	void IAnimationManager::ResetAA(
		Actor*               a_actor,
		ActorAnimationState& a_state)
	{
		if (!m_groupInfo)
		{
			return;
		}

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

		const auto value = m_groupInfo->get_value(a_id, a_value);

		if (entry.animVar != value)
		{
			entry.animVar = value;

			auto& se = m_strings->get_eqp(a_id);

			a_actor->SetVariableOnGraphsInt(se.name, value);

			a_actor->SetVariableOnGraphsInt(m_strings->FNISaa_crc, m_groupInfo->crc);
			a_actor->SetVariableOnGraphsInt(se.crc, m_groupInfo->crc);

			if (a_id == AnimationWeaponType::Bow)
			{
				std::int32_t av;

				av = m_groupInfo->get_value_extra(AnimationExtraGroup::BowIdle, m_settings.data.XP32AABowIdle ? a_value : -1);

				a_actor->SetVariableOnGraphsInt(m_strings->FNISaa_bowidle, av);
				a_actor->SetVariableOnGraphsInt(m_strings->FNISaa_bowidle_crc, m_groupInfo->crc);

				av = m_groupInfo->get_value_extra(AnimationExtraGroup::BowAttack, m_settings.data.XP32AABowAtk ? a_value : -1);

				a_actor->SetVariableOnGraphsInt(m_strings->FNISaa_bowatk, av);
				a_actor->SetVariableOnGraphsInt(m_strings->FNISaa_bowatk_crc, m_groupInfo->crc);
			}

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
		case WEAPON_TYPE::kOneHandSword:
			return AnimationWeaponType::Sword;
		case WEAPON_TYPE::kOneHandDagger:
			return AnimationWeaponType::Dagger;
		case WEAPON_TYPE::kOneHandAxe:
			return AnimationWeaponType::Axe;
		case WEAPON_TYPE::kOneHandMace:
			return AnimationWeaponType::Mace;
		case WEAPON_TYPE::kTwoHandSword:
			return AnimationWeaponType::TwoHandedSword;
		case WEAPON_TYPE::kTwoHandAxe:
			return AnimationWeaponType::TwoHandedAxe;
		case WEAPON_TYPE::kBow:
			return AnimationWeaponType::Bow;
		default:
			return AnimationWeaponType::None;
		}
	}

	IAnimationManager::AnimStringHolder::AnimStringHolder() :
		eqp{ {

			{ "FNISaa_1hmeqp", "FNISaa_1hmeqp_crc" },
			{ "FNISaa_axeeqp", "FNISaa_axeeqp_crc" },
			{ "FNISaa_dageqp", "FNISaa_dageqp_crc" },
			{ "FNISaa_maceqp", "FNISaa_maceqp_crc" },
			{ "FNISaa_2hmeqp", "FNISaa_2hmeqp_crc" },
			{ "FNISaa_2hweqp", "FNISaa_2hweqp_crc" },
			{ "FNISaa_boweqp", "FNISaa_boweqp_crc" }

		} }
	{
	}

}