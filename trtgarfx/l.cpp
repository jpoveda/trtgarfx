/*
   Copyright 2011 Julian Schutsch

   This file is part of TRTGarf

   TRTGarf is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   TRTGarf is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with TRTGarf.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <stdio.h>
#include <float.h>
#include "Signals/Signal.hpp"
#include "Signals/SignalPairSet.hpp"
#include "tools.hpp"
#include <TApplication.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TMultiGraph.h>

#include "boost/filesystem.hpp"

//const std::string MainFileName="ASD_1elec";

const unsigned int SignalBinDiv=256;

int main(int argc,char * argv[])
{
  try
  {
    if(argc<2) throw "Expect parameter : Filename without suffix!";
    std::string MainFileName=argv[1];
//    TApplication application("app",&argc,argv);
    boost::filesystem::create_directory(MainFileName);
    InitRoot();
    std::string InFileName=MainFileName+".inc";
    std::string FileName=MainFileName+".out";
    for(unsigned int i=0;i<=0;++i)
    {
      Signals::Signal In(3.125/SignalBinDiv);
      In.ReadHSpiceSignalInclude(InFileName);
      In.SetSignalUnit("fC/ns");
      In.Plot(MainFileName+"/In"+IntToStr(i)+".pdf");
    
      Signals::Signal Input(0.5);

      std::vector<std::string> vars;
      vars.push_back("inputs");
      Input.ReadSignalFromHSpicelis(FileName,vars,i);
      Input.Plot(MainFileName+"/Input"+IntToStr(i)+".pdf");
      
      vars.clear();
      vars.push_back("pre_out");
      Signals::Signal Preamp(0.5);
      Preamp.ReadSignalFromHSpicelis(FileName,vars,i);
      Preamp.Flatten(1);
      Preamp.Plot(MainFileName+"/Pre"+IntToStr(i)+".pdf");
      
      vars.clear();
      vars.push_back("shaper_out");
      Signals::Signal Shaper(0.5);
      Shaper.ReadSignalFromHSpicelis(FileName,vars,i);
      Shaper.Flatten(1);
      Shaper.Plot(MainFileName+"/Shaper_Out"+IntToStr(i)+".pdf");

      vars.clear();
      vars.push_back("voltage");
      vars.push_back("i");
      vars.push_back("j");
      Signals::Signal vij(0.5);
      vij.ReadSignalFromHSpicelis(FileName,vars,i);
      vij.Flatten(1);
      vij.Plot(MainFileName+"/vij"+IntToStr(i)+".pdf");
      
      vars.clear();
      vars.push_back("voltage");
      vars.push_back("k");
      vars.push_back("l");
      Signals::Signal vkl(0.5);
      vkl.ReadSignalFromHSpicelis(FileName,vars,i);
      vkl.Flatten(1);
      vkl.Plot(MainFileName+"/vkl"+IntToStr(i)+".pdf");

      vars.clear();
      vars.push_back("ternary");
      vars.push_back("");
      vars.push_back("");
      Signals::Signal ternary(0.5);
      ternary.ReadSignalFromHSpicelis(FileName,vars,i);
      ternary.Flatten(1);
      ternary.Plot(MainFileName+"/ternary"+IntToStr(i)+".pdf");     

      vars.clear();
      vars.push_back("blr_out");
      Signals::Signal blrout(0.5);
      blrout.ReadSignalFromHSpicelis(FileName,vars,i);
      blrout.Flatten(1);
      blrout.Plot(MainFileName+"/blrout"+IntToStr(i)+".pdf");
      
    }
  }
  catch(const char * e)
  {
    std::cout<<e<<std::endl;
    throw;
  }

  return 0;
}
