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

		virtual bool Load() = 0;
		virtual bool Save() = 0;

		void SetPath(const fs::path& a_path)
		{
			m_path    = a_path;
			m_pathStr = stl::wstr_to_str(a_path.wstring());
			m_name    = stl::wstr_to_str(a_path.stem().wstring());
		}

		void SetDescription(std::string a_text)  //
			noexcept(noexcept(m_desc.emplace(std::move(a_text))))
		{
			m_desc.emplace(std::move(a_text));
			MarkModified();
		}

		void ClearDescription()  //
			noexcept(noexcept(m_desc.reset()))
		{
			m_desc.reset();
			MarkModified();
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

		[[nodiscard]] constexpr bool IsModified() const noexcept
		{
			return m_modified;
		}

		constexpr void MarkModified() noexcept
		{
			m_modified = true;
		}

	protected:
		std::filesystem::path      m_path;
		stl::fixed_string          m_pathStr;
		stl::fixed_string          m_name;
		std::optional<std::string> m_desc;
		stl::flag<ProfileFlags>    m_flags{ ProfileFlags::kNone };
		bool                       m_modified{ false };

		T m_data;

		mutable except::descriptor m_lastExcept;
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
		virtual bool Save() override;

		constexpr bool HasParserErrors() const noexcept
		{
			return m_hasParserErrors;
		}

	private:
		bool SaveImpl();

		Serialization::ParserState m_state;
		bool                       m_hasParserErrors{ false };
	};

	template <class T>
	bool Profile<T>::Save()
	{
		return SaveImpl();
	}

	template <class T>
	bool Profile<T>::Load()
	{
		using namespace Serialization;

		try
		{
			if (this->m_path.empty())
			{
				throw parser_exception("bad path");
			}

			Json::Value root;

			ReadData(this->m_path, root);

			ParserState state;
			Parser<T>   parser(state);

			auto tmp = std::make_unique<T>();

			if (!parser.Parse(root, *tmp))
			{
				throw parser_exception("parser error");
			}

			m_hasParserErrors = parser.HasErrors();

			const auto                 flags = root.get("flags", stl::underlying(ProfileFlags::kNone)).asUInt();
			std::optional<std::string> desc;

			if (auto& d = root["desc"])
			{
				desc.emplace(d.asString());
			}

			this->m_flags    = std::move(flags);
			this->m_desc     = std::move(desc);
			this->m_data     = std::move(*tmp);
			this->m_modified = false;

			return true;
		}
		catch (const std::exception& e)
		{
			this->m_lastExcept = e;
			return false;
		}
	}

	template <class T>
	bool Profile<T>::SaveImpl()
	{
		using namespace Serialization;

		try
		{
			if (this->m_path.empty())
			{
				throw parser_exception("bad path");
			}

			Json::Value root;

			ParserState state;
			Parser<T>   parser(state);

			parser.Create(this->m_data, root);

			if (this->m_desc)
			{
				root["desc"] = *this->m_desc;
			}

			root["flags"] = this->m_flags.underlying();

			Serialization::WriteData(this->m_path, root);

			this->m_modified = false;

			return true;
		}
		catch (const std::exception& e)
		{
			this->m_lastExcept = e;
			return false;
		}
	}

}