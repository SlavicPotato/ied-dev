#include "pch.h"

#include "NodeMap.h"

#include "Parsers/JSONNodeMapParser.h"

namespace IED
{
	namespace Data
	{
		NodeMap NodeMap::m_Instance;

		NodeMap::NodeMap()
		{
			Merge({

				{ "WeaponBack", "Two-Handed Weapons Back", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
				{ "WeaponBackSWP", "Two-Handed Weapons Back SWP", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponBackAxeMace", "Weapon Back Axe/Mace", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
				{ "WeaponBackAxeMaceSWP", "Weapon Back Axe/Mace SWP", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponAxe", "One-Handed Axe", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
				{ "WeaponAxeLeft", "One-Handed Axe Left", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
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
				{ "WeaponStaff", "Staff", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
				{ "WeaponStaffLeft", "Staff Left", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
				{ "WeaponBow", "Bow", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
				{ "WeaponBowFSM", "Bow FSM", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponBowBetter", "Better Bow", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponBowChesko", "Chesko Bow", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponCrossBow", "Crossbow", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponCrossBowChesko", "Chesko Crossbow", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponSword", "One-Handed Sword", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
				{ "WeaponSwordLeft", "One-Handed Sword Left", NodeDescriptorFlags::kManaged | NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kDefault | NodeDescriptorFlags::kWeapon },
				{ "WeaponSwordOnBack", "One-Handed Sword on Back", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponSwordLeftOnBack", "One-Handed Sword on Back Left", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponSwordSWP", "One-Handed Sword SWP", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponSwordLeftSWP", "One-Handed Sword SWP Left", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponSwordLeftHip", "One-Handed Sword on Left Hip", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponSwordLeftLeftHip", "One-Handed Sword on Left Hip Left", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "WeaponSwordNMD", "One-Handed Sword NMD", NodeDescriptorFlags::kXP32 | NodeDescriptorFlags::kWeapon },
				{ "Weapon", "Right Weapon", NodeDescriptorFlags::kManaged },
				{ "Shield", "Left Weapon / Shield", NodeDescriptorFlags::kManaged },
				{ "ShieldBack", "Shield Back", NodeDescriptorFlags::kXP32 },
				{ "NPC Spine [Spn0]", "Spine Lower" },
				{ "NPC Spine1 [Spn1]", "Spine Middle" },
				{ "NPC Spine2 [Spn2]", "Spine Upper" },
				{ "NPC Pelvis [Pelv]", "Pelvis" },
				{ "NPC Head [Head]", "Head" },
				{ "NPC Root [Root]", "Root" },
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
				{ "NPCEyeBone", "Eyes" },
				{ "HorseSpine2", "Horse Spine Middle" },
				{ "HorseSpine3", "Horse Spine Upper" },
				{ "HorsePelvis", "Horse Pelvis" },
				{ "HorseScull", "Horse Scull" },
				{ "SaddleBone", "Saddle" }

			});
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
			m_data = a_init;
			m_dirty = true;
		}

		auto NodeMap::Add(
			const char* a_node,
			const char* a_desc,
			stl::flag<NodeDescriptorFlags> a_flags)
			-> std::pair<map_type::iterator, bool>
		{
			auto r = m_data.try_emplace(
				a_node,
				a_desc,
				a_flags);

			if (r.second)
			{
				m_dirty = true;
			}

			return r;
		}

		void NodeMap::Sort()
		{
			m_data.sortvec([](const auto& a_lhs, const auto& a_rhs) {
				return a_lhs->second.name < a_rhs->second.name;
			});
		}

		bool NodeMap::RemoveExtra(const stl::fixed_string& a_node)
		{
			auto it = m_data.find(a_node);
			if (it != m_data.end())
			{
				if (it->second.flags.test(NodeDescriptorFlags::kExtra))
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

		bool NodeMap::SaveExtra(const fs::path& a_path)
		{
			using namespace Serialization;

			try
			{
				IScopedLock lock(m_rwLock);

				Parser<map_type> parser;
				Json::Value root;

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
				IScopedLock lock(m_rwLock);

				Json::Value root;

				ReadData(a_path, root);

				Parser<map_type> parser;
				map_type tmp;

				if (!parser.Parse(root, tmp, false))
				{
					throw std::exception("parse failed");
				}

				for (auto& e : tmp)
				{
					Add(e.first.c_str(), e.second.name.c_str(), e.second.flags);
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

			for (auto& e : a_name)
			{
				if (e < 0x20 || e > 0x7E)
				{
					return false;
				}
			}
			return true;
		}

	}
}