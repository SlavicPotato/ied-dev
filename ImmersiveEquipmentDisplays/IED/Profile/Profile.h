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
			m_desc.emplace(std::move(a_text));
		}

		inline void ClearDescription() noexcept
		{
			m_desc.clear();
		}

		[[nodiscard]] constexpr const auto& GetDescription() const noexcept
		{
			return m_desc;
		}

		[[nodiscard]] constexpr const auto& Name() const noexcept
		{
			return m_name;
		}

		[[nodiscard]] constexpr const auto& Path() const noexcept
		{
			return m_path;
		}

		[[nodiscard]] constexpr const auto& PathStr() const noexcept
		{
			return m_pathStr;
		}

		[[nodiscard]] constexpr T& Data() noexcept
		{
			return m_data;
		}

		[[nodiscard]] constexpr const T& Data() const noexcept
		{
			return m_data;
		}

		[[nodiscard]] constexpr const auto& GetLastException() const noexcept
		{
			return m_lastExcept;
		}

		[[nodiscard]] constexpr std::uint64_t GetID() const noexcept
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

			this->m_id = root.get("id", 0).asUInt64();

			if (auto& desc = root["desc"])
			{
				this->m_desc = desc.asString();
			}
			else
			{
				this->m_desc.reset();
			}

			this->m_data = std::move(*tmp);

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

			root["id"] = this->m_id;
			if (this->m_desc)
			{
				root["desc"] = *this->m_desc;
			}

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