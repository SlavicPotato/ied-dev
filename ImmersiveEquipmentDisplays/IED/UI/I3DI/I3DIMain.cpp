#include "pch.h"

#include "I3DIMain.h"

#include "I3DIActorObject.h"
#include "I3DIInputHelpers.h"

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

			const auto& rd = Drivers::Render::GetSingleton();

			if (!m_data)
			{
				m_data = std::make_unique<I3DICommonData>(
					rd.GetDevice().Get(),
					rd.GetContext().Get(),
					rd.GetSwapChainInfo(),
					m_actorContext);

				m_data->scene.EnableAlpha(true);
				m_data->scene.EnableDepth(true);

				m_data->batchNoDepth.EnableDepth(false);
				m_data->batchDepth.EnableDepth(true);
			}

			ImGui::PushID(WINDOW_ID);

			auto& io = ImGui::GetIO();

			m_data->UpdateRay();

			/*if (!io.WantCaptureMouse)
			{
				if (auto h = m_data->actorSelector.GetHovered(
						m_data->ray.origin,
						m_data->ray.dir))
				{
					if (!m_actorContext || m_actorContext->GetActorFormID() != h->first)
					{
						m_data->actorSelector.DrawBox(
							h->second,
							{ 0.5f, 0.5f, 0.5f, 0.5f });

						if (I3DI::IsMouseReleased())  // && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::GetTopMostPopupModal())
						{
							if (!m_actorContext || m_actorContext->GetActorFormID() != h->first)
							{
								m_actorContext = std::make_unique<
									I3DIActorContext>(
									*m_data,
									m_controller,
									h->first,
									XMLoadFloat3(std::addressof(h->second.bound.Center)));

								m_controller.QueueSendAnimationEventToActor(h->first, "idlestaticposeastart");
							}
						}

						XMVECTOR origin;

						auto pos = m_data->actorSelector.GetPopupAnchorPoint(
							m_data->scene,
							h->second,
							origin);

						m_data->commonPopup.SetLineWorldOrigin(origin);
						m_data->commonPopup.SetPosition(pos, { 0.5f, 1.f });

						char b[64];
						stl::snprintf(b, "###as_p_%.8X", h->first.get());

						m_data->commonPopup.Draw(b, *m_data, [&] {
							ImGui::Text("0x%.8X", h->first.get());
						});
					}
				}
			}*/

			if (m_data->queuedActor)
			{
				if (m_actorContext)
				{
					m_actorContext->UnregisterObjects(m_data->objectController);
					m_actorContext.reset();
				}

				auto actorfid = *m_data->queuedActor;
				m_data->queuedActor.reset();

				auto it = m_data->actors.find(actorfid);
				if (it != m_data->actors.end())
				{
					m_actorContext = std::make_unique<
						I3DIActorContext>(
						*m_data,
						m_controller,
						it->first,
						it->second);

					m_actorContext->RegisterObjects(m_data->objectController);

					m_controller.QueueSendAnimationEventToActor(actorfid, "idlestaticposeastart");
				}
			}

			if (m_actorContext)
			{
				if (m_actorContext->LastUpdateFailed())
				//m_actorContext->GetActorObject()->IsActorLost())
				{
					m_actorContext->UnregisterObjects(m_data->objectController);
					m_actorContext.reset();
				}
				else
				{
					m_actorContext->Draw(*m_data);
				}
			}

			m_data->objectController.Run(*m_data);

			ImGui::PopID();

			auto& ui = m_controller.GetConfigStore().settings.data.ui;

			if (io.KeyAlt ? ui.enableControlLock : !ui.enableControlLock)
			{
				ui.enableControlLock = !ui.enableControlLock;
				m_controller.UIGetRenderTask()->SetLock(ui.enableControlLock);
				Drivers::UI::EvaluateTaskState();
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

			if (!GetCameraPV())
			{
				return;
			}

			UpdateActorObjects();

			if (m_actorContext)
			{
				m_actorContext->Update();
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

			m_data->scene.PrepareForDraw();

			m_data->batchNoDepth.Draw(m_data->scene);
			m_data->batchDepth.Draw(m_data->scene);

			if (m_actorContext)
			{
				m_actorContext->Render(*m_data);
			}
		}

		void I3DIMain::OnOpen()
		{
		}

		void I3DIMain::OnClose()
		{
			m_actorContext.reset();
			m_data.reset();
		}

		void I3DIMain::UpdateActorObjects()
		{
			for (const auto& [i, e] : m_controller.GetData())
			{
				auto& actor = e.GetActor();

				if (!e.IsCellAttached() || !actor->formID)
				{
					auto it = m_data->actors.find(i);
					if (it != m_data->actors.end())
					{
						m_data->objectController.UnregisterObject(it->second);
						m_data->actors.erase(it);

						_DMESSAGE("lost (1): %.8X", i);
					}

					continue;
				}

				auto it = m_data->actors.find(i);

				if (it == m_data->actors.end())
				{
					auto object = std::make_shared<I3DIActorObject>(i);

					it = m_data->actors.try_emplace(i, object).first;

					m_data->objectController.RegisterObject(object);

					_DMESSAGE("acq: %.8X", i);
				}

				it->second->Update(e);
			}

			for (auto it = m_data->actors.begin(); it != m_data->actors.end();)
			{
				if (!m_controller.GetData().contains(it->first))
				{
					_DMESSAGE("lost (2): %.8X", it->first);

					m_data->objectController.UnregisterObject(it->second);

					it = m_data->actors.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		bool I3DIMain::GetCameraPV()
		{
			auto pc = PlayerCamera::GetSingleton();

			if (!pc)
			{
				return false;
			}

			auto camera = pc->GetNiCamera();
			if (!camera)
			{
				return false;
			}

			VectorMath::GetCameraPV(
				camera,
				m_data->scene.GetViewMatrix(),
				m_data->scene.GetProjectionMatrix());

			return true;
		}
	}
}