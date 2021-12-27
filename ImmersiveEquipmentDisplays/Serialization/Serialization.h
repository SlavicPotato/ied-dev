#pragma once

namespace IED
{
	namespace Serialization
	{
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
			inline void clear() noexcept
			{
				m_flags = ParserStateFlags::kNone;
			}

			stl::flag<ParserStateFlags> m_flags{ ParserStateFlags::kNone };
		};

		template <class T>
		class Parser : ILog
		{
		public:
			Parser(ParserState& a_state);

			void Create(const T& a_in, Json::Value& a_out) const;
			void Create(const T& a_in, Json::Value& a_out, bool a_arg) const;
			void Create(const T& a_in, Json::Value& a_out, std::uint32_t a_arg) const;

			bool Parse(const Json::Value& a_in, T& a_out) const;
			bool Parse(const Json::Value& a_in, T& a_out, const std::uint32_t a_version) const;
			bool Parse(const Json::Value& a_in, T& a_out, const std::uint32_t a_version, bool a_arg) const;
			bool Parse(const Json::Value& a_in, T& a_out, bool a_arg) const;

			void GetDefault(T& a_out) const;

			inline constexpr bool HasErrors() const noexcept
			{
				return m_state.m_flags.test(ParserStateFlags::kHasErrors);
			}

		protected:
			inline void SetHasErrors() const noexcept
			{
				m_state.m_flags.set(ParserStateFlags::kHasErrors);
			}

			inline void ClearState() const noexcept
			{
				m_state.clear();
			}

		private:
			bool ParseVersion(
				const Json::Value& a_in,
				const char* a_key,
				std::uint32_t& a_out) const;

			ParserState& m_state;
		};

		template <class T>
		Parser<T>::Parser(ParserState& a_state) :
			m_state(a_state)
		{
		}

		template <class T>
		void Parser<T>::Create(const T& a_in, Json::Value& a_out) const
		{
			static_assert(false, "Not implemented");
		}

		template <class T>
		void Parser<T>::Create(const T& a_in, Json::Value& a_out, bool a_arg) const
		{
			static_assert(false, "Not implemented");
		}

		template <class T>
		inline void Parser<T>::Create(
			const T& a_in,
			Json::Value& a_out,
			std::uint32_t a_arg) const
		{
			static_assert(false, "Not implemented");
		}

		template <class T>
		bool Parser<T>::Parse(const Json::Value& a_in, T& a_out) const
		{
			static_assert(false, "Not implemented");
		}

		template <class T>
		bool Parser<T>::Parse(
			const Json::Value& a_in,
			T& a_out,
			const std::uint32_t a_version) const
		{
			static_assert(false, "Not implemented");
		}

		template <class T>
		bool Parser<T>::Parse(
			const Json::Value& a_in,
			T& a_out,
			const std::uint32_t a_version,
			bool a_arg) const
		{
			static_assert(false, "Not implemented");
		}

		template <class T>
		bool Parser<T>::Parse(
			const Json::Value& a_in,
			T& a_out,
			bool a_arg) const
		{
			static_assert(false, "Not implemented");
		}

		template <class T>
		void Parser<T>::GetDefault(T& a_out) const
		{
			static_assert(false, "Not implemented");
		}

		template <class T>
		bool Parser<T>::ParseVersion(
			const Json::Value& a_in,
			const char* a_key,
			std::uint32_t& a_out) const
		{
			if (a_in.isMember(a_key))
			{
				auto& v = a_in[a_key];

				if (!v.isNumeric())
				{
					return false;
				}

				a_out = static_cast<std::uint32_t>(v.asUInt());
			}
			else
			{
				a_out = 0;
			}

			return true;
		}

		template <std::size_t _Size>
		bool ParseFloatArray(const Json::Value& a_in, float (&a_out)[_Size])
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
			float* a_out,
			std::uint32_t a_size);

		template <std::size_t _Size>
		void CreateFloatArray(const float (&a_in)[_Size], Json::Value& a_out)
		{
			for (auto& e : a_in)
			{
				a_out.append(e);
			}
		}

		void CreateFloatArray(
			const float* a_in,
			std::uint32_t a_size,
			Json::Value& a_out);

		void SafeCleanup(const fs::path& a_path) noexcept;
		std::string SafeGetPath(const fs::path& a_path) noexcept;
		void CreateRootPath(const std::filesystem::path& a_path);

		template <class T>
		void ReadData(
			const fs::path& a_path,
			T& a_root)
		{
			std::ifstream ifs;

			ifs.open(a_path, std::ifstream::in | std::ifstream::binary);
			if (!ifs.is_open())
			{
				throw std::system_error(errno, std::system_category(), a_path.string());
			}

			ifs >> a_root;
		}

		template <class Tp>
		void WriteData(
			Tp&& a_path,
			const Json::Value& a_root,
			bool a_styled = false)
		{
			fs::path tmpPath(std::forward<Tp>(a_path));

			CreateRootPath(tmpPath);

			tmpPath += ".tmp";

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
							tmpPath.string());
					}

					if (!a_styled)
					{
						Json::StreamWriterBuilder builder;
						builder["indentation"] = "";
						builder["commentStyle"] = "None";
						std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
						writer->write(a_root, std::addressof(ofs));
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

		template <class T, class Tp>
		void WriteData(
			Tp&& a_path,
			const T& a_root)
		{
			fs::path tmpPath(std::forward<Tp>(a_path));

			CreateRootPath(tmpPath);

			tmpPath += ".tmp";

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
							tmpPath.string());
					}

					ofs << a_root;
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
			T& data;
		};

		template <class T>
		struct parserDescConst_t
		{
			const char* member;
			const T& data;
		};
	}
}

#define JSON_PARSE_VERSION()                                                                 \
	std::uint32_t version;                                                                   \
	if (!ParseVersion(a_in, "version", version))                                             \
	{                                                                                        \
		Error("%s: bad version data", __FUNCTION__);                                         \
		throw std::exception("failed parsing version data");                                 \
	}                                                                                        \
	if (version > CURRENT_VERSION)                                                           \
	{                                                                                        \
		Error("%s: unsupported version (%u > %u) ", __FUNCTION__, version, CURRENT_VERSION); \
		throw std::exception("unsupported version");                                         \
	}