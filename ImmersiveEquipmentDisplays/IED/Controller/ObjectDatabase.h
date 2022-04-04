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

			long long accessed;

		private:
			NiPointer<NiNode> object;
		};

		inline static constexpr long long CLEANUP_DELAY = 1000000;

	public:
		using ObjectDatabaseEntry = std::shared_ptr<entry_t>;

		static inline constexpr auto DEFAULT_LEVEL = ObjectDatabaseLevel::kDisabled;

		[[nodiscard]] bool GetUniqueObject(
			const char*          a_path,
			ObjectDatabaseEntry& a_entry,
			NiPointer<NiNode>&   a_object);

		bool        ValidateObject(const char* a_path, NiAVObject* a_object);
		static bool HasBSDismemberSkinInstance(NiAVObject* a_object);

		void RunObjectCleanup();
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
		inline void SetODBLevel(ObjectDatabaseLevel a_level) noexcept
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
		NiNode* CreateClone(const entry_t& a_entry);

		ObjectDatabaseLevel m_level{ DEFAULT_LEVEL };

		std::optional<long long> m_cleanupDeadline;

		using container_type = stl::unordered_map<stl::fixed_string, ObjectDatabaseEntry>;

		container_type m_data;
	};
}