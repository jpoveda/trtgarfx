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

#include "TRTElectronics/ASDBLRBaselineRestorer.hpp"
#include <iostream>

namespace TRT
{

void ASDBLRBaselineRestorer::Debug(std::string AIndent)
{
  std::cout<<AIndent<<"ASDBLRBaselineRestorer:"<<std::endl;
  for(unsigned int i=0;i<FTopStateCoefficients.size();++i)
  {
    std::cout<<AIndent<<" TopState["<<i<<"]:"<<FTopStateCoefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FTopDiffCoefficients.size();++i)
  {
    std::cout<<AIndent<<" TopDiff["<<i<<"]:"<<FTopDiffCoefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FBottomStateCoefficients.size();++i)
  {
    std::cout<<AIndent<<" BottomState["<<i<<"]:"<<FBottomStateCoefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FBottomDiffCoefficients.size();++i)
  {
    std::cout<<AIndent<<" BottomDiff["<<i<<"]:"<<FBottomDiffCoefficients[i]<<std::endl;
  }
  std::cout<<AIndent<<" Delay : "<<FDelay<<std::endl;
}

void ASDBLRBaselineRestorer::SaveCoefficientsToStream(Tools::ASCIIOutStream& AStream)
{
  AStream<<"TopStateCoefficients"<<FTopStateCoefficients
         <<"TopDiffCoefficients"<<FTopDiffCoefficients
         <<"BottomStateCoefficients"<<FBottomStateCoefficients
         <<"BottomDiffCoefficients"<<FBottomDiffCoefficients
         <<"Delay"<<FDelay;
}

void ASDBLRBaselineRestorer::Apply(Signal& ASource)
{
  AdjustSignalEnviroment(ASource);

  unsigned int DelayBins  = FDelay/GetSignalBinTime()+1;
  double OutVoltage  = 0;
  unsigned int SignalBins = GetSignalBins();

  for(unsigned int i=0;i<DelayBins;++i)
  {
    SetSignalBin(i,0);
  }

  for(unsigned int i=DelayBins;i<SignalBins;++i)
  {
    double Diff=ASource.GetSignalBin(i-DelayBins+1)-ASource.GetSignalBin(i-DelayBins);
    double OutAdd;
    double DiffAdd;
    if(OutVoltage>=0)
    {
      OutAdd  = CalcPoly(FTopStateCoefficients,OutVoltage);
      DiffAdd = CalcPoly(FTopDiffCoefficients,Diff);
    }
    else
    {
      OutAdd  = CalcPoly(FBottomStateCoefficients,OutVoltage);
      DiffAdd = CalcPoly(FBottomDiffCoefficients,Diff);
    }
    OutVoltage += OutAdd+DiffAdd;
    SetSignalBin(i,OutVoltage);
  }
}

}
