#pragma once

#include "ObjectDatabaseEntry.h"

#include "IED/IOTaskBase.h"

namespace IED
{
	class ObjectDatabase;

	class QueuedModel :
		public IOTaskBase<QueuedModel>
	{
		class PostRunTask :
			public TaskDelegate
		{
		public:
			PostRunTask(
				QueuedModel* a_task);

			void Run() override;
			void Dispose() override;

		private:
			NiPointer<QueuedModel> task;
		};

	public:
		QueuedModel(
			const ObjectDatabaseEntry& a_entry,
			const char*                a_path,
			ObjectDatabase&            a_owner,
			std::uint8_t               a_priority = 3);

		~QueuedModel() override;

		void RunTask();

		[[nodiscard]] constexpr auto& GetEntry() const noexcept
		{
			return _entry;
		}
		
		[[nodiscard]] constexpr auto& GetPath() const noexcept
		{
			return _path;
		}

	private:
		ObjectDatabaseEntry _entry;
		std::string         _path;
		ObjectDatabase&     _owner;
	};
}