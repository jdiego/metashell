#ifndef METASHELL_SYSTEM_TEST_RUN_METASHELL_HPP
#define METASHELL_SYSTEM_TEST_RUN_METASHELL_HPP

// Metashell - Interactive C++ template metaprogramming shell
// Copyright (C) 2014, Abel Sinkovics (abel@sinkovics.hu)
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

#include "json_string.hpp"

#include <vector>
#include <string>

namespace metashell_system_test
{
  std::vector<json_string> run_metashell(
    std::initializer_list<json_string> commands_
  );
  
  json_string run_metashell_command(const std::string& command_);
}

#endif

