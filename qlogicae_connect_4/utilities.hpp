#pragma once

#include "pch.h"

namespace QLogicaeConnect4
{
	enum class GameLifecycleStatus : std::uint8_t
	{
		InProgress,
		Win,
		Draw,
		Forfeit
	};

	struct GameMove
	{
		std::uint64_t player_identifier;
		std::size_t column_index;
	};

	int sum(int, int);
}
