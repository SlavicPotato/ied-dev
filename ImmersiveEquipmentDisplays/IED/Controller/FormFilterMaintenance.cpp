#include "pch.h"

#include "FormFilterMaintenance.h"

namespace IED
{
	void FormFilterMaintenance::Receive(const ProfileManagerEvent<FormFilterProfile>& a_evn)
	{
		switch (a_evn.m_type)
		{
		case ProfileManagerEvent<FormFilterProfile>::EventType::kProfileDelete:
			OnProfileDelete(*a_evn.m_profile);
			break;
		case ProfileManagerEvent<FormFilterProfile>::EventType::kProfileRename:
			OnProfileRename(*a_evn.m_oldProfile, *a_evn.m_profile);

			break;
		}
	}
	void FormFilterMaintenance::OnProfileRename(
		const stl::fixed_string& a_oldName, const stl::fixed_string& a_newName)
	{
	}
	void FormFilterMaintenance::OnProfileDelete(
		const stl::fixed_string& a_name)
	{
	}
	void FormFilterMaintenance::OnProfileReload(
		const FormFilterProfile& a_profile)
	{
	}
}