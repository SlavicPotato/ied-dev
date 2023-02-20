#pragma once

#include "ExportFlags.h"
#include "IED/ConfigSerializationFlags.h"
#include "IED/ConfigStore.h"
#include "ImportFlags.h"

#include "Serialization/Serialization.h"

namespace IED
{
	class IJSONSerialization :
		public virtual ILog
	{
	public:
		bool ImportData(
			const Data::configStore_t&                     a_in,
			stl::flag<ImportFlags>                         a_flags,
			stl::flag<Data::ConfigStoreSerializationFlags> a_serFlags);

		bool ExportData(
			const fs::path&                                a_path,
			stl::flag<ExportFlags>                         a_exportFlags,
			stl::flag<Data::ConfigStoreSerializationFlags> a_flags);

		bool LoadConfigStore(const fs::path& a_path, Data::configStore_t& a_out) const;
		bool LoadConfigStore(const fs::path& a_path, Data::configStore_t& a_out, Serialization::ParserState& a_state) const;
		bool SaveConfigStore(const fs::path& a_path, const Data::configStore_t& a_data) const;

		constexpr const auto& JSGetLastException() const noexcept
		{
			return m_lastException;
		}

		std::unique_ptr<Data::configStore_t> CreateFilteredConfigStore(
			const Data::configStore_t&                     a_data,
			stl::flag<ExportFlags>                         a_exportFlags,
			stl::flag<Data::ConfigStoreSerializationFlags> a_flags);

		FN_NAMEPROC("JSONSerialization");

	private:
		bool DoImportOverwrite(
			Data::configStore_t&&                   a_in,
			[[maybe_unused]] stl::flag<ImportFlags> a_flags);

		bool DoImportMerge(
			Data::configStore_t&&                   a_in,
			[[maybe_unused]] stl::flag<ImportFlags> a_flags);

		virtual constexpr stl::recursive_mutex& JSGetLock() noexcept                = 0;
		virtual SKMP_143_CONSTEXPR Data::configStore_t& JSGetConfigStore() noexcept = 0;
		virtual void                                    JSOnDataImport()            = 0;

		mutable except::descriptor m_lastException;
	};
}