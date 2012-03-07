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

#include "Signals/Scale.hpp"
#include <iostream>

namespace Signals
{

void Scale::Debug(std::string AIndent)
{
  std::cout<<AIndent<<"Scale:"<<std::endl;
  for(unsigned int i=0;i<FScaleCoefficients.size();++i)
  {
    std::cout<<AIndent<<" Scale["<<i<<"]:"<<FScaleCoefficients[i]<<std::endl;
  }
}

void Scale::SaveCoefficientsToStream(Tools::ASCIIOutStream& AStream)
{
  AStream<<"ScaleCoefficients"<<FScaleCoefficients;
}

void Scale::Apply(Signal& ASource)
{
  AdjustSignalEnviroment(ASource);

  unsigned int SignalBins=GetSignalBins();
  for(unsigned int i=0;i<SignalBins;++i)
  {
    SetSignalBin(i,CalcPoly(FScaleCoefficients,ASource.GetSignalBin(i)));
  }
}

}
