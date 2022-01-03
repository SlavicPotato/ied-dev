#include "pch.h"

#include "../UICommon.h"
#include "UIExportFilterWidget.h"

namespace IED
{
	namespace UI
	{
		bool UIExportFilterWidget::DrawExportFilters(
			stl::flag<Data::ConfigStoreSerializationFlags>& a_flags)
		{
			using namespace Data;

			bool result = false;

			ImGui::PushID("export_filter");

			ImGui::Spacing();

			ImGui::TextUnformatted("Equipment:");

			ImGui::Indent();
			ImGui::Spacing();

			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Global: ");

			ImGui::SameLine();

			ImGui::PushID("slot_global");

			result |= ImGui::CheckboxFlagsT(
				"Player##slot",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kSlotGlobalPlayer));

			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				"NPC##slot",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kSlotGlobalNPC));

			ImGui::PopID();

			result |= ImGui::CheckboxFlagsT(
				"Actor##slot",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kSlotActor));

			result |= ImGui::CheckboxFlagsT(
				"NPC##slot",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kSlotNPC));

			result |= ImGui::CheckboxFlagsT(
				"Race##slot",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kSlotRace));

			ImGui::Unindent();

			ImGui::TextUnformatted("Custom:");

			ImGui::Indent();
			ImGui::Spacing();

			result |= ImGui::CheckboxFlagsT(
				"Global##custom",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kCustomGlobal));

			result |= ImGui::CheckboxFlagsT(
				"Actor##custom",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kCustomActor));

			bool disabled = a_flags.test(ConfigStoreSerializationFlags::kCustomActor);

			UICommon::PushDisabled(disabled);

			ImGui::SameLine();

			if (!disabled)
			{
				result |= ImGui::CheckboxFlagsT(
					"Player##custom",
					stl::underlying(std::addressof(a_flags.value)),
					stl::underlying(ConfigStoreSerializationFlags::kCustomPlayer));
			}
			else
			{
				bool dummy = true;
				ImGui::Checkbox("Player##custom", std::addressof(dummy));
			}

			UICommon::PopDisabled(disabled);

			result |= ImGui::CheckboxFlagsT(
				"NPC##custom",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kCustomNPC));

			result |= ImGui::CheckboxFlagsT(
				"Race##custom",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kCustomRace));

			ImGui::Unindent();

			ImGui::TextUnformatted("Gear positioning:");

			ImGui::Indent();
			ImGui::Spacing();

			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Global: ");

			ImGui::SameLine();

			ImGui::PushID("node_global");

			result |= ImGui::CheckboxFlagsT(
				"Player##node",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kNodeOverrideGlobalPlayer));
			
			ImGui::SameLine();

			result |= ImGui::CheckboxFlagsT(
				"NPC##node",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kNodeOverrideGlobalNPC));

			ImGui::PopID();

			result |= ImGui::CheckboxFlagsT(
				"Actor##node",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kNodeOverrideActor));

			disabled = a_flags.test(ConfigStoreSerializationFlags::kNodeOverrideActor);

			UICommon::PushDisabled(disabled);

			ImGui::SameLine();

			if (!disabled)
			{
				result |= ImGui::CheckboxFlagsT(
					"Player##node",
					stl::underlying(std::addressof(a_flags.value)),
					stl::underlying(ConfigStoreSerializationFlags::kNodeOverridePlayer));
			}
			else
			{
				bool dummy = true;
				ImGui::Checkbox("Player##node", std::addressof(dummy));
			}

			UICommon::PopDisabled(disabled);

			result |= ImGui::CheckboxFlagsT(
				"NPC##node",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kNodeOverrideNPC));

			result |= ImGui::CheckboxFlagsT(
				"Race##node",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kNodeOverrideRace));

			ImGui::Unindent();

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			result |= ImGui::CheckboxFlagsT(
				"Toggle all##default",
				stl::underlying(std::addressof(a_flags.value)),
				stl::underlying(ConfigStoreSerializationFlags::kAll));

			ImGui::Spacing();

			ImGui::PopID();

			return result;
		}
	}
}