#include "pch.h"

#include "NodeMap.h"

#include "Parsers/JSONNodeMapParser.h"

namespace IED
{
	namespace Data
	{
		NodeMap NodeMap::m_Instance;

		static constexpr NodeMap::initializer_type s_init[] = {

			{ "WeaponBack", "Two-Handed", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponBackLeft", "Two-Handed Left", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kWeapon },
			{ "WeaponBackIED", "Two-Handed IED", NodeDescriptorFlags::kWeapon | NodeDescriptorFlags::kDefaultCopy },
			{ "WeaponBackSWP", "Two-Handed SWP", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponBackFSM", "Two-Handed FSM", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponBackAxeMace", "Two-Handed Axe/Mace", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponBackAxeMaceLeft", "Two-Handed Axe/Mace Left", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kWeapon },
			{ "WeaponBackAxeMaceSWP", "Two-Handed Axe/Mace SWP", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponBackAxeMaceFSM", "Two-Handed Axe/Mace FSM", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponAxe", "One-Handed Axe", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponAxeLeft", "One-Handed Axe Left", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponAxeReverse", "One-Handed Axe Reverse", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponAxeLeftReverse", "One-Handed Axe Left Reverse", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponAxeOnBack", "One-Handed Axe on Back", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponAxeLeftOnBack", "One-Handed Axe on Back Left", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponMace", "Mace", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponMaceLeft", "Mace Left", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponDagger", "Dagger", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponDaggerLeft", "Dagger Left", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponDaggerAnkle", "Dagger Ankle", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponDaggerLeftAnkle", "Dagger Ankle Left", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponDaggerBackHip", "Dagger Back Hip", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponDaggerLeftBackHip", "Dagger Back Hip Left", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponStaff", "Staff", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponStaffLeft", "Staff Left", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponBow", "Bow", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponBowIED", "Bow IED", NodeDescriptorFlags::kWeapon | NodeDescriptorFlags::kDefaultCopy },
			{ "WeaponBowFSM", "Bow FSM", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponBowBetter", "Better Bow", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponBowChesko", "Chesko Bow", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponCrossBow", "Crossbow", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponCrossBowChesko", "Chesko Crossbow", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponSword", "One-Handed Sword", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponSwordLeft", "One-Handed Sword Left", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
			{ "WeaponSwordOnBack", "One-Handed Sword on Back", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponSwordLeftOnBack", "One-Handed Sword on Back Left", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponSwordSWP", "One-Handed Sword SWP", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponSwordLeftSWP", "One-Handed Sword SWP Left", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponSwordFSM", "One-Handed Sword FSM", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponSwordLeftFSM", "One-Handed Sword FSM Left", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponSwordLeftHip", "One-Handed Sword on Left Hip", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponSwordLeftLeftHip", "One-Handed Sword on Left Hip Left", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponSwordNMD", "One-Handed Sword NMD", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "WeaponSwordLeftNMD", "One-Handed Sword NMD Left", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
			{ "Weapon", "Right Weapon", NodeDescriptorFlags::kManaged },
			{ "Shield", "Left Weapon / Shield", NodeDescriptorFlags::kManaged },
			{ "ShieldBack", "Shield Back", NodeDescriptorFlags::kXP32 },
			{ "Quiver", "Quiver", NodeDescriptorFlags::kManaged },
			{ "QUIVERChesko", "Quiver Chesko", NodeDescriptorFlags::kXP32 },
			{ "BOLTDefault", "Bolt", NodeDescriptorFlags::kXP32 },
			{ "QUIVERLeftHipBolt", "Quiver Left-Hip Bolt", NodeDescriptorFlags::kXP32 },
			{ "BOLTChesko", "Bolt Chesko", NodeDescriptorFlags::kXP32 },
			{ "BOLTLeftHipBolt", "Bolt Left-Hip", NodeDescriptorFlags::kXP32 },
			{ "BOLTABQ", "Bolt ABQ", NodeDescriptorFlags::kXP32 },
			{ "NPC Spine [Spn0]", "Spine Lower" },
			{ "NPC Spine1 [Spn1]", "Spine Middle" },
			{ "NPC Spine2 [Spn2]", "Spine Upper" },
			{ "NPC Pelvis [Pelv]", "Pelvis" },
			{ "NPC Head [Head]", "Head" },
			{ "NPC Root [Root]", "NPC Root" },
			{ "NPC L Thigh [LThg]", "Thigh Left" },
			{ "NPC R Thigh [RThg]", "Thigh Right" },
			{ "NPC L Calf [LClf]", "Calf Left" },
			{ "NPC R Calf [RClf]", "Calf Right" },
			{ "NPC L Foot [Lft ]", "Foot Left" },
			{ "NPC R Foot [Rft ]", "Foot Right" },
			{ "NPC L Clavicle [LClv]", "Clavicle Left" },
			{ "NPC R Clavicle [RClv]", "Clavicle Right" },
			{ "NPC L Pauldron", "Pauldron Left" },
			{ "NPC R Pauldron", "Pauldron Right" },
			{ "NPC L Forearm [LLar]", "Forearm Left" },
			{ "NPC R Forearm [RLar]", "Forearm Right" },
			{ "NPC L Hand [LHnd]", "Hand Left" },
			{ "NPC R Hand [RHnd]", "Hand Right" },
			{ "NPC R UpperArm [RUar]", "Upper Arm Right" },
			{ "NPC L UpperArm [LUar]", "Upper Left Right" },
			{ "NPC TailBone01", "Tail 1" },
			{ "NPC TailBone02", "Tail 2" },
			{ "NPC TailBone03", "Tail 3" },
			{ "NPC TailBone04", "Tail 4" },
			{ "NPC TailBone05", "Tail 5" },
			{ "NPC TailBone06", "Tail 6" },
			{ "NPC TailBone07", "Tail 7" },
			{ "NPC TailBone08", "Tail 8" },
			{ "RightWing1", "Wing 1 Right" },
			{ "LeftWing1", "Wing 1 Left" },
			{ "RightWing2", "Wing 2 Right" },
			{ "LeftWing2", "Wing 2 Left" },
			{ "RightWing3", "Wing 3 Right" },
			{ "LeftWing3", "Wing 3 Left" },
			{ "RightWing4", "Wing 4 Right" },
			{ "LeftWing4", "Wing 4 Left" },
			{ "RightWing5", "Wing 5 Right" },
			{ "LeftWing5", "Wing 5 Left" },
			{ "NPCEyeBone", "Eyes" },
			{ "HorseSpine1", "Horse Spine Lower" },
			{ "HorseSpine2", "Horse Spine Middle" },
			{ "HorseSpine3", "Horse Spine Upper" },
			{ "HorseSpine4", "Horse Spine Top" },
			{ "HorseNeck1", "Horse Neck Lower" },
			{ "HorseNeck2", "Horse Neck Middle 1" },
			{ "HorseNeck3", "Horse Neck Middle 2" },
			{ "HorseNeck4", "Horse Neck Upper" },
			{ "HorsePelvis", "Horse Pelvis" },
			{ "HorseScull", "Horse Scull" },
			{ "HorseTail1", "Horse Tail" },
			{ "HorseLFemur", "Horse Femur Left" },
			{ "HorseRFemur", "Horse Femur Right" },
			{ "HorseLTibia", "Horse Tibia Left" },
			{ "HorseRTibia", "Horse Tibia Right" },
			{ "HorseLScapula", "Horse Scapula Left" },
			{ "HorseRScapula", "Horse Scapula Right" },
			{ "HorseLHumerus", "Horse Humerus Left" },
			{ "HorseRHumerus", "Horse Humerus Right" },
			{ "HorseLRadius", "Horse Radius Left" },
			{ "HorseRRadius", "Horse Radius Right" },
			{ "Chelka 1", "Horse Chelka 1" },
			{ "Chelka 2", "Horse Chelka 2" },
			{ "Chelka 3", "Horse Chelka 3" },
			{ "Chelka 4", "Horse Chelka 4" },
			{ "Chelka 5", "Horse Chelka 5" },
			{ "SaddleBone", "Saddle" }

		};

		NodeMap::NodeMap()
		{
			for (auto& e : s_init)
			{
				m_data.try_emplace(e.node, e.desc, e.flags);
			}
		}

		void NodeMap::Merge(std::initializer_list<initializer_type> a_init)
		{
			for (auto& e : a_init)
			{
				Add(e.node, e.desc, e.flags);
			}
		}

		void NodeMap::Merge(std::initializer_list<map_type::value_type> a_init)
		{
			m_data  = a_init;
			m_dirty = true;
		}

		void NodeMap::Sort()
		{
			m_data.sortvec([](const auto& a_lhs, const auto& a_rhs) {
				return stl::fixed_string::less_str{}(a_lhs->second.name, a_rhs->second.name);
			});
		}

		bool NodeMap::RemoveUserNode(const stl::fixed_string& a_node)
		{
			if (auto it = m_data.find(a_node); it != m_data.end())
			{
				if (it->second.flags.test(NodeDescriptorFlags::kUserNode))
				{
					m_data.erase(it);
					return true;
				}
			}

			return false;
		}

		void NodeMap::SortIfDirty()
		{
			if (m_dirty)
			{
				Sort();
				m_dirty = false;
			}
		}

		bool NodeMap::SaveUserNodes(const fs::path& a_path) const
		{
			using namespace Serialization;

			try
			{
				stl::lock_guard lock(m_rwLock);

				ParserState      state;
				Parser<map_type> parser(state);
				Json::Value      root;

				parser.Create(m_data.getmap(), root, true);

				WriteData(a_path, root);

				return true;
			}
			catch (const std::exception& e)
			{
				m_lastException = e;
				return false;
			}
		}

		bool NodeMap::LoadExtra(const fs::path& a_path)
		{
			using namespace Serialization;

			try
			{
				stl::lock_guard lock(m_rwLock);

				Json::Value root;

				ReadData(a_path, root);

				ParserState      state;
				Parser<map_type> parser(state);

				map_type tmp;

				if (!parser.Parse(root, tmp, false))
				{
					throw parser_exception("parse failed");
				}

				for (auto& e : tmp)
				{
					Add(
						e.first,
						std::move(e.second.name),
						e.second.flags);
				}

				return true;
			}
			catch (const std::exception& e)
			{
				m_lastException = e;
				return false;
			}
		}

		bool NodeMap::ValidateNodeName(const std::string& a_name) noexcept
		{
			if (a_name.empty())
			{
				return false;
			}

			return std::find_if(
					   a_name.begin(),
					   a_name.end(),
					   [](auto& a_v) {
						   return a_v < 0x20 || a_v > 0x7E;
					   }) == a_name.end();
		}

	}
}