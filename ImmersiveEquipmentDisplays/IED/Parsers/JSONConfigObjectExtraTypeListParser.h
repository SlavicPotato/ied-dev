
#include "IED/ConfigCommon.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configObjectTypeExtraList_t>::Parse(
			const Json::Value&             a_in,
			Data::configObjectTypeExtraList_t& a_out) const;

		template <>
		void Parser<Data::configObjectTypeExtraList_t>::Create(
			const Data::configObjectTypeExtraList_t& a_in,
			Json::Value&                         a_out) const;

	}
}