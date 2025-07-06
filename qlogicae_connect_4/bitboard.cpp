#pragma once

#include "pch.h"

#include "bitboard.hpp"

namespace QLogicaeConnect4
{
	Bitboard::Bitboard()
		: _row_count(6), _column_count(7), _bitboards(2, 0), _heights(7, 0)
	{
	}

	bool Bitboard::apply_move(const std::size_t& column_index, const std::size_t& player_index)
	{
		if (!is_move_valid(column_index)) return false;
		std::size_t row = _heights[column_index];
		_bitboards[player_index] |= (1ULL << (row * _column_count + column_index));
		++_heights[column_index];
		return true;
	}

	bool Bitboard::is_move_valid(const std::size_t& column_index) const
	{
		return column_index < _column_count && _heights[column_index] < _row_count;
	}

	std::size_t Bitboard::get_available_row(const std::size_t& column_index) const
	{
		return _heights[column_index];
	}

	bool Bitboard::is_board_full() const
	{
		for (std::size_t h : _heights)
			if (h < _row_count) return false;
		return true;
	}

	bool Bitboard::check_for_win(const std::size_t& row_index, const std::size_t& column_index, const std::size_t& player_index) const
	{
		auto bitboard = _bitboards[player_index];
		std::size_t cols = _column_count;
		std::size_t pos = row_index * cols + column_index;
		std::uint64_t mask = 1ULL << pos;

		constexpr std::array<int, 4> directions = { 1, 7, 6, 8 };
		for (int d : directions)
		{
			int count = 1;
			for (int i = 1; i < 4; ++i)
			{
				if (bitboard & (mask << (d * i))) ++count;
				else break;
			}
			for (int i = 1; i < 4; ++i)
			{
				if (bitboard & (mask >> (d * i))) ++count;
				else break;
			}
			if (count >= 4) return true;
		}
		return false;
	}

	void Bitboard::undo_move(const std::size_t& column_index, const std::size_t& row_index, const std::size_t& player_index)
	{
		_bitboards[player_index] &= ~(1ULL << (row_index * _column_count + column_index));
		--_heights[column_index];
	}

	void Bitboard::reset()
	{
		_bitboards.assign(_bitboards.size(), 0);
		std::fill(_heights.begin(), _heights.end(), 0);
	}

	std::size_t Bitboard::get_row_count() const { return _row_count; }
	std::size_t Bitboard::get_column_count() const { return _column_count; }

}
