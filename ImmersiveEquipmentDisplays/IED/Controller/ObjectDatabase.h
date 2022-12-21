#pragma once

#include "ObjectDatabaseLevel.h"

namespace IED
{
	class ObjectDatabase :
		public virtual ILog
	{
		struct entry_t
		{
			friend class ObjectDatabase;

			long long accessed{ 0 };

		private:
			NiPointer<NiNode> object;
		};

		inline static constexpr long long CLEANUP_DELAY = 1000000;

	public:
		using ObjectDatabaseEntry = std::shared_ptr<entry_t>;

	private:
		using container_type = stl::unordered_map<stl::fixed_string, ObjectDatabaseEntry>;

	public:
		static inline constexpr auto DEFAULT_LEVEL = ObjectDatabaseLevel::kDisabled;

		[[nodiscard]] bool GetUniqueObject(
			const char*          a_path,
			ObjectDatabaseEntry& a_entry,
			NiPointer<NiNode>&   a_object) noexcept;

		bool        ValidateObject(const char* a_path, NiAVObject* a_object) noexcept;
		static bool HasBSDismemberSkinInstance(NiAVObject* a_object) noexcept;

		void RunObjectCleanup() noexcept;
		void QueueDatabaseCleanup() noexcept;

		[[nodiscard]] inline constexpr auto GetODBLevel() const noexcept
		{
			return m_level;
		}

		[[nodiscard]] inline auto GetODBObjectCount() const noexcept
		{
			return m_data.size();
		}

		[[nodiscard]] std::size_t GetODBUnusedObjectCount() const noexcept;

		void ClearObjectDatabase();

		FN_NAMEPROC("ObjectDatabase");

	protected:
		inline constexpr void SetODBLevel(ObjectDatabaseLevel a_level) noexcept
		{
			m_level =
				a_level != ObjectDatabaseLevel::kDisabled ?
					std::clamp(
						a_level,
						ObjectDatabaseLevel::kNone,
						ObjectDatabaseLevel::kMax) :
					a_level;
		}

	private:
		static NiNode* CreateClone(const entry_t& a_entry) noexcept;

		ObjectDatabaseLevel      m_level{ DEFAULT_LEVEL };
		std::optional<long long> m_cleanupDeadline;
		container_type           m_data;
	};
}