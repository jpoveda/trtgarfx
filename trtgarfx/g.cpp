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

#include "TRT/Straw.hpp"
#include "Signals/Signal.hpp"
#include "Signals/SignalSet.hpp"
#include "Tools/Stream.hpp"

#include "boost/filesystem.hpp"

const unsigned int SignalBins    = 64;
const unsigned int SignalBinDiv  = 256;

const std::string UserName="kccmp";
const std::string Password="asmb%xx4_c";

int main(int argc,char * argv[])
{
  if(argc<2)
  {
    std::cout<<"Please specify wire distance"<<std::endl;
    return -1;
  }
  TRT::Straw straw("pion",3.125/SignalBinDiv,SignalBins*SignalBinDiv);
  straw.EnableIonSignal();
  straw.EnableMicroscopicCalculation();
  straw.LoadIonLookupTable("IonLookup.dat");
  straw.SetSignalScalingFactor(double(25)/3);
  std::cout<<"LOOP"<<std::endl;
  double distance=atof(argv[1]);
  
  std::cout<<"Distance : "<<distance<<std::endl;
  Signals::SignalSet signalset(DoubleToStr(distance));

  for(unsigned int i=0;i<128;++i)
  {
    std::cout<<">>>"<<i<<std::endl;

    double energyloss;
    unsigned int trackelectrons;
    unsigned int clusters;
    straw.CalcTrack(5.0,distance,energyloss,trackelectrons,clusters,true);
    Signals::Signal signal=straw.GetSignal();
    signalset.SetSignal(-1,"signal",signal);
  }
  Tools::ASCIIOutStream f("pion5GeV"+DoubleToStr(distance)+"cm.sigs");
  f<<signalset;
  f.Close();
  return 0;
}
