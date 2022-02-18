#pragma once

namespace IED
{
	namespace Serialization
	{
		inline static constexpr auto PARSER_NOT_IMPL_STR = "Not implemented";

		enum class ParserStateFlags : std::uint32_t
		{
			kNone = 0,

			kHasErrors = 1u << 0,
		};

		DEFINE_ENUM_CLASS_BITWISE(ParserStateFlags);

		class ParserState
		{
			template <class T>
			friend class Parser;

		public:
			[[nodiscard]] inline constexpr bool has_errors() const noexcept
			{
				return m_flags.test(ParserStateFlags::kHasErrors);
			}

		private:
			inline constexpr void clear() noexcept
			{
				m_flags = ParserStateFlags::kNone;
			}

			stl::flag<ParserStateFlags> m_flags{ ParserStateFlags::kNone };
		};

		template <class T>
		class Parser : ILog
		{
		public:
			constexpr Parser(ParserState& a_state);

			void Create(const T& a_in, Json::Value& a_out) const;
			void Create(const T& a_in, Json::Value& a_out, bool a_arg) const;
			void Create(const T& a_in, Json::Value& a_out, std::uint32_t a_arg) const;

			bool Parse(const Json::Value& a_in, T& a_out) const;
			bool Parse(const Json::Value& a_in, T& a_out, const std::uint32_t a_version) const;
			bool Parse(const Json::Value& a_in, T& a_out, const std::uint32_t a_version, bool a_arg) const;
			bool Parse(const Json::Value& a_in, T& a_out, bool a_arg) const;

			void GetDefault(T& a_out) const;

			[[nodiscard]] inline constexpr bool HasErrors() const noexcept
			{
				return m_state.m_flags.test(ParserStateFlags::kHasErrors);
			}

		protected:
			inline constexpr void SetHasErrors() const noexcept
			{
				m_state.m_flags.set(ParserStateFlags::kHasErrors);
			}

			inline constexpr void ClearState() const noexcept
			{
				m_state.clear();
			}

		private:
			ParserState& m_state;
		};

		template <class T>
		inline constexpr Parser<T>::Parser(ParserState& a_state) :
			m_state(a_state)
		{
		}

		template <class T>
		void Parser<T>::Create(const T& a_in, Json::Value& a_out) const
		{
			static_assert(false, PARSER_NOT_IMPL_STR);
		}

		template <class T>
		void Parser<T>::Create(const T& a_in, Json::Value& a_out, bool a_arg) const
		{
			static_assert(false, PARSER_NOT_IMPL_STR);
		}

		template <class T>
		void Parser<T>::Create(
			const T&      a_in,
			Json::Value&  a_out,
			std::uint32_t a_arg) const
		{
			static_assert(false, PARSER_NOT_IMPL_STR);
		}

		template <class T>
		bool Parser<T>::Parse(const Json::Value& a_in, T& a_out) const
		{
			static_assert(false, PARSER_NOT_IMPL_STR);
		}

		template <class T>
		bool Parser<T>::Parse(
			const Json::Value&  a_in,
			T&                  a_out,
			const std::uint32_t a_version) const
		{
			static_assert(false, PARSER_NOT_IMPL_STR);
		}

		template <class T>
		bool Parser<T>::Parse(
			const Json::Value&  a_in,
			T&                  a_out,
			const std::uint32_t a_version,
			bool                a_arg) const
		{
			static_assert(false, PARSER_NOT_IMPL_STR);
		}

		template <class T>
		bool Parser<T>::Parse(
			const Json::Value& a_in,
			T&                 a_out,
			bool               a_arg) const
		{
			static_assert(false, PARSER_NOT_IMPL_STR);
		}

		template <class T>
		void Parser<T>::GetDefault(T& a_out) const
		{
			static_assert(false, PARSER_NOT_IMPL_STR);
		}

		template <std::size_t _Size>
		constexpr bool ParseFloatArray(
			const Json::Value& a_in,
			float (&a_out)[_Size])
		{
			if (!a_in.isArray())
			{
				return false;
			}

			if (a_in.size() != _Size)
			{
				return false;
			}

			for (std::uint32_t i = 0; i < _Size; i++)
			{
				auto& v = a_in[i];

				if (!v.isNumeric())
					return false;

				a_out[i] = v.asFloat();
			}

			return true;
		}

		bool ParseFloatArray(
			const Json::Value& a_in,
			float*             a_out,
			std::uint32_t      a_size);

		template <std::size_t _Size>
		constexpr void CreateFloatArray(
			const float (&a_in)[_Size],
			Json::Value& a_out)
		{
			for (auto& e : a_in)
			{
				a_out.append(e);
			}
		}

		void CreateFloatArray(
			const float*  a_in,
			std::uint32_t a_size,
			Json::Value&  a_out);

		void        SafeCleanup(const fs::path& a_path) noexcept;
		std::string SafeGetPath(const fs::path& a_path) noexcept;
		void        CreateRootPath(const fs::path& a_path);

		bool FileExists(const fs::path& a_path) noexcept;

		inline static constexpr auto TMP_EXT = ".tmp";

		template <class T>
		constexpr void ReadData(
			const fs::path& a_path,
			T&              a_root)
		{
			std::ifstream ifs;

			ifs.open(
				a_path,
				std::ifstream::in | std::ifstream::binary,
				_SH_DENYWR);

			if (!ifs.is_open())
			{
				throw std::system_error(
					errno,
					std::system_category(),
					SafeGetPath(a_path));
			}

			ifs >> a_root;
		}

		template <class Tp, class Td>
		constexpr void WriteData(
			const Tp& a_path,
			const Td& a_root,
			bool      a_styled = false)
		{
			fs::path tmpPath(a_path);

			tmpPath += TMP_EXT;

			// yes this should throw here
			CreateRootPath(tmpPath);

			try
			{
				{
					std::ofstream ofs;

					ofs.open(
						tmpPath,
						std::ofstream::out | std::ofstream::binary | std::ofstream::trunc,
						_SH_DENYWR);

					if (!ofs.is_open())
					{
						throw std::system_error(
							errno,
							std::system_category(),
							SafeGetPath(tmpPath));
					}

					if constexpr (std::is_convertible_v<Td, Json::Value>)
					{
						if (!a_styled)
						{
							Json::StreamWriterBuilder builder;
							builder["indentation"]  = "";
							builder["commentStyle"] = "None";

							std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
							writer->write(a_root, std::addressof(ofs));
						}
						else
						{
							ofs << a_root;
						}
					}
					else
					{
						ofs << a_root;
					}
				}

				fs::rename(tmpPath, a_path);
			}
			catch (const std::exception& e)
			{
				SafeCleanup(tmpPath);
				throw e;
			}
		}

		template <class T>
		struct parserDesc_t
		{
			const char* member;
			T&          data;
		};

		template <class T>
		struct parserDescConst_t
		{
			const char* member;
			const T&    data;
		};

		bool ParseVersion(
			const Json::Value& a_in,
			const char*        a_key,
			std::uint32_t&     a_out);

		std::uint32_t ExtractVersion(
			const Json::Value& a_in,
			std::uint32_t      a_current,
			const char*        a_func);

	}
}

#define JSON_PARSE_VERSION() \
	auto version = ExtractVersion(a_in, CURRENT_VERSION, __FUNCTION__);
