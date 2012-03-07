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

#include "Signals/Discriminate.hpp"

namespace Signals
{

void Discriminate::Apply(Signal& ASource)
{
  AdjustSignalEnviroment(ASource);
  for(unsigned int i=0;i<GetSignalBins();++i)
  {
    if(ASource.GetSignalBin(i)>=FThreshold)
    {
      SetSignalBin(i,1);
    }
    else
    {
      SetSignalBin(i,0);
    }
  }
}

Discriminate::Discriminate(double ASignalBinTime,double AThreshold):Signal(ASignalBinTime)
{
  FThreshold=AThreshold;
}

}
