#include "pch.h"

#include "GlobalProfileManager.h"

namespace IED
{
	GlobalProfileManager::ProfileManagerSlot         GlobalProfileManager::m_slotManager;
	GlobalProfileManager::ProfileManagerCustom       GlobalProfileManager::m_customManager;
	GlobalProfileManager::ProfileManagerNodeOverride GlobalProfileManager::m_nodeOverrideManager;
	GlobalProfileManager::ProfileManagerFormFilter   GlobalProfileManager::m_formFilterManager;
	GlobalProfileManager::ProfileManagerModel        GlobalProfileManager::m_modelManager(".obj");
}