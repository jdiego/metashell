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

#include <metashell/console_displayer.hpp>
#include <metashell/colored_string.hpp>
#include <metashell/highlight_syntax.hpp>
#include <metashell/indenter.hpp>

#include "indenter.hpp"

#include <mindent/stream_display.hpp>

#include <functional>
#include <iostream>
#include <sstream>

using namespace metashell;

console_displayer::console_displayer(
  iface::console& console_,
  bool indent_,
  bool syntax_highlight_
) :
  _console(&console_),
  _indent(indent_),
  _syntax_highlight(syntax_highlight_)
{}

void console_displayer::show_raw_text(const std::string& text_)
{
  _console->show(text_);
  _console->new_line();
}

void console_displayer::show_error(const std::string& msg_)
{
  if (!msg_.empty()) {
    if (_syntax_highlight)
    {
      _console->show(colored_string(msg_, color::bright_red));
    }
    else
    {
      _console->show(msg_);
    }
    _console->new_line();
  }
}

void console_displayer::show_type(const type& type_)
{
  show_cpp_code(type_.name());
}

void console_displayer::show_comment(const text& msg_)
{
  // TODO: handle one-liners differently (with //)
  indenter ind(_console->width(), " * ");

  ind.raw("/*");

  for (const auto& p : msg_.paragraphs)
  {
    if (p.content.empty())
    {
      ind.empty_line();
    }
    else
    {
      ind.left_align(
        p.content,
        " * " + p.rest_of_lines_indentation,
        " * " + p.first_line_indentation
      );
    }
  }

  ind.raw(" */");

  show_cpp_code(ind.str());
}

void console_displayer::show_cpp_code(const std::string& code_)
{
  if (code_ != "")
  {
    if (_indent)
    {
      if (_syntax_highlight)
      {
        indent(
          _console->width(),
          2,
          std::function<void(const token&)>(
            [this](const token& t_)
            {
              this->_console->show(highlight_syntax(t_.value()));
            }
          ),
          code_,
          "<output>"
        );
      }
      else
      {
        indent(
          _console->width(),
          2,
          std::function<void(const token&)>(
            [this](const token& t_) { this->_console->show(t_.value()); }
          ),
          code_,
          "<output>"
        );
      }
    }
    else
    {
      display_code(code_);
    }
    _console->new_line();
  }
}

void console_displayer::display_code(const std::string& code_)
{
  if (_syntax_highlight)
  {
    _console->show(highlight_syntax(code_));
  }
  else
  {
    _console->show(code_);
  }
}

void console_displayer::show_frame(const frame& frame_)
{
  display_code(frame_.name().name());
  if (frame_.has_kind())
  {
    std::ostringstream s;
    s << " (" << frame_.kind() << ")";
    _console->show(s.str());
  }
  _console->new_line();
}

void console_displayer::show_backtrace(const backtrace& trace_)
{
  int i = 0;
  for (const frame& f : trace_)
  {
    std::ostringstream s;
    s << "#" << i << " ";
    _console->show(colored_string(s.str(), color::white));
    show_frame(f);
    ++i;
  }
}

namespace
{
  color get_color(int n_)
  {
    color cs[] = {
      color::red,
      color::green,
      color::yellow,
      color::blue,
      color::cyan
    };
    return cs[n_ % (sizeof(cs) / sizeof(cs[0]))];
  }

  void display_trace_graph(
    int depth_,
    const std::vector<int>& depth_counter_,
    bool print_mark_,
    iface::console& console_
  )
  {
    assert(depth_counter_.size() > static_cast<unsigned int>(depth_));

    if (depth_ > 0)
    {
      //TODO respect the -H (no syntax highlight parameter)
      for (int i = 1; i < depth_; ++i)
      {
        console_.show(
          colored_string(depth_counter_[i] > 0 ? "| " : "  ", get_color(i))
        );
      }

      const color mark_color = get_color(depth_);
      if (print_mark_)
      {
        if (depth_counter_[depth_] == 0)
        {
          console_.show(colored_string("` ", mark_color));
        }
        else
        {
          console_.show(colored_string("+ ", mark_color));
        }
      }
      else if (depth_counter_[depth_] > 0)
      {
        console_.show(colored_string("| ", mark_color));
      }
      else
      {
        console_.show("  ");
      }
    }
  }

  colored_string format_frame(const frame& f_)
  {
    std::ostringstream s;
    if (f_.has_kind())
    {
      s << " (" << f_.kind() << ")";
    }
    return highlight_syntax(f_.name().name()) + s.str();
  }

  void display_node(
    const call_graph_node& node_,
    const std::vector<int>& depth_counter_,
    int width_,
    iface::console& console_
  )
  {
    const colored_string element_content = format_frame(node_.current_frame());

    const int non_content_length = 2*node_.depth();

    const int pretty_print_threshold = 10;
    if (
      width_ < pretty_print_threshold
      || non_content_length >= width_ - pretty_print_threshold
    )
    {
      // We have no chance to display the graph nicely :(
      display_trace_graph(node_.depth(), depth_counter_, true, console_);

      console_.show(element_content);
      console_.new_line();
    }
    else
    {
      int content_width = width_ - non_content_length;
      for (unsigned i = 0; i < element_content.size(); i += content_width)
      {
        display_trace_graph(node_.depth(), depth_counter_, i == 0, console_);
        console_.show(element_content.substr(i, content_width));
        console_.new_line();
      }
    }
  }
}

void console_displayer::show_call_graph(const iface::call_graph& cg_)
{
  const auto width = _console->width();

  std::vector<int> depth_counter(1);

  ++depth_counter[0]; // This value is neved read

  for (const call_graph_node& n : cg_)
  {
    --depth_counter[n.depth()];

    display_node(n, depth_counter, width, *_console);

    if (depth_counter.size() <= static_cast<unsigned int>(n.depth()+1))
    {
      depth_counter.resize(n.depth()+1+1);
    }

    depth_counter[n.depth()+1] += n.number_of_children();
  }
}

