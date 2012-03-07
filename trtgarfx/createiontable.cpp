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
#include <TApplication.h>

#include "boost/filesystem.hpp"

const unsigned int SignalBins    = 64;
const unsigned int SignalDiv     = 256;

int main(int argc,char * argv[])
{

  try
  {

    InitRoot();

    TRT::Straw strawtubeanalysis("pion",3.125/SignalDiv,SignalBins*SignalDiv);

    strawtubeanalysis.SetIonDriftDistanceStep(2*10.e-7);
    strawtubeanalysis.CalcIonLookupTable(2*10.e-6); // cm
    strawtubeanalysis.SaveIonLookupTable("IonLookup.dat");
    strawtubeanalysis.CheckIonLookupTable("IonLookup.dat");
  }
  catch(char const * e)
  {
    std::cout<<"Exception:"<<e<<std::endl;
  }
  return 0;
}
