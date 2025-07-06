#pragma once

#include "pch.h"

#include "game_engine.hpp"

namespace QLogicaeConnect4
{
	GameEngine::GameEngine()
		: _bitboard(), _current_player_index(0), _status(GameLifecycleStatus::InProgress), _winning_player_id(0), _start_time(std::chrono::steady_clock::now())
	{
		for (std::size_t i = 0; i < 2; ++i)
			_player_ids.push_back(i + 1);
		_elapsed_times.resize(_player_ids.size(), std::chrono::nanoseconds(0));
		_last_move_times.resize(_player_ids.size(), _start_time);
	}

	bool GameEngine::apply_move(const std::size_t& column_index)
	{
		if (_status != GameLifecycleStatus::InProgress) return false;
		if (!_bitboard.is_move_valid(column_index)) return false;
		std::size_t row = _bitboard.get_available_row(column_index);
		if (!_bitboard.apply_move(column_index, _current_player_index)) return false;
		_update_elapsed_time();
		_move_history.push_back({ _player_ids[_current_player_index], column_index });
		_redo_stack.clear();
		_update_game_state(row, column_index);
		if (_status == GameLifecycleStatus::InProgress) _switch_turn();
		return true;
	}

	std::future<bool> GameEngine::apply_move_async(const std::size_t& column_index)
	{
		return std::async(std::launch::async, [this, &column_index]() { return apply_move(column_index); });
	}

	bool GameEngine::undo_last_move()
	{
		if (_move_history.empty()) return false;
		const GameMove& last = _move_history.back();
		std::size_t column = last.column_index;
		std::size_t row = _bitboard.get_available_row(column) - 1;
		std::size_t player_index = _current_player_index;
		_current_player_index = (_current_player_index + _player_ids.size() - 1) % _player_ids.size();
		_bitboard.undo_move(column, row, _current_player_index);
		_redo_stack.push_back(last);
		_move_history.pop_back();
		_status = GameLifecycleStatus::InProgress;
		_winning_player_id = 0;
		return true;
	}

	std::future<bool> GameEngine::undo_last_move_async()
	{
		return std::async(std::launch::async, [this]() { return undo_last_move(); });
	}

	bool GameEngine::redo_last_move()
	{
		if (_redo_stack.empty()) return false;
		GameMove move = _redo_stack.back();
		_redo_stack.pop_back();
		_current_player_index = std::distance(_player_ids.begin(), std::find(_player_ids.begin(), _player_ids.end(), move.player_identifier));
		return apply_move(move.column_index);
	}

	std::future<bool> GameEngine::redo_last_move_async()
	{
		return std::async(std::launch::async, [this]() { return redo_last_move(); });
	}

	void GameEngine::forfeit_game_by_player(const std::uint64_t& player_identifier)
	{
		_status = GameLifecycleStatus::Forfeit;
		_winning_player_id = 0;
		for (std::size_t i = 0; i < _player_ids.size(); ++i)
		{
			if (_player_ids[i] != player_identifier)
			{
				_winning_player_id = _player_ids[i];
				break;
			}
		}
	}

	std::future<void> GameEngine::forfeit_game_by_player_async(const std::uint64_t& player_identifier)
	{
		return std::async(std::launch::async, [this, &player_identifier]() { forfeit_game_by_player(player_identifier); });
	}

	GameLifecycleStatus GameEngine::get_game_status() const { return _status; }
	std::uint64_t GameEngine::get_winning_player_id() const { return _winning_player_id; }
	bool GameEngine::is_game_drawn() const { return _status == GameLifecycleStatus::Draw; }
	std::uint64_t GameEngine::get_current_player_id() const { return _player_ids[_current_player_index]; }

	std::chrono::nanoseconds GameEngine::get_total_elapsed_time() const
	{
		return std::chrono::steady_clock::now() - _start_time;
	}

	std::chrono::nanoseconds GameEngine::get_elapsed_time_for_player(const std::uint64_t& player_id) const
	{
		auto it = std::find(_player_ids.begin(), _player_ids.end(), player_id);
		if (it == _player_ids.end()) return std::chrono::nanoseconds(0);
		std::size_t index = std::distance(_player_ids.begin(), it);
		return _elapsed_times[index];
	}

	void GameEngine::reset_game()
	{
		_bitboard.reset();
		_move_history.clear();
		_redo_stack.clear();
		_status = GameLifecycleStatus::InProgress;
		_winning_player_id = 0;
		_current_player_index = 0;
		_start_time = std::chrono::steady_clock::now();
		std::fill(_elapsed_times.begin(), _elapsed_times.end(), std::chrono::nanoseconds(0));
		std::fill(_last_move_times.begin(), _last_move_times.end(), _start_time);
	}

	std::future<void> GameEngine::reset_game_async()
	{
		return std::async(std::launch::async, [this]() { reset_game(); });
	}

	std::string GameEngine::serialize_to_json() const { return "{}"; }
	GameEngine GameEngine::deserialize_from_json(const std::string_view json_str) { return GameEngine(); }

	void GameEngine::_update_game_state(const std::size_t& row_index, const std::size_t& column_index)
	{
		if (_bitboard.check_for_win(row_index, column_index, _current_player_index))
		{
			_status = GameLifecycleStatus::Win;
			_winning_player_id = _player_ids[_current_player_index];
		}
		else if (_bitboard.is_board_full())
		{
			_status = GameLifecycleStatus::Draw;
			_winning_player_id = 0;
		}
	}

	void GameEngine::_switch_turn()
	{
		_current_player_index = (_current_player_index + 1) % _player_ids.size();
	}

	void GameEngine::_update_elapsed_time()
	{
		auto now = std::chrono::steady_clock::now();
		_elapsed_times[_current_player_index] += now - _last_move_times[_current_player_index];
		_last_move_times[_current_player_index] = now;
	}

}
