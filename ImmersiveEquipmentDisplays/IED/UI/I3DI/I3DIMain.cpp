#include "pch.h"

#include "I3DIMain.h"

#include "I3DIActorObject.h"
#include "I3DIFreeCamera.h"
#include "I3DIInputHelpers.h"
#include "I3DIObjectCamera.h"

#include "IED/UI/PopupQueue/UIPopupQueue.h"

#include "Drivers/Render.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		using namespace DirectX;

		I3DIMain::I3DIMain(
			Controller& a_controller) :
			UIContextBase(a_controller),
			UILocalizationInterface(a_controller)
		{
		}

		void I3DIMain::Initialize()
		{
		}

		void I3DIMain::Draw()
		{
			if (!IsWindowOpen())
			{
				return;
			}

			if (!m_data)
			{
				if (!CreateCommonData())
				{
					SetOpenState(false);
					return;
				}
			}

			auto& io = ImGui::GetIO();

			m_data->UpdateRay();

			ImGui::PushID(WINDOW_ID);

			if (auto& queued = m_data->queuedActor)
			{
				auto actor = *queued;
				queued.reset();

				SwitchToActorContext(actor);
			}

			if (auto& context = m_actorContext)
			{
				if (context->LastUpdateFailed())
				//m_actorContext->GetActorObject()->IsActorLost())
				{
					SetNodeConditionForced(context->GetActorFormID(), false);

					context->UnregisterObjects(m_data->objectController);
					context.reset();
				}
				else
				{
					context->Draw(*m_data);
				}
			}

			m_data->objectController.Run(*m_data);

			ImGui::PopID();

			auto& ui = m_controller.GetConfigStore().settings.data.ui;

			if (io.KeyAlt ? ui.enableControlLock : !ui.enableControlLock)
			{
				ui.enableControlLock = !ui.enableControlLock;
				m_controller.UIGetRenderTask()->SetControlLock(ui.enableControlLock);
				Drivers::UI::EvaluateTaskState();

				/*if (m_actorContext)
				{
					if (auto& camera = m_actorContext->GetActorObject()->GetCamera())
					{
						if (auto freeCam = dynamic_cast<I3DIFreeCamera*>(camera.get()))
						{
							freeCam->EnableTranslation(ui.enableControlLock);
						}
					}
				}*/
			}
		}

		void I3DIMain::PrepareGameData()
		{
			if (!IsWindowOpen())
			{
				return;
			}

			if (!m_data)
			{
				return;
			}

			auto camera = GetCamera();

			if (camera)
			{
				/*if (auto& context = m_actorContext)
				{
					if (!context->HasCamera())
					{
						context->SetCamera(std::make_unique<I3DIObjectCamera>(camera));
					}

					if (auto cc = dynamic_cast<I3DIObjectCamera*>(context->GetCamera().get()))
					{
						auto& obj = m_actorContext->GetActorObject();

						cc->i_p = XMLoadFloat3(&obj->GetActorBound().Center);

						context->UpdateCamera(camera);
					}
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
				context->Update();
			}
		}

		void I3DIMain::Render()
		{
			if (!IsWindowOpen())
			{
				return;
			}

			if (!m_data)
			{
				return;
			}

			const auto& rd = Drivers::Render::GetSingleton();

			D3D11StateBackupScoped backup(
				m_data->scene.GetStateBackup(true),
				rd.GetContext().Get(),
				true);

			m_data->scene.PreDraw();

			/*m_data->oit.Render(m_data->scene, [&]() {
				m_data->objectController.RenderObjects(*m_data);
			});*/

			m_data->objectController.RenderObjects(*m_data);

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
			for (auto& e : m_controller.GetData())
			{
				e.second.SetNodeConditionForced(false);
			}

			m_actorContext.reset();
			m_data.reset();
		}

		void I3DIMain::OnMouseMove(const Handlers::MouseMoveEvent& a_evn)
		{
			if (!IsWindowOpen())
			{
				return;
			}

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
					m_actorContext);

				return true;
			}
			catch (const std::exception& e)
			{
				auto& queue = m_controller.UIGetPopupQueue();

				queue.push(
					UIPopupType::Message,
					LS(CommonStrings::Error),
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
					LS(CommonStrings::Error),
					"%s",
					__FUNCTION__);

				return false;
			}
		}

		void I3DIMain::SwitchToActorContext(Game::FormID a_actor)
		{
			if (m_actorContext)
			{
				SetNodeConditionForced(m_actorContext->GetActorFormID(), false);

				m_actorContext->UnregisterObjects(m_data->objectController);
				m_actorContext.reset();
			}

			auto& data = m_controller.GetData();

			auto ith = data.find(a_actor);
			if (ith == data.end())
			{
				return;
			}

			auto ita = m_data->actors.find(a_actor);
			if (ita == m_data->actors.end())
			{
				return;
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
					LS(CommonStrings::Error),
					"%s\n\n%s",
					__FUNCTION__,
					e.what());

				return;
			}
			catch (...)
			{
				auto& queue = m_controller.UIGetPopupQueue();

				queue.push(
					UIPopupType::Message,
					LS(CommonStrings::Error),
					"%s",
					__FUNCTION__);

				return;
			}

			m_actorContext->RegisterObjects(m_data->objectController);

			ith->second.RequestTransformUpdate();
			m_controller.QueueSendAnimationEventToActor(a_actor, "idlestaticposeastart");
		}

		void I3DIMain::UpdateActorObjects()
		{
			auto& actors = m_data->actors;

			for (auto& [i, e] : m_controller.GetData())
			{
				auto& actor = e.GetActor();

				if (!e.IsCellAttached() || !actor->formID)
				{
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
					auto object = std::make_shared<I3DIActorObject>(i);

					it = actors.try_emplace(i, object).first;

					m_data->objectController.RegisterObject(object);

					//_DMESSAGE("acq: %.8X", i);
				}

				it->second->Update(e);
			}

			for (auto it = actors.begin(); it != actors.end();)
			{
				if (!m_controller.GetData().contains(it->first))
				{
					//_DMESSAGE("lost (2): %.8X", it->first);

					m_data->objectController.UnregisterObject(it->second);

					it = actors.erase(it);
				}
				else
				{
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

		NiCamera* I3DIMain::GetCamera()
		{
			auto pc = PlayerCamera::GetSingleton();
			if (!pc)
			{
				return nullptr;
			}

			auto camera = pc->GetNiCamera();
			if (!camera)
			{
				return nullptr;
			}

			return camera;
		}

		void I3DIMain::SetNodeConditionForced(Game::FormID a_actor, bool a_switch) const
		{
			auto& data = m_controller.GetData();

			if (auto it = data.find(a_actor); it != data.end())
			{
				it->second.SetNodeConditionForced(a_switch);
			}
		}
	}
}