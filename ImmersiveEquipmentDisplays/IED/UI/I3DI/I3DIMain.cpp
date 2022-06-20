#include "pch.h"

#include "I3DIMain.h"

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
				m_data = std::make_unique<Data>(
					rd.GetDevice().Get(),
					rd.GetContext().Get(),
					rd.GetSwapChainInfo());

				m_data->scene.EnableAlpha(true);
				m_data->scene.EnableDepth(true);

				m_data->batchNoDepth.EnableDepth(false);
			}

			ImGui::PushID(WINDOW_ID);

			auto& io = ImGui::GetIO();

			m_data->UpdateRay();

			if (!io.WantCaptureMouse)
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
								m_actorContext = std::make_unique<I3DIActorContext>(*m_data, m_controller, h->first);
							}
						}

						XMVECTOR origin;

						auto pos = m_data->actorSelector.GetPopupAnchorPoint(
							m_data->scene,
							h->second,
							origin);

						//ImGui::SetNextWindowPos({ pos.x, pos.y }, ImGuiCond_Always, { 0.5f, 1.f });

						m_data->commonPopup.SetLineWorldOrigin(origin);
						m_data->commonPopup.SetPosition(pos, { 0.5f, 1.f });

						char b[64];
						stl::snprintf(b, "###as_p_%.8X", h->first.get());

						m_data->commonPopup.Draw(b, *m_data, [&] {
							ImGui::Text("0x%.8X", h->first.get());
						});
					}
				}
			}

			if (m_actorContext)
			{
				if (m_actorContext->LastUpdateFailed())
				{
					m_actorContext.reset();
				}
				else
				{
					m_actorContext->Draw(*m_data);
				}
			}

			auto& ui = m_controller.GetConfigStore().settings.data.ui;

			if (io.KeyAlt ? ui.enableControlLock : !ui.enableControlLock)
			{
				ui.enableControlLock = !ui.enableControlLock;
				m_controller.UIGetRenderTask()->SetLock(ui.enableControlLock);
				Drivers::UI::EvaluateTaskState();
			}

			ImGui::PopID();
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

			m_data->actorSelector.Update(m_controller);

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
			m_data->actorSelector.Render(m_data->scene);

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