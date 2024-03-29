#pragma once

#include "Events.h"
#include "Profile.h"

namespace IED
{
	template <class T>
	class ProfileManager :
		public ::Events::ThreadSafeEventDispatcher<ProfileManagerEvent<T>>,
		ILog
	{
	public:
		using profile_data_type = typename T::base_type;

		static_assert(std::is_base_of_v<ProfileBase<profile_data_type>, T>);

		using storage_type = stl::vectormap<stl::fixed_string, T>;

		ProfileManager() = default;
		ProfileManager(const fs::path& a_ext);
		ProfileManager(fs::path&& a_ext);

		virtual ~ProfileManager() noexcept = default;

		ProfileManager(const ProfileManager<T>&)               = delete;
		ProfileManager<T>& operator=(const ProfileManager<T>&) = delete;

		bool Load(const fs::path& a_path);
		bool Unload();

		[[nodiscard]] bool CreateProfile(
			const std::string& a_name,
			T&                 a_out,
			bool               a_save = false);

		template <
			class Tp,
			class = std::enable_if_t<std::is_convertible_v<Tp, T>>>
		[[nodiscard]] bool AddProfile(Tp&& a_in);
		[[nodiscard]] bool DeleteProfile(const stl::fixed_string& a_name);

		[[nodiscard]] bool RenameProfile(
			const stl::fixed_string& a_oldName,
			const std::string&       a_newName);

		template <class Td>
		[[nodiscard]] bool SaveProfile(
			const stl::fixed_string& a_name,
			Td&&                     a_in);

		[[nodiscard]] bool SaveProfile(const stl::fixed_string& a_name);

		[[nodiscard]] constexpr auto& Data() noexcept
		{
			return m_storage;
		}

		[[nodiscard]] constexpr auto& Data() const noexcept
		{
			return m_storage;
		}

		[[nodiscard]] constexpr const auto& GetLastException() const noexcept
		{
			return m_lastExcept;
		}

		[[nodiscard]] constexpr bool IsInitialized() const noexcept
		{
			return m_isInitialized;
		}

		FN_NAMEPROC("ProfileManager");

	private:
		virtual void OnProfileAdd(T& a_profile);
		virtual void OnProfileDelete(T& a_profile);
		virtual void OnProfileRename(T& a_profile, const stl::fixed_string& a_oldName);

		void sort();

		storage_type               m_storage;
		fs::path                   m_root;
		const fs::path             m_ext{ ".json" };
		mutable except::descriptor m_lastExcept;
		bool                       m_isInitialized{ false };
	};

	template <class T>
	ProfileManager<T>::ProfileManager(
		const fs::path& a_ext) :
		m_ext(a_ext)
	{
	}

	template <class T>
	ProfileManager<T>::ProfileManager(
		fs::path&& a_ext) :
		m_ext(std::move(a_ext))
	{
	}

	template <class T>
	bool ProfileManager<T>::Load(const fs::path& a_path)
	{
		try
		{
			if (!fs::exists(a_path))
			{
				if (!fs::create_directories(a_path))
				{
					throw stl::traced_exception("Couldn't create profile directory");
				}
			}
			else if (!fs::is_directory(a_path))
			{
				throw stl::traced_exception("Root path is not a directory");
			}

			storage_type tmp;

			for (const auto& entry : fs::directory_iterator(a_path))
			{
				try
				{
					if (!entry.is_regular_file())
					{
						continue;
					}

					auto& path = entry.path();
					if (!path.has_extension() ||
					    path.extension() != m_ext)
					{
						continue;
					}

					const auto profile = std::make_unique<T>();

					profile->SetPath(path);

					if (!profile->Load())
					{
						Error(
							"Failed loading profile '%s': %s",
							profile->PathStr().c_str(),
							profile->GetLastException().what());

						continue;
					}

					if (profile->HasParserErrors())
					{
						Warning(
							"Errors occured while parsing profile '%s'",
							profile->PathStr().c_str());
					}

					auto name = profile->Name();

					tmp.emplace(std::move(name), std::move(*profile));
				}
				catch (const std::exception& e)
				{
					Warning(
						"%s: exception occured while processing profile '%s': %s",
						__FUNCTION__,
						Serialization::SafeGetPath(entry.path()).c_str(),
						e.what());

					continue;
				}
			}

			tmp.sortvec([](auto& a_lhs, auto& a_rhs) -> bool {
				return stl::fixed_string::less_str{}(a_lhs->first, a_rhs->first);
			});

			m_root          = a_path;
			m_storage       = std::move(tmp);
			m_isInitialized = true;

			Message("Loaded %zu profile(s)", m_storage.size());

			return true;
		}
		catch (const std::exception& e)
		{
			Error("%s: %s", __FUNCTION__, e.what());
			m_lastExcept = e;

			return false;
		}
	}

