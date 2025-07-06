#pragma once

#include "pch.h"

#include "utilities.hpp"

namespace QLogicaeConnect4
{
	class Bitboard
	{
	public:
		Bitboard();
		~Bitboard() = default;
		Bitboard(const Bitboard&) = delete;
		Bitboard(Bitboard&&) noexcept = delete;
		Bitboard& operator=(const Bitboard&) = delete;
		Bitboard& operator=(Bitboard&&) noexcept = delete;

		std::size_t get_row_count() const;
		std::size_t get_column_count() const;

		void reset();
		bool is_board_full() const;
		bool is_move_valid(const std::size_t& column_index) const;
		std::size_t get_available_row(const std::size_t& column_index) const;
		bool apply_move(const std::size_t& column_index, const std::size_t& player_index);
		void undo_move(const std::size_t& column_index, const std::size_t& row_index, const std::size_t& player_index);
		bool check_for_win(const std::size_t& row_index, const std::size_t& column_index, const std::size_t& player_index) const;

	protected:
		std::size_t _row_count;
		std::size_t _column_count;
		std::vector<std::size_t> _heights;
		std::vector<std::uint64_t> _bitboards;
	};
}
