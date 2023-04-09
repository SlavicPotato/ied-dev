#pragma once

#include "ObjectDatabaseEntry.h"

#include <ext/IOTask.h>

namespace IED
{
	class ObjectDatabase;

	class QueuedModel :
		public RE::IOTask
	{
	public:
		QueuedModel(
			const ObjectDatabaseEntry& a_entry,
			const char*                a_path,
			ObjectDatabase&            a_owner,
			std::uint8_t               a_priority = 3);

		~QueuedModel() override = default;

		void Unk_01() override;
		void Unk_02() override;

		bool Run() override;

	private:
		ObjectDatabaseEntry _entry;
		std::string         _path;
		ObjectDatabase&     _owner;
	};
}