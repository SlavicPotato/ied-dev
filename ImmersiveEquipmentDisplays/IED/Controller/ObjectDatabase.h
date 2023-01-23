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

		static constexpr long long CLEANUP_DELAY = 1000000;

	public:
		using ObjectDatabaseEntry = std::shared_ptr<entry_t>;

	private:
		using container_type = stl::unordered_map<stl::fixed_string, ObjectDatabaseEntry>;

	public:
		static constexpr auto DEFAULT_LEVEL = ObjectDatabaseLevel::kNone;

		[[nodiscard]] bool GetUniqueObject(
			const char*          a_path,
			ObjectDatabaseEntry& a_outEntry,
			NiPointer<NiNode>&   a_outObject,
			float                a_colliderScale = 1.0f) noexcept;

		bool        ValidateObject(const char* a_path, NiAVObject* a_object) noexcept;
		static bool HasBSDismemberSkinInstance(NiAVObject* a_object) noexcept;

		void RunObjectCleanup() noexcept;
		void QueueDatabaseCleanup() noexcept;

		[[nodiscard]] constexpr auto GetODBLevel() const noexcept
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
		constexpr void SetODBLevel(ObjectDatabaseLevel a_level) noexcept
		{
			if (a_level == ObjectDatabaseLevel::kDisabled)
			{
				m_level = ObjectDatabaseLevel::kNone;
			}
			else
			{
				m_level = std::clamp(
					a_level,
					ObjectDatabaseLevel::kNone,
					ObjectDatabaseLevel::kMax);
			}
		}

	private:
		static NiNode* CreateClone(NiNode* a_object, float a_collisionObjectScale) noexcept;

		ObjectDatabaseLevel      m_level{ DEFAULT_LEVEL };
		std::optional<long long> m_cleanupDeadline;
		container_type           m_data;
	};
}