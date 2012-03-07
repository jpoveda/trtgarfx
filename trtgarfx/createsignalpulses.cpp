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

#include <TApplication.h>
#include "Signals/Signal.hpp"
#include "Signals/SignalSet.hpp"
#include "Tools/Stream.hpp"
#include "tools.hpp"
#include <iostream>
#include <TROOT.h>
#include <TCanvas.h>
#include <TGraph.h>

#include "boost/filesystem.hpp"

int main(int argc,char ** argv)
{

  Signals::SignalSet signals("Test pulses with different height(directly set)");
  for(double height=0.1;height<1;height*=1.1)
  {
    std::cout<<"Height:"<<height<<std::endl;
    Signals::Signal signal(3.125/256);
    signal.SetSignalLength(200);
    signal.CreateGauss(50,10,height);
    signals.SetSignal(-1,"signal",signal);
  }
  Tools::ASCIIOutStream f("pulses.sig");
  f<<signals;

  return 0;
}
