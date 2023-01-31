#include "pch.h"

#include "I3DIMain.h"

#include "I3DIActorObject.h"
#include "I3DIFreeCamera.h"
#include "I3DIInputHelpers.h"
#include "I3DIObjectCamera.h"

#include "IED/UI/PopupQueue/UIPopupQueue.h"

#include "Drivers/Render.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/Widgets/UIWidgetCommonStrings.h"

//#include <ext/Clouds.h>
//#include <ext/Sky.h>

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		I3DIMain::I3DIMain(
			Controller& a_controller) :
			m_controller(a_controller),
			m_prepPT(1000000)
		{
		}

		void I3DIMain::Initialize()
		{
		}

		void I3DIMain::Draw()
		{
			if (!m_data)
			{
				if (!CreateCommonData())
				{
					SetOpenState(false);
					return;
				}
			}

			if (I3DI::IsMouseInputValid())
			{
				m_data->UpdateMouseRay();
			}

			ImGui::PushID(WINDOW_ID);
			
			if (auto& queued = m_data->queuedActor)
			{
				const auto actor = *queued;
				queued.reset();

				if (actor == Game::FormID{})
				{
					ReleaseCurrentActorContext();
				}
				else
				{
					SwitchToActorContext(actor);
				}
			}

			if (auto& context = m_actorContext)
			{
				if (context->LastUpdateFailed())
				{
					ReleaseCurrentActorContext();
				}
				else
				{
					context->Draw(*m_data);
				}
			}

			m_data->objectController.Run(*m_data);

			ImGui::PopID();
		}

		void I3DIMain::PrepareGameData()
		{
			m_prepPT.Begin();

			PrepareGameDataImpl();

			m_prepPT.End(m_lastPrepTime);
		}

		void I3DIMain::Render()
		{
			if (!m_data)
			{
				return;
			}

			/*if (Game::InPausedMenu())
			{
				return;
			}*/

			const auto& rd = Drivers::Render::GetSingleton();

			D3D11StateBackupScoped backup(
				m_data->scene.GetStateBackup(true),
				rd.GetContext().Get(),
				true);

			m_data->scene.PreDraw();

			/*m_data->oit.Render(m_data->scene, [&]() {
				m_data->objectController.RenderObjects(*m_data);
			});*/

			m_data->objectController.DrawObjects(*m_data);

			m_data->batchNoDepth.Draw(m_data->scene);
			m_data->batchDepth.Draw(m_data->scene);

			/*if (m_actorContext)
			{
				m_actorContext->Render(*m_data);
			}*/

			m_data->scene.PostDraw();
		}

		void I3DIMain::OnOpen()
		{
		}

		void I3DIMain::OnClose()
		{
			for (auto& e : m_controller.GetActorMap())
			{
				e.second.SetNodeConditionForced(false);
			}

			if (m_actorContext)
			{
				//m_heldActorOnClose.emplace(m_actorContext->GetActorFormID());

				ReleaseCurrentActorContext();
			}

			m_data.reset();
		}

		bool I3DIMain::OnWantOpenStateChange(bool a_newState)
		{
			return m_controller.CPUHasSSE41();
		}

		void I3DIMain::OnMouseMove(const Handlers::MouseMoveEvent& a_evn)
		{
			if (!m_data)
			{
				return;
			}

			if (auto& context = m_actorContext)
			{
				context->OnMouseMoveEvent(*m_data, a_evn);
			}
		}

		bool I3DIMain::CreateCommonData()
		{
			const auto& rd = Drivers::Render::GetSingleton();

			try
			{
				m_data = std::make_unique<I3DICommonData>(
					rd.GetDevice().Get(),
					rd.GetContext().Get(),
					rd.GetSwapChainInfo(),
					m_actorContext,
					m_controller);

				return true;
			}
			catch (const std::exception& e)
			{
				auto& queue = m_controller.UIGetPopupQueue();

				queue.push(
					UIPopupType::Message,
					UIL::LS(CommonStrings::Error),
					"%s\n\n%s",
					__FUNCTION__,
					e.what());

				return false;
			}
			catch (...)
			{
				auto& queue = m_controller.UIGetPopupQueue();

				queue.push(
					UIPopupType::Message,
					UIL::LS(CommonStrings::Error),
					"%s",
					__FUNCTION__);

				return false;
			}
		}

		void I3DIMain::PrepareGameDataImpl()
		{
			if (!m_data)
			{
				return;
			}

			if (auto camera = GetCamera())
			{
				/*if (auto& context = m_actorContext)
				{
					auto cam = context->GetCamera().get();

					if (!cam)
					{
						context->CreateCamera<I3DIFreeCamera>(camera);
						cam = context->GetCamera().get();
					}

					cam->CameraUpdate(camera);

					auto sky = RE::Sky::GetSingleton();

					sky->clouds->GetRoot()->m_worldTransform.pos = camera->m_worldTransform.pos;
				}*/

				VectorMath::GetCameraPV(
					camera,
					m_data->scene.GetViewMatrix(),
					m_data->scene.GetProjectionMatrix(),
					m_data->scene.GetCameraPosition());
			}

			UpdateActorObjects();

			if (auto& context = m_actorContext)
			{
				context->Update(*m_data);
			}

			if (auto& actor = m_heldActorOnClose)
			{
				if (!m_data->queuedActor)
				{
					m_data->queuedActor.emplace(*actor);
				}

				actor.reset();
			}
		}

		bool I3DIMain::SwitchToActorContext(Game::FormID a_actor)
		{
			auto& data = m_controller.GetActorMap();

			auto ith = data.find(a_actor);
			if (ith == data.end())
			{
				return false;
			}

			auto ita = m_data->actors.find(a_actor);
			if (ita == m_data->actors.end())
			{
				return false;
			}

			if (m_actorContext)
			{
				if (m_actorContext->GetActorFormID() == a_actor)
				{
					return true;
				}

				ReleaseCurrentActorContext();
			}

			try
			{
				m_actorContext = std::make_unique<
					I3DIActorContext>(
					*m_data,
					m_controller,
					ith->second,
					ita->second);
			}
			catch (const std::exception& e)
			{
				auto& queue = m_controller.UIGetPopupQueue();

				queue.push(
					UIPopupType::Message,
					UIL::LS(CommonStrings::Error),
					"%s\n\n%s",
					__FUNCTION__,
					e.what());

				return false;
			}
			catch (...)
			{
				auto& queue = m_controller.UIGetPopupQueue();

				queue.push(
					UIPopupType::Message,
					UIL::LS(CommonStrings::Error),
					"%s",
					__FUNCTION__);

				return false;
			}

			m_actorContext->RegisterObjects(m_data->objectController);

			ith->second.RequestTransformUpdate();

			if (ImGui::GetIO().KeyShift)
			{
				m_controller.QueueSendAnimationEventToActor(a_actor, "IdleStaticPoseAStart");
				m_actorContext->SetAnimEventSent();
			}

			return true;
		}

		void I3DIMain::ReleaseCurrentActorContext()
		{
			if (m_actorContext)
			{
				SetNodeConditionForced(m_actorContext->GetActorFormID(), false);

				if (m_actorContext->GetAnimEventSent())
				{
					m_controller.QueueSendAnimationEventToActor(
						m_actorContext->GetActorFormID(),
						"IdleForceDefaultState");
				}

				m_actorContext->UnregisterObjects(m_data->objectController);
				m_actorContext.reset();
			}
		}

		void I3DIMain::UpdateActorObjects()
		{
			auto& actors = m_data->actors;

			for (auto& [i, e] : m_controller.GetActorMap())
			{
				if (!e.IsActive())
				{
					if (m_actorContext && m_actorContext->GetActorFormID() == i)
					{
						ReleaseCurrentActorContext();
					}

					auto it = actors.find(i);
					if (it != actors.end())
					{
						m_data->objectController.UnregisterObject(it->second);
						actors.erase(it);

						//_DMESSAGE("lost (1): %.8X", i);
					}

					e.SetNodeConditionForced(false);

					continue;
				}

				auto it = actors.find(i);

				if (it == actors.end())
				{
					it = actors.try_emplace(
								   i,
								   std::make_shared<I3DIActorObject>(i))
					         .first;

					m_data->objectController.RegisterObject(it->second);

					//_DMESSAGE("acq: %.8X", i);
				}

				it->second->Update(e);
			}

			for (auto it = actors.begin(); it != actors.end();)
			{
				if (!m_controller.GetActorMap().contains(it->first))
				{
					//_DMESSAGE("lost (2): %.8X", it->first);

					if (m_actorContext && m_actorContext->GetActorFormID() == it->first)
					{
						ReleaseCurrentActorContext();
					}

					m_data->objectController.UnregisterObject(it->second);

					it = actors.erase(it);
				}
				else
				{
					if (it->first == Data::IData::GetPlayerRefID())
					{
						it->second->SetDisabled(m_data->controller.IsInFirstPerson());
					}

					++it;
				}
			}
		}

		static void DumpTf(const char* a_id, const NiTransform& a_tf)
		{
			_DMESSAGE(">> %s\n", a_id);

			_DMESSAGE("Scale: %f", a_tf.scale);
			_DMESSAGE("Pos: %f %f %f", a_tf.pos.x, a_tf.pos.y, a_tf.pos.z);
			_DMESSAGE(
				"Mat:\n"
				"%f %f %f\n"
				"%f %f %f\n"
				"%f %f %f\n",
				a_tf.rot.data[0][0],
				a_tf.rot.data[0][1],
				a_tf.rot.data[0][2],
				a_tf.rot.data[1][0],
				a_tf.rot.data[1][1],
				a_tf.rot.data[1][2],
				a_tf.rot.data[2][0],
				a_tf.rot.data[2][1],
				a_tf.rot.data[2][2]);

			_DMESSAGE("<< %s\n\n", a_id);
		}

		NiPointer<NiCamera> I3DIMain::GetCamera()
		{
			auto pc = PlayerCamera::GetSingleton();
			if (!pc)
			{
				return nullptr;
			}

			return pc->GetNiCamera();
		}

		void I3DIMain::SetNodeConditionForced(Game::FormID a_actor, bool a_switch) const
		{
			auto& data = m_controller.GetActorMap();

			if (auto it = data.find(a_actor); it != data.end())
			{
				it->second.SetNodeConditionForced(a_switch);
			}
		}
	}
}