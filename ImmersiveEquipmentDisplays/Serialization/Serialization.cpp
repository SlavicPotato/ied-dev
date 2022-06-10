#include "pch.h"

#include "Serialization.h"

namespace IED
{
	namespace Serialization
	{
		bool ParseFloatArray(
			const Json::Value& a_in,
			float*             a_out,
			std::uint32_t      a_size)
		{
			if (!a_in.isArray())
				return false;

			if (a_in.size() != a_size)
				return false;

			for (std::uint32_t i = 0; i < a_size; i++)
			{
				auto& v = a_in[i];

				if (!v.isNumeric())
					return false;

				a_out[i] = v.asFloat();
			}

			return true;
		}

		void CreateFloatArray(
			const float*  a_in,
			std::uint32_t a_size,
			Json::Value&  a_out)
		{
			for (std::uint32_t i = 0; i < a_size; i++)
			{
				a_out.append(a_in[i]);
			}
		}

		void SafeCleanup(const fs::path& a_path) noexcept
		{
			try
			{
				fs::remove(a_path);
			}
			catch (...)
			{
			}
		}

		std::string SafeGetPath(const fs::path& a_path) noexcept
		{
			try
			{
				return str_conv::wstr_to_str(a_path.wstring());
			}
			catch (...)
			{
				return {};
			}
		}

		void CreateRootPath(const fs::path& a_path)
		{
			auto path = a_path.parent_path();

			if (!std::filesystem::exists(path))
			{
				if (!std::filesystem::create_directories(path))
				{
					throw std::exception("Couldn't create base directory");
				}
			}
			else if (!std::filesystem::is_directory(path))
			{
				throw std::exception("Root path is not a directory");
			}
		}

		bool FileExists(const fs::path& a_path) noexcept
		{
			try
			{
				return std::filesystem::exists(a_path);
			}
			catch (...)
			{
				return false;
			}
		}

		bool ParseVersion(
			const Json::Value& a_in,
			const char*        a_key,
			std::uint32_t&     a_out)
		{
			if (a_in.isMember(a_key))
			{
				auto& v = a_in[a_key];

				if (!v.isIntegral())
				{
					return false;
				}

				a_out = v.asUInt();
			}
			else
			{
				a_out = 0;
			}

			return true;
		}

		std::uint32_t ExtractVersion(
			const Json::Value& a_in,
			std::uint32_t      a_current,
			const char*        a_func)
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				gLog.Error(
					"%s: bad version data",
					a_func);

				throw std::exception("bad version data");
			}

			if (version > a_current)
			{
				gLog.Error(
					"%s: unsupported version (%u > %u) ",
					a_func,
					version,
					a_current);

				throw std::exception("unsupported version");
			}

			return version;
		}
	}
}
