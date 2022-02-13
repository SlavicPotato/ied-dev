#pragma once

namespace IED
{
	template <class T>
	struct ProfileManagerEvent
	{
		enum class EventType
		{
			kProfileAdd,
			kProfileDelete,
			kProfileRename,
			kProfileSave
		};

		EventType m_type;
		const stl::fixed_string* m_oldProfile{ nullptr };
		const stl::fixed_string* m_profile{ nullptr };
		T* m_data{ nullptr };
	};
}