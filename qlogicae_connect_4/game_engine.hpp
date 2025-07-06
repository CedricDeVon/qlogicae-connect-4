#pragma once

#include "pch.h"

#include "utilities.hpp"
#include "bitboard.hpp"

namespace QLogicaeConnect4
{
	class GameEngine
	{
	public:
		GameEngine();
		~GameEngine() = default;
		GameEngine(const GameEngine&) = delete;
		GameEngine(GameEngine&&) noexcept = delete;
		GameEngine& operator=(const GameEngine&) = delete;
		GameEngine& operator=(GameEngine&&) noexcept = delete;
		
		void reset_game();
		bool undo_last_move();
		bool redo_last_move();
		bool is_game_drawn() const;
		std::string serialize_to_json() const;
		std::uint64_t get_current_player_id() const;
		GameLifecycleStatus get_game_status() const;
		std::uint64_t get_winning_player_id() const;
		bool apply_move(const std::size_t& column_index);
		std::chrono::nanoseconds get_total_elapsed_time() const;
		void forfeit_game_by_player(const std::uint64_t& player_identifier);
		static GameEngine deserialize_from_json(const std::string_view json_str);
		std::chrono::nanoseconds get_elapsed_time_for_player(const std::uint64_t& player_id) const;

		std::future<void> reset_game_async();
		std::future<bool> redo_last_move_async();
		std::future<bool> undo_last_move_async();
		std::future<bool> apply_move_async(const std::size_t& column_index);
		std::future<void> forfeit_game_by_player_async(const std::uint64_t& player_identifier);

	protected:
		Bitboard _bitboard;
		GameLifecycleStatus _status;
		std::uint64_t _winning_player_id;
		std::vector<GameMove> _redo_stack;
		std::size_t _current_player_index;
		std::vector<GameMove> _move_history;
		std::vector<std::uint64_t> _player_ids;
		std::chrono::steady_clock::time_point _start_time;
		std::vector<std::chrono::nanoseconds> _elapsed_times;
		std::vector<std::chrono::steady_clock::time_point> _last_move_times;

		void _switch_turn();
		void _update_elapsed_time();
		void _update_game_state(const std::size_t& row_index, const std::size_t& column_index);
	};
}
