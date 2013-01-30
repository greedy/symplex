/*
 *  Copyright 2013 Geoffrey Reedy
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ast.hpp"
#include "parser.hpp"
#include "eval.hpp"
#include <fstream>
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
  std::ifstream sourcefile(argv[1]);
  std::string sourcecode;
  std::getline(sourcefile, sourcecode, '\0');
  Parser p;
  p.parse(&sourcecode.front(), &sourcecode.back()+1);
  Runner r(p.result);
  r.runSuites();
  return 0;
}
