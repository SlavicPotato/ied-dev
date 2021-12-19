#include "pch.h"

#include "GlobalProfileManager.h"

namespace IED
{
	GlobalProfileManager::ProfileManagerSlot
		GlobalProfileManager::m_slotManager("^[a-zA-Z0-9_\\-\\+ \\'\\\"\\,\\(\\)\\[\\]]+$");
	GlobalProfileManager::ProfileManagerCustom
		GlobalProfileManager::m_customManager("^[a-zA-Z0-9_\\-\\+ \\'\\\"\\,\\(\\)\\[\\]]+$");
	GlobalProfileManager::ProfileManagerNodeOverrides
		GlobalProfileManager::m_nodeOverrideManager("^[a-zA-Z0-9_\\-\\+ \\'\\\"\\,\\(\\)\\[\\]]+$");
	GlobalProfileManager::ProfileManagerFormFilter
		GlobalProfileManager::m_formFilterManager("^[a-zA-Z0-9_\\-\\+ \\'\\\"\\,\\(\\)\\[\\]]+$");
}