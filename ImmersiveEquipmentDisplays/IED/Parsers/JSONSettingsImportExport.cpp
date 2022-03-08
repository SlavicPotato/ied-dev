#include "pch.h"

#include "JSONSettingsImportExport.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::ImportExport>::Parse(
			const Json::Value&                 a_in,
			Data::SettingHolder::ImportExport& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			a_out.exportFlags = static_cast<Data::ConfigStoreSerializationFlags>(
				data.get("export_flags", stl::underlying(Data::ConfigStoreSerializationFlags::kAll)).asUInt());

			a_out.importFlags = static_cast<ImportFlags>(
				data.get("import_flags", stl::underlying(ImportFlags::kEraseTemporary)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::ImportExport>::Create(
			const Data::SettingHolder::ImportExport& a_data,
			Json::Value&                             a_out) const
		{
			auto& data = a_out["data"];

			data["export_flags"] = stl::underlying(a_data.exportFlags.value);
			data["import_flags"] = stl::underlying(a_data.importFlags.value);

			a_out["version"] = CURRENT_VERSION;
		}

	}
}