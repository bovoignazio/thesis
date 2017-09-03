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

  std::ifstream input_f("input");
  std::ofstream output_f("output");
	
  GSPAN::gSpan gspan(input_f, output_f, maxpat, minnodes, enc, where, directed);
  // auto res = gspan.run(minsup);
	 
  namespace th = thesis;
	 
  using namespace std::placeholders;
  using namespace ranges;

  auto alpha =  .00000005;
  
  auto alg = [&](const auto f){return gspan.run(f);};
  auto alg_m = [&](const auto f){return gspan.run(f).size();};
  auto c_alg_m = [&](const auto f, const auto pv){return gspan.c_run_m(f, pv, alpha);};

  //  auto out = th::lamp_dec_(alg_m, 100, 240, alpha);
  //  auto out = th::bis_leap_(c_alg_m, 100, 240, alpha);
  // auto out = th::early_term_(c_alg_m,100, 240, alpha);
  auto out = th::one_pass_(alg,100, 240, alpha);

  std::cout << "ROOT: " << out << std::endl;
  

}
