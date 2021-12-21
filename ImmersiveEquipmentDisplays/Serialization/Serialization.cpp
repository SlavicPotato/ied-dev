#include "pch.h"

#include "Serialization.h"

namespace IED
{
	namespace Serialization
	{
		bool ParseFloatArray(
			const Json::Value& a_in,
			float* a_out,
			std::uint32_t a_size)
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
			const float* a_in,
			std::uint32_t a_size,
			Json::Value& a_out)
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
				return a_path.string();
			}
			catch (...)
			{
				return {};
			}
		}

		void CreateRootPath(const std::filesystem::path& a_path)
		{
			auto form = a_path.parent_path();

			if (!std::filesystem::exists(form))
			{
				if (!std::filesystem::create_directories(form))
				{
					throw std::exception("Couldn't create base directory");
				}
			}
			else if (!std::filesystem::is_directory(form))
			{
				throw std::exception("Root path is not a directory");
			}
		}
	}
}