	template <class T>
	bool ProfileManager<T>::Unload()
	{
		try
		{
			if (!m_isInitialized)
				throw stl::traced_exception("Not initialized");

			m_storage.clear();
			m_root.clear();

			m_isInitialized = false;

			return true;
		}
		catch (const std::exception& e)
		{
			Error("%s: %s", __FUNCTION__, e.what());
			m_lastExcept = e;
			return false;
		}
	}

	template <class T>
	bool ProfileManager<T>::CreateProfile(
		const std::string& a_name,
		T&                 a_out,
		bool               a_save)
	{
		try
		{
			if (!m_isInitialized)
				throw stl::traced_exception("Not initialized");

			if (!a_name.size())
				throw stl::traced_exception("Profile name length == 0");

			fs::path path(m_root);

			auto fn = fs::path(stl::str_to_wstr(a_name)).filename();
			if (!fn.has_filename())
			{
				throw stl::traced_exception("Bad filename");
			}

			path /= fn;
			path += m_ext;

			if (fs::exists(path))
				throw stl::traced_exception("Profile already exists");

			a_out.SetPath(path);

			if (a_save)
			{
				if (!a_out.Save())
				{
					throw stl::traced_exception(a_out.GetLastException().what());
				}
			}

			return true;
		}
		catch (const std::exception& e)
		{
			Error("%s: %s", __FUNCTION__, e.what());
			m_lastExcept = e;
			return false;
		}
	}

	template <class T>
	template <class Tp, class>
	bool ProfileManager<T>::AddProfile(Tp&& a_in)
	{
		try
		{
			if (!m_isInitialized)
				throw stl::traced_exception("Not initialized");

			auto key = a_in.Name();

			//CheckProfileKey(key);

			auto r = m_storage.emplace(std::move(key), std::forward<Tp>(a_in));
			if (r.second)
			{
				sort();

				OnProfileAdd(r.first->second);
				const ProfileManagerEvent<T> evn{
					ProfileManagerEvent<T>::EventType::kProfileAdd,
					nullptr,
					std::addressof(r.first->first),
					std::addressof(r.first->second)
				};
				this->SendEvent(evn);
			}
			else
			{
				throw stl::traced_exception("Profile already exists");
			}

			return true;
		}
		catch (const std::exception& e)
		{
			Error("%s: %s", __FUNCTION__, e.what());
			m_lastExcept = e;
			return false;
		}
	}

	template <class T>
	bool ProfileManager<T>::DeleteProfile(const stl::fixed_string& a_name)
	{
		try
		{
			if (!m_isInitialized)
			{
				throw stl::traced_exception("Not initialized");
			}

			auto it = m_storage.find(a_name);
			if (it == m_storage.end())
			{
				throw stl::traced_exception("No such profile exists");
			}

			const auto& path = it->second.Path();

			if (fs::exists(path) && fs::is_regular_file(path))
			{
				if (!fs::remove(path))
					throw stl::traced_exception("Failed to remove the file");
			}

			OnProfileDelete(it->second);
			const ProfileManagerEvent<T> evn{
				ProfileManagerEvent<T>::EventType::kProfileDelete,
				nullptr,
				std::addressof(it->first),
				std::addressof(it->second)
			};
			this->SendEvent(evn);

			m_storage.erase(it);

			return true;
		}
		catch (const std::exception& e)
		{
			Error("%s: %s", __FUNCTION__, e.what());
			m_lastExcept = e;
			return false;
		}
	}

