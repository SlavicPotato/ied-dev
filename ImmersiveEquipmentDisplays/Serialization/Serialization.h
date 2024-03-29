#pragma once

#define PARSER_NOT_IMPL_STR "not implemented"

namespace IED
{
	namespace Serialization
	{
#if defined(SKMP_TRACED_EXCEPTIONS)
		class parser_exception :
			public stl::traced_exception
		{
		public:
			explicit parser_exception(
				const std::string&            a_message,
				boost::stacktrace::stacktrace a_trace = boost::stacktrace::stacktrace()) :
				stl::traced_exception(a_message, a_trace)
			{
			}
		};
#else
		using parser_exception = std::runtime_error;
#endif

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

			friend class ParserBase;

		public:
			[[nodiscard]] constexpr bool has_errors() const noexcept
			{
				return m_flags.test(ParserStateFlags::kHasErrors);
			}

		private:
			constexpr void clear() noexcept
			{
				m_flags = ParserStateFlags::kNone;
			}

			stl::flag<ParserStateFlags> m_flags{ ParserStateFlags::kNone };
		};

		class ParserBase :
			public ILog
		{
		public:
			ParserBase(ParserState& a_state) :
				m_state(a_state)
			{
			}

			[[nodiscard]] constexpr bool HasErrors() const noexcept
			{
				return m_state.has_errors();
			}

		protected:
			constexpr void SetHasErrors() const noexcept
			{
				m_state.m_flags.set(ParserStateFlags::kHasErrors);
			}

			constexpr void ClearState() const noexcept
			{
				m_state.clear();
			}

			ParserState& m_state;
		};

		template <template <class> class, class...>
		class ParserTemplateVA :
			public ParserBase
		{
		public:
			template <template <class> class X, class... Args>
			inline auto make() const
			{
				return ParserTemplateVA<X, Args...>(m_state);
			}

			using ParserBase::ParserBase;
		};

		template <class T>
		class Parser :
			public ParserBase
		{
		public:
			using ParserBase::ParserBase;

			template <class X>
			inline auto make() const
			{
				return Parser<X>(m_state);
			}

			void Create(const T& a_in, Json::Value& a_out) const;
			void Create(const T& a_in, Json::Value& a_out, bool a_arg) const;
			void Create(const T& a_in, Json::Value& a_out, std::uint32_t a_arg) const;

			bool Parse(const Json::Value& a_in, T& a_out) const;
			bool Parse(const Json::Value& a_in, T& a_out, const std::uint32_t a_version) const;
			bool Parse(const Json::Value& a_in, T& a_out, const std::uint32_t a_version, bool a_arg) const;
			bool Parse(const Json::Value& a_in, T& a_out, bool a_arg) const;
			bool Parse(const Json::Value& a_in, T& a_out, float a_arg) const;
		};

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
				{
					return false;
				}

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

		void        SafeCleanup(const fs::path& a_path);
		std::string SafeGetPath(const fs::path& a_path);
		void        CreateRootPath(const fs::path& a_path);

		bool FileExists(const fs::path& a_path);
		bool IsDirectory(const fs::path& a_path);

		static constexpr auto TMP_EXT = ".tmp";

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

			if (!ifs || !ifs.is_open())
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

					if (!ofs || !ofs.is_open())
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
			catch (...)
			{
				SafeCleanup(tmpPath);
				throw;
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
			std::uint32_t      a_current);

	}
}

#define JSON_PARSE_VERSION() \
	[[maybe_unused]] const auto version = ExtractVersion(a_in, CURRENT_VERSION);
