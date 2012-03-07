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

#include "Signals/Rebin.hpp"
#include <math.h>

namespace Signals
{

void Rebin::Apply(Signal& ASource)
{
  double fRebinFactor = GetSignalBinTime()/ASource.GetSignalBinTime();
  unsigned int RebinFactor = floor(fRebinFactor);
  if(RebinFactor==0) throw Tools::MsgException("Rebin::Apply : Rebin factor of 0.");
  if(RebinFactor-RebinFactor>0.000001) throw Tools::MsgException("Rebin::Apply : Cannot rebin fractional, require bintimeout*n=bintimein");
  SetSignalBins(ASource.GetSignalBins()/RebinFactor);
  for(unsigned int i=0;i<GetSignalBins();++i)
  {
    double  Sum   = 0;
    unsigned int Index = i*RebinFactor;
    for(unsigned int n=0;n<RebinFactor;++n)
    {
      Sum+=ASource.GetSignalBin(Index+n);
    }
    SetSignalBin(i,Sum);
  }
}

}
