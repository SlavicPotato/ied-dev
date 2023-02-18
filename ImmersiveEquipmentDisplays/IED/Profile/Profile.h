#pragma once

#include "Serialization/Serialization.h"

namespace IED
{
	enum class ProfileFlags : std::uint32_t
	{
		kNone = 0,

		kMergeOnly = 1u << 0
	};

	DEFINE_ENUM_CLASS_BITWISE(ProfileFlags);

	template <class T>
	class ProfileBase
	{
	public:
		using base_type = T;

		ProfileBase() = default;

		virtual bool Load()                              = 0;
		virtual bool Save(const T& a_data, bool a_store) = 0;
		virtual bool Save(T&& a_data, bool a_store)      = 0;

		virtual bool Save()
		{
			return Save(m_data, false);
		}

		void SetPath(const fs::path& a_path)
		{
			m_path    = a_path;
			m_pathStr = stl::wstr_to_str(a_path.wstring());
			m_name    = stl::wstr_to_str(a_path.stem().wstring());
		}

		inline void SetDescription(const std::string& a_text) noexcept
		{
			m_desc = a_text;
		}

		inline void SetDescription(std::string&& a_text) noexcept
		{
			m_desc.emplace(std::move(a_text));
		}

		inline void ClearDescription() noexcept
		{
			m_desc.reset();
		}

		[[nodiscard]] constexpr auto& GetDescription() const noexcept
		{
			return m_desc;
		}

		[[nodiscard]] constexpr auto& Name() const noexcept
		{
			return m_name;
		}

		[[nodiscard]] constexpr auto& Path() const noexcept
		{
			return m_path;
		}

		[[nodiscard]] constexpr auto& PathStr() const noexcept
		{
			return m_pathStr;
		}

		[[nodiscard]] constexpr auto& Data() noexcept
		{
			return m_data;
		}

		[[nodiscard]] constexpr auto& Data() const noexcept
		{
			return m_data;
		}

		[[nodiscard]] constexpr auto& GetLastException() const noexcept
		{
			return m_lastExcept;
		}

		[[nodiscard]] constexpr auto& GetFlags() noexcept
		{
			return m_flags;
		}

		[[nodiscard]] constexpr auto& GetFlags() const noexcept
		{
			return m_flags;
		}

	protected:
		std::filesystem::path      m_path;
		stl::fixed_string          m_pathStr;
		stl::fixed_string          m_name;
		std::optional<std::string> m_desc;
		stl::flag<ProfileFlags>    m_flags{ ProfileFlags::kNone };

		T m_data;

		except::descriptor m_lastExcept;
	};

	template <class T>
	class Profile :
		public ProfileBase<T>
	{
	public:
		using ProfileBase<T>::ProfileBase;
		using ProfileBase<T>::Save;

		virtual ~Profile() noexcept = default;

		virtual bool Load() override;
		virtual bool Save(const T& a_data, bool a_store) override;
		virtual bool Save(T&& a_data, bool a_store) override;

		constexpr bool HasParserErrors() const noexcept
		{
			return m_hasParserErrors;
		}

	private:
		template <class Td>
		bool SaveImpl(Td&& a_data, bool a_store);

		Serialization::ParserState m_state;
		bool                       m_hasParserErrors{ false };
	};

	template <class T>
	bool Profile<T>::Save(const T& a_data, bool a_store)
	{
		return SaveImpl(a_data, a_store);
	}

	template <class T>
	bool Profile<T>::Save(T&& a_data, bool a_store)
	{
		return SaveImpl(std::move(a_data), a_store);
	}

	template <class T>
	bool Profile<T>::Load()
	{
		try
		{
			if (this->m_path.empty())
			{
				throw std::exception("bad path");
			}

			std::ifstream fs;
			fs.open(this->m_path, std::ifstream::in | std::ifstream::binary);

			if (!fs || !fs.is_open())
			{
				throw std::system_error(
					errno,
					std::system_category(),
					Serialization::SafeGetPath(this->m_path));
			}

			Json::Value root;
			fs >> root;

			Serialization::ParserState state;
			Serialization::Parser<T>   parser(state);

			auto tmp = std::make_unique<T>();

			if (!parser.Parse(root, *tmp))
			{
				throw std::exception("parser error");
			}

			m_hasParserErrors = parser.HasErrors();

			const auto                 flags = root.get("flags", stl::underlying(ProfileFlags::kNone)).asUInt();
			std::optional<std::string> desc;

			if (auto& d = root["desc"])
			{
				desc.emplace(d.asString());
			}

			this->m_data  = std::move(*tmp);
			this->m_flags = std::move(flags);
			this->m_desc  = std::move(desc);

			return true;
		}
		catch (const std::exception& e)
		{
			this->m_lastExcept = e;
			return false;
		}
	}

	template <class T>
	template <class Td>
	bool Profile<T>::SaveImpl(
		Td&& a_data,
		bool a_store)
	{
		try
		{
			if (this->m_path.empty())
			{
				throw std::exception("bad path");
			}

			Json::Value root;

			Serialization::ParserState state;
			Serialization::Parser<T>   parser(state);

			parser.Create(a_data, root);

			if (this->m_desc)
			{
				root["desc"] = *this->m_desc;
			}

			root["flags"] = this->m_flags.underlying();

			Serialization::WriteData(this->m_path, root);

			if (a_store)
			{
				this->m_data = std::forward<Td>(a_data);
			}

			return true;
		}
		catch (const std::exception& e)
		{
			this->m_lastExcept = e;
			return false;
		}
	}

}