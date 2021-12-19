#include "pch.h"

#include "UIClipboard.h"

namespace IED
{
	namespace UI
	{
		UIClipboard UIClipboard::m_Instance;

		void UIClipboard::clear()
		{
			if (m_Instance.m_data.data)
			{
				switch (m_Instance.m_data.type)
				{
				case DataType::ConfigSlot:
					erase<Data::configSlot_t>();
					break;
				case DataType::ConfigCustom:
					erase<Data::configCustomNameValue_t>();
					break;
				case DataType::ConfigBase:
					erase<Data::configBase_t>();
					break;
				case DataType::ConfigBaseValues:
					erase<Data::configBaseValues_t>();
					break;
				case DataType::EquipmentOverride:
					erase<Data::equipmentOverride_t>();
					break;
				case DataType::EquipmentOverrideConditionList:
					erase<Data::equipmentOverrideConditionList_t>();
					break;
				case DataType::FormSet:
					erase<Data::configFormSet_t>();
					break;
				case DataType::FormList:
					erase<Data::configFormList_t>();
					break;
				case DataType::NodeOverride:
					erase<Data::configNodeOverride_t>();
					break;
				case DataType::NodeOverridePlacement:
					erase<Data::configNodeOverridePlacement_t>();
					break;
				case DataType::NodeOverridePlacementOverride:
					erase<Data::configNodeOverridePlacementOverride_t>();
					break;
				case DataType::NodeOverridePlacementOverrideList:
					erase<Data::configNodeOverridePlacementOverrideList_t>();
					break;
				case DataType::NodeOverridePlacementValues:
					erase<Data::configNodeOverridePlacementValues_t>();
					break;
				case DataType::NodeOverrideValues:
					erase<Data::configNodeOverrideValues_t>();
					break;
				case DataType::NodeOverrideOffset:
					erase<Data::configNodeOverrideOffset_t>();
					break;
				case DataType::NodeOverrideOffsetList:
					erase<Data::configNodeOverrideOffsetList_t>();
					break;
				case DataType::NodeOverrideOffsetConditionList:
					erase<Data::configNodeOverrideConditionList_t>();
					break;
				case DataType::ConfigTransform:
					erase<Data::configTransform_t>();
					break;
				case DataType::FormFilter:
					erase<Data::configFormFilter_t>();
					break;
				}
			}
		}

		UIClipboard::~UIClipboard()
		{
			clear();
		}

	}
}