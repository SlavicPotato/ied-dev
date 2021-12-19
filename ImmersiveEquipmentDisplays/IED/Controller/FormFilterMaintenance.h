#pragma once

#include "IED/GlobalProfileManager.h"
#include "IED/Profile/Events.h"

namespace IED
{
	class FormFilterMaintenance :
		Events::EventSink<ProfileManagerEvent<FormFilterProfile>>
	{
	private:
		virtual void Receive(const ProfileManagerEvent<FormFilterProfile>& a_evn) override;

		void OnProfileRename(
			const stl::fixed_string& a_oldName,
			const stl::fixed_string& a_newName);

		void OnProfileDelete(
			const stl::fixed_string& a_name);

		void OnProfileReload(
			const FormFilterProfile& a_profile);
	};
}