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
			{
				return false;
			}

			if (a_in.size() != a_size)
			{
				return false;
			}

			for (std::uint32_t i = 0; i < a_size; i++)
			{
				auto& v = a_in[i];

				if (!v.isNumeric())
				{
					return false;
				}

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

		void SafeCleanup(const fs::path& a_path)
		{
			try
			{
				fs::remove(a_path);
			}
			catch (...)
			{
			}
		}

		std::string SafeGetPath(const fs::path& a_path)
		{
			try
			{
				return stl::wstr_to_str(a_path.wstring());
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
					throw std::runtime_error("couldn't create base directory");
				}
			}
			else if (!std::filesystem::is_directory(path))
			{
				throw std::runtime_error("root path is not a directory");
			}
		}

		bool FileExists(const fs::path& a_path)
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

		bool IsDirectory(const fs::path& a_path)
		{
			try
			{
				return std::filesystem::is_directory(a_path);
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
			std::uint32_t      a_current)
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				throw parser_exception("bad version data");
			}

			if (version > a_current)
			{
				char buf[64];
				stl::snprintf(
					buf,
					"unsupported version (%u > %u) ",
					version,
					a_current);

				throw parser_exception(buf);
			}

			return version;
		}
	}
}
