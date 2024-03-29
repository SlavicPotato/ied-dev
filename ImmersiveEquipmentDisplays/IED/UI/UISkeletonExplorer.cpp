#include "pch.h"

#include "UISkeletonExplorer.h"

#include "IED/Controller/Controller.h"

#include "UISkeletonExplorerStrings.h"
#include "Widgets/UIWidgetCommonStrings.h"

#include "UICommon.h"

namespace IED
{
	namespace UI
	{
		UISkeletonExplorer::UISkeletonExplorer(
			Controller& a_controller) :
			UIMiscTextInterface(a_controller),
			UIActorList<ActorSkeletonData>(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			m_nodeFilter(true),
			m_controller(a_controller)
		{
		}

		void UISkeletonExplorer::Draw()
		{
			SetWindowDimensions(75.0f, 650.0f);

			if (ImGui::Begin(
					UIL::LS<UIWidgetCommonStrings, 3>(
						UIWidgetCommonStrings::SkeletonExplorer,
						WINDOW_ID),
					GetOpenState()))
			{
				ImGui::Spacing();

				ListDraw();

				ImGui::Spacing();
				ImGui::Separator();

				DrawOptions();

				ImGui::Separator();

				DrawFilterTree();

				ImGui::Separator();
				ImGui::Spacing();

				DrawSkeletonTreePanel();
			}

			ImGui::End();
		}

		void UISkeletonExplorer::OnOpen()
		{
			Reset();
		}

		void UISkeletonExplorer::OnClose()
		{
			Reset();
		}

		void UISkeletonExplorer::Reset()
		{
			ListReset();
		}

		void UISkeletonExplorer::DrawOptions()
		{
			ImGui::PushID("opt");

			ImGui::Spacing();

			auto& settings = m_controller.GetSettings();

			if (settings.mark_if(
					ImGui::Checkbox(
						UIL::LS(UISkeletonExplorerStrings::LoadedSkeleton),
						std::addressof(settings.data.ui.skeletonExplorer.showLoadedSkeleton))))
			{
				if (m_listCurrent)
				{
					m_listCurrent->data = GetData(m_listCurrent->handle);
				}
			}

			ImGui::SameLine();

			if (m_listCurrent && m_listCurrent->handle == Data::IData::GetPlayerRefID())
			{
				if (settings.mark_if(
						ImGui::Checkbox(
							UIL::LS(UISkeletonExplorerStrings::FirstPersonSkeleton),
							std::addressof(settings.data.ui.skeletonExplorer.firstPerson))))
				{
					m_listCurrent->data = GetData(m_listCurrent->handle);
				}
			}

			ImGui::Spacing();

			ImGui::PopID();
		}

		void UISkeletonExplorer::DrawFilterTree()
		{
			if (TreeEx(
					"flt_tree",
					false,
					"%s",
					UIL::LS(CommonStrings::Filter)))
			{
				auto& settings = m_controller.GetSettings();

				ImGui::PushItemWidth(ImGui::GetFontSize() * -13.0f);

				ImGui::Spacing();

				m_nodeFilter.Draw();

				ImGui::SameLine();

				settings.mark_if(
					ImGui::Checkbox(
						UIL::LS(UISkeletonExplorerStrings::FilterShowChildNodes),
						std::addressof(settings.data.ui.skeletonExplorer.filterShowChildNodes)));

				ImGui::Spacing();

				ImGui::PopItemWidth();

				ImGui::TreePop();
			}
		}

		void UISkeletonExplorer::DrawSkeletonDataHeader(
			const SI_Root& a_data)
		{
			bool r = false;

			ImGui::PushID("header");

			if (!a_data.path.empty())
			{
				ImGui::Text(
					"%s:",
					UIL::LS(CommonStrings::Path));

				ImGui::SameLine();

				TextWrappedCopyable(
					"%s",
					a_data.path.c_str());

				r = true;
			}

			if (a_data.skeletonID)
			{
				if (auto& id = a_data.skeletonID->id())
				{
					ImGui::Text("%s:", UIL::LS(CommonStrings::ID));
					ImGui::SameLine();

					TextCopyable("%d", *id);

					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
					ImGui::SameLine();
				}

				ImGui::Text("%s:", UIL::LS(CommonStrings::Signature));
				ImGui::SameLine();

				TextCopyable(
					"%llu",
					a_data.skeletonID->signature());

				r = true;
			}

			if (r)
			{
				ImGui::Separator();
				ImGui::Spacing();
			}

			ImGui::PopID();
		}

		void UISkeletonExplorer::DrawSkeletonTreePanel()
		{
			if (!m_listCurrent)
			{
				return;
			}

			auto& data = m_listCurrent->data;
			if (!data.data)
			{
				return;
			}

			const stl::lock_guard lock(data.data->lock);

			ImGui::PushID("skel_tree_panel");

			DrawSkeletonDataHeader(*data.data);

			if (ImGui::BeginChild(
					"skel_tree",
					{ -1.0f, 0.0f },
					false,
					ImGuiWindowFlags_HorizontalScrollbar))
			{
				DrawSkeletonTree(
					m_listCurrent->handle,
					data);
			}

			ImGui::EndChild();

			ImGui::PopID();
		}

		void UISkeletonExplorer::DrawSkeletonTree(
			Game::FormID             a_handle,
			const ActorSkeletonData& a_data)
		{
			if (!a_data.data->initialized)
			{
				ImGui::TextUnformatted(UIL::LS(UISkeletonExplorerStrings::CouldNotUpdate));
				ImGui::Spacing();

				return;
			}

			UpdateSkeletonData(a_handle, a_data);

			if (!a_data.data->succeeded)
			{
				ImGui::TextColored(
					UICommon::g_colorWarning,
					"%s",
					UIL::LS(UISkeletonExplorerStrings::DataUnavailable));

				ImGui::Separator();
				ImGui::Spacing();
			}

			assert(a_data.data->actor == a_handle);

			DrawRoot(a_handle, *a_data.data);
		}

		void UISkeletonExplorer::UpdateSkeletonData(
			Game::FormID             a_handle,
			const ActorSkeletonData& a_data)
		{
			const auto& settings = m_controller
			                           .GetSettings()
			                           .data.ui.skeletonExplorer;

			if (settings.showLoadedSkeleton)
			{
				if (IPerfCounter::delta_us(
						a_data.data->lastUpdate,
						IPerfCounter::Query()) > 100000)
				{
					ISkeletonInfo::QueueSkeletonInfoLookup(
						a_handle,
						settings.firstPerson,
						true,
						a_data.data);
				}
			}
		}

		void UISkeletonExplorer::DrawRoot(
			Game::FormID   a_handle,
			const SI_Root& a_data)
		{
			ImGui::PushID("root");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6.f, 6.0f });

