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

#include "Signals/HighPass.hpp"
#include <iostream>

namespace Signals
{

void HighPass::Debug(std::string AIndent)
{
  std::cout<<AIndent<<"HighPass:"<<std::endl;
  for(unsigned int i=0;i<FDeltaCoefficients.size();++i)
  {
    std::cout<<AIndent<<" Delta["<<i<<"]:"<<FDeltaCoefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FDecayCoefficients.size();++i)
  {
    std::cout<<AIndent<<" Decay["<<i<<"]:"<<FDecayCoefficients[i]<<std::endl;
  }
}

void HighPass::SaveCoefficientsToStream(Tools::ASCIIOutStream& AStream)
{
  AStream<<"DeltaCoefficients"<<FDeltaCoefficients
         <<"DecayCoefficients"<<FDecayCoefficients
         <<"Delay"<<FDelay;
}

void HighPass::Apply(Signal& ASource)
{
  AdjustSignalEnviroment(ASource);

  unsigned int DelayBins = FDelay/GetSignalBinTime()+1;

  for(unsigned int i=0;i<DelayBins;++i)
  {
    SetSignalBin(i,0);
  }

  double OutVoltage = 0;
  double SignalBins = GetSignalBins();

  for(unsigned int i=DelayBins;i<SignalBins;++i)
  {
    OutVoltage+=CalcPoly(FDeltaCoefficients,ASource.GetSignalBin(i-DelayBins)-OutVoltage)
               -CalcPoly(FDecayCoefficients,OutVoltage);
    SetSignalBin(i,OutVoltage);
  }

}

}
