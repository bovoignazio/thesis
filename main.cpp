/*
    $Id: main.cpp,v 1.4 2004/05/21 05:50:13 taku-ku Exp $;
 
   Copyright (C) 2004 Taku Kudo, All rights reserved.
     This is free software with ABSOLUTELY NO WARRANTY.
  
   This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.
    
   This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
    
   You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
     02111-1307, USA
*/
#include "gspan.h"
#include "testability.hpp"

#include <chrono>
#include <functional>
#include <vector>
#include <utility>

#include <unistd.h>
#include <fstream>

#define OPT " [-m minsup] [-d] [-e] [-w] "

int main (int argc, char **argv)
{
  unsigned int maxpat = 0xffffffff;
  unsigned int minnodes = 0;
  bool where = false;
  bool enc = false;
  bool directed = false;
  auto n1 = 0u;
  auto n2 = 0u;

  std::ifstream input_f("MUTAG");
  std::string input_labels("MUTAG_label");
  std::ofstream output_f("output");

  {
    std::ifstream input(input_labels);
    if (!input) {
      std::cerr << "class file problem" << std::endl;
      exit(EXIT_FAILURE);
    }

    for (std::string line; std::getline(input,line);) {
      if (line[0] == '1') 
	++n1;
      else if (line[0] == '0') 
	++n2;
      else 
	std::cerr << "label file error" << std::endl;
    }

  }

  GSPAN::gSpan gspan(input_f, output_f, maxpat, minnodes, enc, where, directed);
  // auto res = gspan.run(minsup);
	 
  namespace th = thesis;
	 
  using namespace std::placeholders;
  using namespace ranges;

  auto alpha =  .0000000005;
  
  auto alg = [&](const auto f){return gspan.run(f);};
  auto alg_m = [&](const auto f){return gspan.run(f).size();};
  auto c_alg_m = [&](const auto f, const auto pv){return gspan.c_run_m(f, pv, alpha);};

  //auto out = th::lamp_dec_(alg_m, n1, n2, alpha);
  std::cout << "alpha: " << alpha << std::endl;
  auto out = th::bis_leap_(c_alg_m, n1, n2, alpha);

  //  std::cout << alg_m(24) << std::endl;
  //  auto out = th::early_term_(c_alg_m,n1, n2, alpha);
  std::cout << "OUT: " << out << std::endl;
  auto app = th::one_pass_(alg,n1, n2, alpha);
  std::cout << "APP: " << app << std::endl;




}
