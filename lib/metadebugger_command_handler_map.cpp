
// Metashell - Interactive C++ template metaprogramming shell
// Copyright (C) 2014, Andras Kucsma (andras.kucsma@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <metashell/metadebugger_command_handler_map.hpp>
#include <metashell/metadebugger_shell.hpp>

#include <sstream>

#include <boost/range/iterator_range_core.hpp>
#include <boost/algorithm/string/predicate.hpp>


namespace metashell {

metadebugger_command_handler_map::metadebugger_command_handler_map(
    const commands_t& commands_arg) :
  commands(commands_arg)
{
  std::vector<key_command_map_t::value_type> key_command_vec;
  for (std::size_t i = 0; i < commands.size(); ++i) {
    for (const std::string& key : commands[i].get_keys()) {
      key_command_vec.push_back(key_command_map_t::value_type(key, i));
    }
  }
  key_command_map.insert(key_command_vec.begin(), key_command_vec.end());
}

boost::optional<std::tuple<metadebugger_command, std::string>>
metadebugger_command_handler_map::get_command_for_line(
    const std::string& line) const
{

  std::stringstream line_stream(line);
  std::string command;

  if (!(line_stream >> command)) {
    return boost::none;
  }

  key_command_map_t::const_iterator lower =
    key_command_map.lower_bound(command);

  using boost::algorithm::starts_with;

  if (lower == key_command_map.end() ||
      !starts_with(lower->first, command))
  {
    return boost::none;
  }

  // Check if the found command is unambiguous
  if (command != lower->first && // Pass if the match is full
      std::next(lower) != key_command_map.end() &&
      starts_with(std::next(lower)->first, command))
  {
    return boost::none;
  }

  line_stream >> std::ws;
  std::string rest;
  std::getline(line_stream, rest);

  assert(lower->second < commands.size());
  return std::make_tuple(commands[lower->second], rest);
}

}

