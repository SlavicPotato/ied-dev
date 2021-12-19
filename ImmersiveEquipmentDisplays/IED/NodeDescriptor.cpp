#include "pch.h"

#include "NodeDescriptor.h"
#include "NodeMap.h"

namespace IED
{
	namespace Data
	{
		void NodeDescriptor::lookup_flags()
		{
			const auto& nodeMap = Data::NodeMap::GetSingleton();
			const auto& data = nodeMap.GetData();

			if (auto it = data.find(name); it != data.end())
			{
				flags = it->second.flags;
			}
		}

	}  // namespace Data
}  // namespace IED