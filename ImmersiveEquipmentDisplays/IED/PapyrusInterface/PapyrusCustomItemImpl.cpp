#include "pch.h"

#include "PapyrusCustomItemImpl.h"

#include "IED/Controller/Controller.h"
#include "IED/FormCommon.h"
#include "IED/Main.h"
#include "IED/NodeMap.h"
#include "IED/StringHolder.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Custom
		{
			using namespace Data;

			bool CreateItemImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex,
				TESForm*                 a_form,
				bool                     a_inventoryForm,
				const BSFixedString&     a_node)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto node = GetOrCreateNodeDescriptor(a_node);
				if (!node)
				{
					return false;
				}

				auto& targetData = GetConfigMap(a_class);

				auto r = targetData.try_emplace(a_target);

				auto rp = r.first->second.try_emplace(a_key);
				auto re = rp.first->second.data.try_emplace(a_name);

				if (!re.second)
				{
					return false;
				}

				auto& e = re.first->second(a_sex);

				e.form = a_form->formID;
				if (a_inventoryForm)
				{
					e.customFlags.set(Data::CustomFlags::kIsInInventory);
				}
				e.targetNode = std::move(node);

				re.first->second.visit([](auto& a_v) {
					a_v.flags.set(BaseFlags::kDisabled);
				});

				return true;
			}

			bool DeleteItemImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto& targetData = GetConfigMap(a_class);

				auto itd = targetData.find(a_target);
				if (itd == targetData.end())
				{
					return false;
				}

				auto itp = itd->second.find(a_key);
				if (itp == itd->second.end())
				{
					return false;
				}

				bool r = itp->second.data.erase(a_name) != 0;

				if (r)
				{
					if (itp->second.data.empty())
					{
						itd->second.erase(itp);
						if (itd->second.empty())
						{
							targetData.erase(itd);
						}
					}

					QueueReset(a_target, a_class, a_key, a_name);
				}

				return r;
			}

			bool DeleteAllImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto& targetData = GetConfigMap(a_class);

				auto it = targetData.find(a_target);
				if (it == targetData.end())
				{
					return false;
				}

				bool r = it->second.erase(a_key) != 0;

				if (r)
				{
					if (it->second.empty())
					{
						targetData.erase(it);
					}

					QueueReset(a_target, a_class, a_key);
				}

				return r;
			}

			bool DeleteAllImpl(
				const stl::fixed_string& a_key)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto& store = Initializer::GetController()->GetConfigStore();

				stl::unordered_set<Game::FormID> erased;

				for (auto& e : store.active.custom.GetFormMaps())
				{
					for (auto itd = e.begin(); itd != e.end();)
					{
						bool ee = false;

						auto itp = itd->second.begin();
						while (itp != itd->second.end())
						{
							if (itp->first == a_key)
							{
								itp = itd->second.erase(itp);
								ee  = true;
							}
							else
							{
								++itp;
							}
						}

						if (ee)
						{
							erased.emplace(itd->first);
						}

						if (itd->second.empty())
						{
							itd = e.erase(itd);
						}
						else
						{
							++itd;
						}
					}
				}

				for (auto& e : erased)
				{
					Initializer::GetController()->QueueReset(e, ControllerUpdateFlags::kNone);
				}

				return !erased.empty();
			}

			bool SetItemAttachmentModeImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex,
				int                      a_attachmentMode,
				bool                     a_syncReference)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto old = e.flags;

				switch (a_attachmentMode)
				{
				case 0:
					e.flags.set(BaseFlags::kReferenceMode);
					break;
				case 1:
					if (e.targetNode.flags.test(NodeDescriptorFlags::kManaged))
					{
						return false;
					}
					e.flags.clear(BaseFlags::kReferenceMode);
					break;
				default:
					return false;
				}

				e.flags.set(BaseFlags::kSyncReferenceTransform, a_syncReference);

				if (e.flags != old && !e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class, a_key, a_name);
				}

				return true;
			}

			bool SetItemEnabledImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex,
				bool                     a_switch)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.flags.set(BaseFlags::kDisabled, !a_switch);

				QueueEvaluate(a_target, a_class);

				return true;
			}

			bool SetItemNodeImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex,
				const BSFixedString&     a_node)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto node = GetOrCreateNodeDescriptor(a_node);
				if (!node)
				{
					return false;
				}

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				if (!e.flags.test(BaseFlags::kReferenceMode) &&
				    node.flags.test(NodeDescriptorFlags::kManaged))
				{
					return false;
				}

				e.targetNode = std::move(node);

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			bool SetItemPositionImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex,
				const NiPoint3&          a_position)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.position = a_position;

				e.update_tag();

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueTransformUpdate(a_target, a_class, a_key, a_name);
				}

				return true;
			}

			bool SetItemRotationImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex,
				const NiPoint3&          a_rotation)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.rotation = a_rotation;

				e.update_rotation_matrix();
				e.update_tag();

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueTransformUpdate(a_target, a_class, a_key, a_name);
				}

				return true;
			}

			bool SetItemScaleImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex,
				float                    a_scale)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.scale = a_scale;

				e.update_tag();

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueTransformUpdate(a_target, a_class, a_key, a_name);
				}

				return true;
			}

			bool ClearItemTransformImpl(
				Game::FormID                   a_target,
				ConfigClass                    a_class,
				const stl::fixed_string&       a_key,
				const stl::fixed_string&       a_name,
				ConfigSex                      a_sex,
				stl::flag<TransformClearFlags> a_flags)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				if (a_flags.test(TransformClearFlags::Position))
				{
					e.position.reset();
				}

				if (a_flags.test(TransformClearFlags::Rotation))
				{
					e.rotation.reset();
				}

				if (a_flags.test(TransformClearFlags::Scale))
				{
					e.scale.clear();
					*e.scale = 1.0f;
				}

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueTransformUpdate(a_target, a_class, a_key, a_name);
				}

				return true;
			}

			bool SetItemInventoryImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex,
				bool                     a_switch)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.customFlags.set(Data::CustomFlags::kIsInInventory, a_switch);

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			bool SetItemFormImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex,
				TESForm*                 a_form)
			{
				if (!IFormCommon::IsValidCustomForm(a_form))
				{
					return false;
				}

				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.form = a_form->formID;

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			bool AddItemExtraFormImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				TESForm*                 a_form,
				std::int32_t             a_index)
			{
				if (!IFormCommon::IsValidCustomForm(a_form))
				{
					return false;
				}

				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				if (e.form.get_id() == a_form->formID)
				{
					return false;
				}

				auto sz = e.extraItems.size();
				if (!sz)
				{
					e.extraItems.emplace_back(a_form->formID);
				}
				else
				{
					if (std::find(
							e.extraItems.begin(),
							e.extraItems.end(),
							a_form->formID) != e.extraItems.end())
					{
						return false;
					}

					if (a_index < 0 || a_index >= sz)
					{
						e.extraItems.emplace_back(a_form->formID);
					}
					else
					{
						e.extraItems.insert(
							e.extraItems.begin() + a_index,
							a_form->formID);
					}
				}

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			bool RemoveItemExtraFormImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				TESForm*                 a_form)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto it = std::find(
					e.extraItems.begin(),
					e.extraItems.end(),
					a_form->formID);

				if (it == e.extraItems.end())
				{
					return false;
				}

				e.extraItems.erase(it);

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			bool RemoveItemExtraFormImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				std::int32_t             a_index)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto sz = e.extraItems.size();
				if (!sz || a_index < 0 || a_index >= sz)
				{
					return false;
				}

				e.extraItems.erase(e.extraItems.begin() + a_index);

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			std::int32_t GetNumExtraFormsImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				using size_type = decltype(e.extraItems)::size_type;

				return static_cast<std::int32_t>(std::clamp(
					e.extraItems.size(),
					size_type(0),
					size_type(std::numeric_limits<std::int32_t>::max())));
			}

			bool SetItemModelSwapFormImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex,
				TESForm*                 a_form)
			{
				if (!IFormCommon::IsValidCustomForm(a_form))
				{
					return false;
				}

				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.modelForm = a_form->formID;

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class);
				}

				return true;
			}

			bool ClearItemModelSwapFormImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				if (!e.modelForm.get_id())
				{
					return false;
				}

				e.modelForm = Game::FormID{};

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			bool SetItemCountRangeImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex,
				std::int32_t             a_min,
				std::int32_t             a_max)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.countRange = {
					static_cast<std::uint32_t>(std::clamp(a_min, 0, INT32_MAX)),
					static_cast<std::uint32_t>(std::clamp(a_max, 0, INT32_MAX)),
				};

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			bool SetItemEquipmentModeImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch,
				bool                     a_ignoreRaceEquipTypes,
				bool                     a_disableIfEquipped)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.customFlags.set(Data::CustomFlags::kEquipmentMode, a_switch);
				e.customFlags.set(Data::CustomFlags::kIgnoreRaceEquipTypes, a_ignoreRaceEquipTypes);
				e.customFlags.set(Data::CustomFlags::kDisableIfEquipped, a_disableIfEquipped);

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			bool SetItemLeftWeaponImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto old = e.customFlags;

				e.customFlags.set(CustomFlags::kLeftWeapon, a_switch);

				if (e.customFlags != old && !e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class, a_key, a_name);
				}

				return true;
			}

			bool SetItemRemoveScabbardImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto old = e.flags;

				e.flags.set(BaseFlags::kRemoveScabbard, a_switch);

				if (e.flags != old && !e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class, a_key, a_name);
				}

				return true;
			}

			bool SetItemUseWorldModelImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto old = e.flags;

				e.flags.set(BaseFlags::kUseWorldModel, a_switch);

				if (e.flags != old && !e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class, a_key, a_name);
				}

				return true;
			}

			bool SetIgnoreRaceEquipTypesImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto old = e.customFlags;

				e.customFlags.set(CustomFlags::kIgnoreRaceEquipTypes, a_switch);

				if (e.customFlags != old && !e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class, a_key, a_name);
				}

				return true;
			}

			bool SetItemLoadChanceImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_enable,
				float                    a_chance)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.customFlags.set(CustomFlags::kUseChance, a_enable);

				e.probability = std::clamp(a_chance, 0.0f, 100.0f);

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			bool SetItemAnimationEnabledImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_enable)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto old = e.flags;

				e.flags.set(BaseFlags::kPlaySequence, a_enable);

				if (old != e.flags && !e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class);
				}

				return true;
			}

			bool SetItemAnimationSequenceImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				const stl::fixed_string& a_sequence)
			{
				if (a_sequence.empty())
				{
					return false;
				}

				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.niControllerSequence = a_sequence;

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			bool SetItemWeaponAnimationDisabledImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_enable)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto old = e.flags;

				e.flags.set(BaseFlags::kDisableBehaviorGraphAnims, a_enable);

				if (old != e.flags && !e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class);
				}

				return true;
			}

			bool SetItemAnimationEventEnabledImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_enable)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto old = e.flags;

				e.flags.set(BaseFlags::kAnimationEvent, a_enable);

				if (old != e.flags && !e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class);
				}

				return true;
			}

			bool SetItemAnimationEventImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				const stl::fixed_string& a_event)
			{
				if (a_event.empty())
				{
					return false;
				}

				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				e.animationEvent = a_event;

				if (!e.flags.test(BaseFlags::kDisabled))
				{
					QueueEvaluate(a_target, a_class);
				}

				return true;
			}

			bool SetItemDisableHavokImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_disable)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto old = e.flags;

				e.flags.set(BaseFlags::kDisableHavok, a_disable);

				if (old != e.flags && !e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class);
				}

				return true;
			}

			bool SetItemRemoveTracersImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_disable)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto old = e.flags;

				e.flags.set(BaseFlags::kRemoveProjectileTracers, a_disable);

				if (old != e.flags && !e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class);
				}

				return true;
			}

			bool SetItemAttachLightImpl(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				Data::ConfigSex          a_sex,
				bool                     a_switch)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				auto& e = conf->get(a_sex);

				auto old = e.flags;

				e.flags.set(BaseFlags::kAttachLight, a_switch);

				if (old != e.flags && !e.flags.test(BaseFlags::kDisabled))
				{
					QueueReset(a_target, a_class);
				}

				return true;
			}

			bool ItemExistsImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				return LookupConfig(a_target, a_class, a_key, a_name) != nullptr;
			}

			bool ItemEnabledImpl(
				Game::FormID             a_target,
				ConfigClass              a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name,
				ConfigSex                a_sex)
			{
				const stl::lock_guard lock(Initializer::GetController()->GetLock());

				auto conf = LookupConfig(a_target, a_class, a_key, a_name);
				if (!conf)
				{
					return false;
				}

				return !conf->get(a_sex).flags.test(BaseFlags::kDisabled);
			}

		}
	}
}