	template <class T>
	bool ProfileManager<T>::RenameProfile(
		const stl::fixed_string& a_oldName,
		const std::string&       a_newName)
	{
		try
		{
			if (!m_isInitialized)
				throw stl::traced_exception("Not initialized");

			auto it = m_storage.find(a_oldName);
			if (it == m_storage.end())
				throw stl::traced_exception("No such profile exists");

			if (m_storage.find(a_newName) != m_storage.end())
				throw stl::traced_exception("A profile with that name already exists");

			fs::path newFilename(a_newName);
			newFilename += m_ext;

			auto newPath = it->second.Path();
			newPath.replace_filename(newFilename);

			if (fs::exists(newPath))
				throw stl::traced_exception("A profile file with that name already exists");

			fs::rename(it->second.Path(), newPath);

			it->second.SetPath(newPath);

			auto newName = it->second.Name();

			auto r = m_storage.emplace(std::move(newName), std::move(it->second));
			m_storage.erase(it);

			sort();

			OnProfileRename(r.first->second, a_oldName);

			const auto oldName(a_oldName);

			const ProfileManagerEvent<T> evn{
				ProfileManagerEvent<T>::EventType::kProfileRename,
				std::addressof(oldName),
				std::addressof(r.first->first),
				std::addressof(r.first->second)
			};
			this->SendEvent(evn);

			return true;
		}
		catch (const std::exception& e)
		{
			Error("%s: %s", __FUNCTION__, e.what());
			m_lastExcept = e;
			return false;
		}
	}

	template <class T>
	template <class Td>
	bool ProfileManager<T>::SaveProfile(
		const stl::fixed_string& a_name,
		Td&&                     a_in)
	{
		try
		{
			auto it = m_storage.find(a_name);
			if (it == m_storage.end())
				throw stl::traced_exception("No such profile exists");

			it->second.Data() = std::forward<Td>(a_in);
			it->second.MarkModified();

			if (!it->second.Save())
				throw stl::traced_exception(it->second.GetLastException().what());

			const ProfileManagerEvent<T> evn{
				ProfileManagerEvent<T>::EventType::kProfileSave,
				nullptr,
				std::addressof(it->first),
				std::addressof(it->second)
			};
			this->SendEvent(evn);

			return true;
		}
		catch (const std::exception& e)
		{
			Error("%s: %s", __FUNCTION__, e.what());
			m_lastExcept = e;
			return false;
		}
	}

	template <class T>
	bool ProfileManager<T>::SaveProfile(
		const stl::fixed_string& a_name)
	{
		try
		{
			auto it = m_storage.find(a_name);
			if (it == m_storage.end())
				throw stl::traced_exception("No such profile exists");

			if (!it->second.Save())
				throw stl::traced_exception(it->second.GetLastException().what());

			const ProfileManagerEvent<T> evn{
				ProfileManagerEvent<T>::EventType::kProfileSave,
				nullptr,
				std::addressof(it->first),
				std::addressof(it->second)
			};
			this->SendEvent(evn);

			return true;
		}
		catch (const std::exception& e)
		{
			Error("%s: %s", __FUNCTION__, e.what());
			m_lastExcept = e;
			return false;
		}
	}

	template <class T>
	void ProfileManager<T>::OnProfileAdd(T&)
	{
	}

	template <class T>
	void ProfileManager<T>::OnProfileDelete(T&)
	{
	}

	template <class T>
	void ProfileManager<T>::OnProfileRename(T&, const stl::fixed_string&)
	{
	}

	template <class T>
	void ProfileManager<T>::sort()
	{
		m_storage.sortvec([](auto& a_lhs, auto& a_rhs) -> bool {
			return stl::fixed_string::less_str{}(a_lhs->first, a_rhs->first);
		});
	}

}