			DrawObject(a_data, a_data.object, 0, false);

			ImGui::PopStyleVar();

			ImGui::PopID();
		}

		void UISkeletonExplorer::DrawObject(
			const SI_Root&     a_data,
			const SI_NiObject& a_object,
			std::uint32_t      a_index,
			bool               a_disableFilter)
		{
			ImGui::PushID(a_index);

			auto flags = ImGuiTreeNodeFlags_DefaultOpen |
			             ImGuiTreeNodeFlags_SpanAvailWidth;

			if (a_object.children.empty())
			{
				flags |= ImGuiTreeNodeFlags_Bullet;
			}

			if (a_disableFilter || m_nodeFilter.Test(a_object.name))
			{
				const auto& settings = m_controller
				                           .GetSettings()
				                           .data.ui.skeletonExplorer;

				if (settings.filterShowChildNodes)
				{
					a_disableFilter = true;
				}

				bool result;

				if (a_object.parent)
				{
					result = ImGui::TreeNodeEx(
						"",
						flags,
						"%s [%s] [%s] [%.8X]",
						a_object.name.c_str(),
						a_object.parent->name.c_str(),
						a_object.type.c_str(),
						a_object.flags);
				}
				else
				{
					result = ImGui::TreeNodeEx(
						"",
						flags,
						"%s [%s] [%.8X]",
						a_object.name.c_str(),
						a_object.type.c_str(),
						a_object.flags);
				}

				if (result)
				{
					DrawObjectInfo(
						a_object,
						a_data.isLoadedData);

					DrawChildNodes(
						a_data,
						a_object,
						a_disableFilter);

					ImGui::TreePop();
				}
			}
			else
			{
				DrawChildNodes(
					a_data,
					a_object,
					a_disableFilter);
			}

			ImGui::PopID();
		}

		void UISkeletonExplorer::DrawChildNodes(
			const SI_Root&     a_data,
			const SI_NiObject& a_object,
			bool               a_disableFilter)
		{
			std::uint32_t i = 0;

			for (auto& e : a_object.children)
			{
				DrawObject(a_data, e, i, a_disableFilter);
				i++;
			}
		}

		void UISkeletonExplorer::DrawObjectInfo(
			const SI_NiObject& a_data,
			bool               a_drawWorld)
		{
			ImGui::Spacing();
			ImGui::Indent();

			ImGui::PushStyleColor(
				ImGuiCol_Text,
				UICommon::g_colorGreyed);

			TextCopyable(
				"Local: P:[%f %f %f] R:[%f %f %f] S:[%f]",
				a_data.local.pos.x,
				a_data.local.pos.y,
				a_data.local.pos.z,
				a_data.local.rot.x,
				a_data.local.rot.y,
				a_data.local.rot.z,
				a_data.local.scale);

			if (a_drawWorld)
			{
				TextCopyable(
					"World: P:[%f %f %f] R:[%f %f %f] S:[%f]",
					a_data.world.pos.x,
					a_data.world.pos.y,
					a_data.world.pos.z,
					a_data.world.rot.x,
					a_data.world.rot.y,
					a_data.world.rot.z,
					a_data.world.scale);
			}

			ImGui::PopStyleColor();

			ImGui::Unindent();
			ImGui::Spacing();
		}

		ActorSkeletonData UISkeletonExplorer::GetData(Game::FormID a_handle)
		{
			ActorSkeletonData result;

			const auto& settings = m_controller
			                           .GetSettings()
			                           .data.ui.skeletonExplorer;

			ISkeletonInfo::QueueSkeletonInfoLookup(
				a_handle,
				settings.firstPerson,
				settings.showLoadedSkeleton,
				result.data);

			return result;
		}

		void UISkeletonExplorer::ListResetAllValues(Game::FormID a_handle)
		{
		}

		void UISkeletonExplorer::OnListOptionsChange()
		{
			m_controller.GetSettings().mark_dirty();
		}

		Data::SettingHolder::EditorPanelActorSettings& UISkeletonExplorer::GetActorSettings() const
		{
			return m_controller.GetSettings().data.ui.skeletonExplorer.actorSettings;
		}

		UIData::UICollapsibleStates& UISkeletonExplorer::GetCollapsibleStatesData()
		{
			return m_controller.GetSettings().data.ui.skeletonExplorer.colStates;
		}

		void UISkeletonExplorer::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
		}
	}
}