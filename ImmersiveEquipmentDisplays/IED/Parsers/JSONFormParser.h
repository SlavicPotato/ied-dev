#pragma once

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Game::FormID>::Parse(
			const Json::Value& a_in,
			Game::FormID& a_outData) const;

		template <>
		void Parser<Game::FormID>::Create(
			const Game::FormID& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<Game::FormID>::GetDefault(Game::FormID& a_out) const;

	}
}