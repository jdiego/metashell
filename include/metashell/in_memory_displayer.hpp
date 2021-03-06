#ifndef METASHELL_IN_MEMORY_DISPLAYER_HPP
#define METASHELL_IN_MEMORY_DISPLAYER_HPP

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

#include <metashell/iface/displayer.hpp>

#include <vector>

namespace metashell
{
  class in_memory_displayer : public iface::displayer
  {
  public:
    typedef std::vector<call_graph_node> call_graph;

    virtual void show_raw_text(const std::string& text_) override;
    virtual void show_error(const std::string& msg_) override;
    virtual void show_type(const type& type_) override;
    virtual void show_comment(const text& msg_) override;
    virtual void show_cpp_code(const std::string& code_) override;

    virtual void show_backtrace(const backtrace& trace_) override;
    virtual void show_frame(const frame& frame_) override;
    virtual void show_call_graph(const iface::call_graph& cg_) override;

    const std::vector<std::string>& errors() const;
    const std::vector<std::string>& raw_texts() const;
    const std::vector<type>& types() const;
    const std::vector<text>& comments() const;
    const std::vector<std::string>& cpp_codes() const;

    const std::vector<frame>& frames() const;
    const std::vector<backtrace>& backtraces() const;
    const std::vector<call_graph>& call_graphs() const;

    bool empty() const;
    void clear();
  private:
    std::vector<std::string> _errors;
    std::vector<std::string> _raw_texts;
    std::vector<type> _types;
    std::vector<text> _comments;
    std::vector<std::string> _cpp_codes;
    std::vector<frame> _frames;
    std::vector<backtrace> _backtraces;
    std::vector<call_graph> _call_graphs;
  };
}

#endif


