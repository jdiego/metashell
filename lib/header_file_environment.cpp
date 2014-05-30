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

#include <metashell/header_file_environment.hpp>
#include <metashell/headers.hpp>
#include <metashell/config.hpp>

#include "exception.hpp"

#include <just/process.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <iostream>
#include <fstream>
#include <vector>

using namespace metashell;

namespace
{
  const char env_fn[] = "metashell_environment.hpp";

  void precompile(
    const std::string& clang_path_,
    const std::vector<std::string>& clang_args_,
    const std::string& fn_
  )
  {
    using boost::algorithm::trim_copy;

    std::vector<std::string> cmd(1, clang_path_);
    cmd.insert(cmd.end(), clang_args_.begin(), clang_args_.end());
    cmd.push_back("-w");
    cmd.push_back("-o");
    cmd.push_back(fn_ + ".pch");
    cmd.push_back(fn_);

    const just::process::output o = just::process::run(cmd, "");
    const std::string err = o.standard_output() + o.standard_error();
    if (
      !err.empty()
      // clang displays this even when "-w" is used. This can be ignored
      && trim_copy(err) !=
        "warning: precompiled header used __DATE__ or __TIME__."
    )
    {
      throw exception("Error precompiling header " + fn_ + ": " + err);
    }
  }
}

header_file_environment::header_file_environment(
  bool use_precompiled_headers_,
  const config& config_
) :
  _dir(),
  _buffer(_dir.path(), config_, "-I" + _dir.path()),
  _clang_args(),
  _empty_headers(_buffer.internal_dir(), true),
  _use_precompiled_headers(use_precompiled_headers_),
  _clang_path(config_.clang_path)
{
  _clang_args = _buffer.clang_arguments();
  if (_use_precompiled_headers)
  {
    _clang_args.push_back("-include");
    _clang_args.push_back(env_filename());
  }

  save();
  _buffer.get_headers().generate();
}

void header_file_environment::append(const std::string& s_)
{
  _buffer.append(s_);
  save();
}

std::string header_file_environment::get() const
{
  return
    _use_precompiled_headers ?
      std::string() : // The -include directive includes the header
      "#include <" + std::string(env_fn) + ">\n";
}

std::string header_file_environment::get_appended(const std::string& s_) const
{
  return get() + s_;
}

const std::vector<std::string>&
  header_file_environment::clang_arguments() const
{
  return _clang_args;
}

std::string header_file_environment::env_filename() const
{
  return internal_dir() + "/" + env_fn;
}

void header_file_environment::save()
{
  const std::string fn = env_filename();
  {
    std::ofstream f(fn.c_str());
    if (f)
    {
      f << _buffer.get();
    }
    else
    {
      throw exception("Error saving environment to " + fn);
    }
  }

  if (_use_precompiled_headers)
  {
    precompile(_clang_path, _buffer.clang_arguments(), fn);
  }
}

std::string header_file_environment::internal_dir() const
{
  return _dir.path();
}

const headers& header_file_environment::get_headers() const
{
  return _empty_headers;
}

