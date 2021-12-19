#pragma once

#include "../ConfigOverride.h"
#include "../ConfigOverrideDefault.h"

namespace IED
{
	class IJSONSerialization :
		public virtual ILog
	{
	public:
		bool ImportData(const fs::path& a_path, bool a_eraseTemporary);
		bool ExportData(const fs::path& a_path, stl::flag<Data::ConfigStoreSerializationFlags> a_flags);

		bool LoadConfigStore(const fs::path& a_path, Data::configStore_t& a_out) const;
		bool SaveConfigStore(const fs::path& a_path, const Data::configStore_t& a_data) const;

		inline constexpr const auto& JSGetLastException() const noexcept
		{
			return m_lastException;
		}

		Data::configStore_t CreateExportData(
			const Data::configStore_t& a_data,
			stl::flag<Data::ConfigStoreSerializationFlags> a_flags);

		FN_NAMEPROC("JSONSerialization");

	private:
		mutable except::descriptor m_lastException;

		virtual constexpr WCriticalSection& JSGetLock() noexcept = 0;
		virtual constexpr Data::configStore_t& JSGetConfigStore() noexcept = 0;
		virtual void JSOnDataImport() = 0;
	};
}  // namespace IED