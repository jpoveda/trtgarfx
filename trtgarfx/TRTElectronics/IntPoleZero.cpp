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

#include "TRTElectronics/IntPoleZero.hpp"
#include <iostream>

namespace TRT
{

void IntPoleZero::Debug(std::string AIndent)
{
  std::cout<<AIndent<<"IntPoleZero:"<<std::endl;
  for(unsigned int i=0;i<FDelta1Coefficients.size();++i)
  {
    std::cout<<AIndent<<" Delta1["<<i<<"]:"<<FDelta1Coefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FDecay1Coefficients.size();++i)
  {
    std::cout<<AIndent<<" Decay1["<<i<<"]:"<<FDecay1Coefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FDelta2Coefficients.size();++i)
  {
    std::cout<<AIndent<<" Delta2["<<i<<"]:"<<FDelta2Coefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FDecay2Coefficients.size();++i)
  {
    std::cout<<AIndent<<" Decay2["<<i<<"]:"<<FDecay2Coefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FWeight2Coefficients.size();++i)
  {
    std::cout<<AIndent<<" Weight2["<<i<<"]:"<<FWeight2Coefficients[i]<<std::endl;
  }
  std::cout<<" Delay"<<FDelay<<std::endl;
}

void IntPoleZero::SaveCoefficientsToStream(Tools::ASCIIOutStream& AStream)
{
  AStream<<"Delta1Coefficients"<<FDelta1Coefficients
         <<"Decay1Coefficients"<<FDecay1Coefficients
         <<"Delta2Coefficients"<<FDelta2Coefficients
         <<"Decay2Coefficients"<<FDecay2Coefficients
         <<"Weight2Coefficients"<<FWeight2Coefficients
         <<"Delay"<<FDelay;
}

void IntPoleZero::Apply(Signal& ASource)
{
  AdjustSignalEnviroment(ASource);
  unsigned int DelayBins = FDelay/GetSignalBinTime()+1;

  for(unsigned int i=0;i<DelayBins;++i)
  {
    SetSignalBin(i,0);
  }

  double Integral1  = 0;
  double Integral2  = 0;
  double OutVoltage = 0;

  unsigned int SignalBins=GetSignalBins();

  for(unsigned int i=DelayBins;i<SignalBins;++i)
  {
    double Input  = ASource.GetSignalBin(i);

    Integral1   +=CalcPoly(FDelta1Coefficients,Input-Integral1)
                -CalcPoly(FDecay1Coefficients,Integral1);
    Integral2   +=CalcPoly(FDelta2Coefficients,Input-Integral2)
                -CalcPoly(FDecay2Coefficients,Integral2);

    OutVoltage  = CalcPoly(FWeight2Coefficients,Integral2)-Integral1;

    SetSignalBin(i,OutVoltage);
  }
}

}
