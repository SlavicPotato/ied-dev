#include "pch.h"

#include "I3DIActorContext.h"

#include "I3DIActorObject.h"
#include "I3DICommonData.h"
#include "I3DIObjectController.h"

#include "IED/Controller/Controller.h"
#include "IED/NodeOverrideData.h"

#include "Drivers/Input/Handlers.h"

#include "I3DIBoundingSphere.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		static stl::fixed_string GetProbableCMENameFromMOV(
			const stl::fixed_string& a_node)
		{
			if (a_node.size() < 4)
			{
				return {};
			}

			if (_strnicmp(a_node.c_str(), "MOV ", 4) != 0)
			{
				return {};
			}

			std::string tmp(a_node);

			tmp[0] = 'C';
			tmp[1] = 'M';
			tmp[2] = 'E';

			return tmp;
		}

		I3DIActorContext::I3DIActorContext(
			I3DICommonData&                         a_data,
			Controller&                             a_controller,
			const ActorObjectHolder&                a_holder,
			const std::shared_ptr<I3DIActorObject>& a_actorObject) noexcept(false) :
			m_controller(a_controller),
			m_actor(a_holder.GetActorFormID()),
			m_actorObject(a_actorObject)
		{
			auto& activeWeaponNodes = a_holder.GetWeapNodes();
			auto& cme               = NodeOverrideData::GetCMENodeData();
			auto& movAnchorModel    = a_data.assets.GetModel(I3DIModelID::kAttachmentPoint);

			if (!movAnchorModel)
			{
				throw std::exception("anchor model not loaded");
			}

			for (auto& e : NodeOverrideData::GetWeaponNodeData())
			{
				auto& model = a_data.assets.GetModel(e.second.modelID);
				if (!model)
				{
					continue;
				}

				auto itwn = std::find_if(
					activeWeaponNodes.begin(),
					activeWeaponNodes.end(),
					[&](auto& a_v) {
						return a_v.nodeName == e.first;
					});

				if (itwn == activeWeaponNodes.end())
				{
					continue;
				}

				auto r = m_weaponNodes.emplace(
					e.first,
					std::make_unique<I3DIWeaponNode>(
						a_data.scene.GetDevice().Get(),
						model,
						e.first,
						e.second,
						*this));

				r.first->second->EnableDepth(true);

				for (auto& f : e.second.movs)
				{
					const NodeOverrideData::overrideNodeEntry_t* cmeNodeEntry = nullptr;

					if (auto name = GetProbableCMENameFromMOV(f.first); !name.empty())
					{
						if (auto it = cme.find(name); it != cme.end())
						{
							cmeNodeEntry = std::addressof(it->second);
						}
					}

					auto s = m_movNodes.emplace(
						f.first,
						std::make_unique<I3DIMOVNode>(
							a_data.scene.GetDevice().Get(),
							movAnchorModel,
							f.first,
							f.second,
							cmeNodeEntry,
							*r.first->second,
							*this));

					s.first->second->EnableDepth(false);
				}
			}
		}

		void I3DIActorContext::RegisterObjects(
			I3DIObjectController& a_objectController)
		{
			for (auto& e : m_weaponNodes)
			{
				a_objectController.RegisterObject(e.second);
			}

			for (auto& e : m_movNodes)
			{
				a_objectController.RegisterObject(e.second);
			}
		}

		void I3DIActorContext::UnregisterObjects(
			I3DIObjectController& a_objectController)
		{
			for (auto& e : m_weaponNodes)
			{
				a_objectController.UnregisterObject(e.second);
			}

			for (auto& e : m_movNodes)
			{
				a_objectController.UnregisterObject(e.second);
			}

			for (auto& e : m_physicsObjects)
			{
				a_objectController.UnregisterObject(e.second);
			}
		}

		bool I3DIActorContext::Update(I3DICommonData& a_data)
		{
			auto& data = m_controller.GetObjects();

			auto it = data.find(m_actor);
			if (it == data.end())
			{
				m_lastUpdateFailed = true;
				return false;
			}

			const bool weaponsEnabled = a_data.controller.GetSettings().data.ui.i3di.enableWeapons;

			if (weaponsEnabled)
			{
				auto& weaponNodes = it->second.GetWeapNodes();

				for (auto& e : weaponNodes)
				{
					auto itwn = m_weaponNodes.find(e.nodeName);
					if (itwn == m_weaponNodes.end())
					{
						continue;
					}

					itwn->second->UpdateLocalMatrix(e.node->m_localTransform);

					const auto m = VectorMath::NiTransformToMatrix4x4(e.node->m_worldTransform);

					itwn->second->UpdateWorldMatrix(m);
					itwn->second->SetOriginalWorldMatrix(m);

					itwn->second->UpdateBound();
					itwn->second->SetHasWorldData(true);
					itwn->second->SetGeometryHidden(false);
				}

				auto& cmeNodes = it->second.GetCMENodes();

				for (auto& e : it->second.GetMOVNodes())
				{
					auto itmn = m_movNodes.find(e.first);
					if (itmn == m_movNodes.end())
					{
						continue;
					}

					const auto m = VectorMath::NiTransformToMatrix4x4(e.second.node->m_worldTransform);

					itmn->second->UpdateWorldMatrix(m);
					itmn->second->SetOriginalWorldMatrix(m);

					itmn->second->UpdateBound();
					itmn->second->SetHasWorldData(true);

					auto itwn = std::find_if(
						weaponNodes.begin(),
						weaponNodes.end(),
						[&](auto& a_v) {
							return a_v.node->m_parent == e.second.node;
						});

					itmn->second->SetWeaponNodeAttached(itwn != weaponNodes.end());

					if (itwn != weaponNodes.end())
					{
						auto ita = m_weaponNodes.find(itwn->nodeName);
						if (ita != m_weaponNodes.end())
						{
							if (auto info = itmn->second->GetCMENodeInfo())
							{
								auto itb = cmeNodes.find(info->name);
								if (itb != cmeNodes.end())
								{
									ita->second->SetGeometryHidden(
										itb->second.has_visible_geometry(nullptr));
								}
							}
						}
					}
				}
			}

			auto& sclist = it->second.GetSimComponentList();

			for (auto& e : sclist)
			{
				auto itp = m_physicsObjects.find(e->GetLUID());
				if (itp == m_physicsObjects.end())
				{
					auto r = m_physicsObjects.emplace(
						e->GetLUID(),
						std::make_unique<I3DIPhysicsObject>(a_data, *this, *e));

					a_data.objectController.RegisterObject(r.first->second);
				}
				else
				{
					itp->second->UpdateData(*e);
				}
			}

			if (m_physicsObjects.size() > sclist.size())
			{
				stl::unordered_set<luid_tag> tmp;

				for (auto& e : sclist)
				{
					tmp.emplace(e->GetLUID());
				}

				for (auto itp = m_physicsObjects.begin(); itp != m_physicsObjects.end();)
				{
					if (!tmp.contains(itp->second->GetLUID()))
					{
						a_data.objectController.UnregisterObject(itp->second);

						itp = m_physicsObjects.erase(itp);
					}
					else
					{
						++itp;
					}
				}
			}

			m_lastUpdateFailed = false;
			m_ranFirstUpdate   = true;

			it->second.SetNodeConditionForced(weaponsEnabled);

			return true;
		}

		void I3DIActorContext::Draw(I3DICommonData& a_data)
		{
			if (!m_ranFirstUpdate)
			{
				return;
			}

			if (auto& camera = m_camera)
			{
				camera->CameraSetTranslation();
			}
		}

		void I3DIActorContext::UpdateCamera(NiCamera* a_camera)
		{
			if (auto& camera = m_camera)
			{
				camera->CameraUpdate(a_camera);
			}
		}

		void I3DIActorContext::OnMouseMoveEvent(
			I3DICommonData&                 a_data,
			const Handlers::MouseMoveEvent& a_evn)
		{
			if (auto& camera = m_camera)
			{
				camera->CameraProcessMouseInput(a_evn);
			}
		}
	}
}