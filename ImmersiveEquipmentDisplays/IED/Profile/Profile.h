#pragma once

#include "Serialization/Serialization.h"

namespace IED
{
	template <class T>
	class ProfileBase
	{
	public:
		using base_type = T;

		ProfileBase() = default;

		virtual bool Load()                              = 0;
		virtual bool Save(const T& a_data, bool a_store) = 0;
		virtual bool Save(T&& a_data, bool a_store)      = 0;
		virtual void SetDefaults() noexcept              = 0;

		virtual bool Save()
		{
			return Save(m_data, false);
		}

		void SetPath(const fs::path& a_path)
		{
			m_path    = a_path;
			m_pathStr = str_conv::wstr_to_str(a_path.wstring());
			m_name    = str_conv::wstr_to_str(a_path.stem().wstring());
		}

		inline void SetDescription(const std::string& a_text) noexcept
		{
			m_desc = a_text;
		}

		inline void SetDescription(std::string&& a_text) noexcept
		{
			m_desc = std::move(a_text);
		}

		inline void ClearDescription() noexcept
		{
			m_desc.clear();
		}

		[[nodiscard]] inline constexpr const auto& GetDescription() const noexcept
		{
			return m_desc;
		}

		[[nodiscard]] inline constexpr const auto& Name() const noexcept
		{
			return m_name;
		}

		[[nodiscard]] inline constexpr const auto& Path() const noexcept
		{
			return m_path;
		}

		[[nodiscard]] inline constexpr const auto& PathStr() const noexcept
		{
			return m_pathStr;
		}

		[[nodiscard]] inline constexpr T& Data() noexcept
		{
			return m_data;
		}

		[[nodiscard]] inline constexpr const T& Data() const noexcept
		{
			return m_data;
		}

		[[nodiscard]] inline constexpr const auto& GetLastException() const noexcept
		{
			return m_lastExcept;
		}

		[[nodiscard]] inline constexpr std::uint64_t GetID() const noexcept
		{
			return m_id;
		}

		inline void SetID(std::uint64_t a_id) noexcept
		{
			m_id = a_id;
		}

	protected:
		std::filesystem::path m_path;
		stl::fixed_string     m_pathStr;
		stl::fixed_string     m_name;

		std::uint64_t              m_id{ 0 };
		std::optional<std::string> m_desc;

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

		virtual void SetDefaults() noexcept override;

		inline constexpr bool HasParserErrors() const noexcept
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
			if (m_path.empty())
			{
				throw std::exception("bad path");
			}

			std::ifstream fs;
			fs.open(m_path, std::ifstream::in | std::ifstream::binary);

			if (!fs.is_open())
			{
				throw std::system_error(
					errno,
					std::system_category(),
					Serialization::SafeGetPath(m_path));
			}

			Json::Value root;
			fs >> root;

			Serialization::ParserState state;
			Serialization::Parser<T>   parser(state);

			T tmp;

			if (!parser.Parse(root, tmp))
			{
				throw std::exception("parser error");
			}

			m_hasParserErrors = parser.HasErrors();

			m_id = root.get("id", 0).asUInt64();

			if (auto& desc = root["desc"])
			{
				m_desc = desc.asString();
			}
			else
			{
				m_desc.reset();
			}

			m_data = std::move(tmp);

			return true;
		}
		catch (const std::exception& e)
		{
			m_lastExcept = e;
			return false;
		}
	}

	template <class T>
	void Profile<T>::SetDefaults() noexcept
	{
		Serialization::ParserState state;
		Serialization::Parser<T>   parser(state);

		parser.GetDefault(m_data);
	}

	template <class T>
	template <class Td>
	bool Profile<T>::SaveImpl(
		Td&& a_data,
		bool a_store)
	{
		try
		{
			if (m_path.empty())
			{
				throw std::exception("bad path");
			}

			Json::Value root;

			Serialization::ParserState state;
			Serialization::Parser<T>   parser(state);

			parser.Create(a_data, root);

			root["id"] = m_id;
			if (m_desc)
			{
				root["desc"] = *m_desc;
			}

			Serialization::WriteData(m_path, root);

			if (a_store)
			{
				m_data = std::forward<Td>(a_data);
			}

			return true;
		}
		catch (const std::exception& e)
		{
			m_lastExcept = e;
			return false;
		}
	}

}