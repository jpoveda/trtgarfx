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

#include "TRTElectronics/ASDBLRPreamplifier.hpp"
#include <iostream>

namespace TRT
{

void ASDBLRPreamplifier::Debug(std::string AIndent)
{
  std::cout<<AIndent<<"ASDBLRPreamplifier:"<<std::endl;
  for(unsigned int i=0;i<FStateInputCoefficients.size();++i)
  {
    std::cout<<AIndent<<" StateInput["<<i<<"]:"<<FStateInputCoefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FStateInputDiffCoefficients.size();++i)
  {
    std::cout<<AIndent<<" StateDiff["<<i<<"]:"<<FStateInputDiffCoefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FStateDecayCoefficients.size();++i)
  {
    std::cout<<AIndent<<" StateDecay["<<i<<"]:"<<FStateDecayCoefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FOutStateCoefficients.size();++i)
  {
    std::cout<<AIndent<<" OutState["<<i<<"]:"<<FOutStateCoefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FOutStateDiffCoefficients.size();++i)
  {
    std::cout<<AIndent<<" OutStateDiff["<<i<<"]:"<<FOutStateDiffCoefficients[i]<<std::endl;
  }
  for(unsigned int i=0;i<FOutOutCoefficients.size();++i)
  {
    std::cout<<AIndent<<" OutOut["<<i<<"]:"<<FOutOutCoefficients[i]<<std::endl;
  }
  std::cout<<AIndent<<" Delay:"<<FDelay<<std::endl;
}

void ASDBLRPreamplifier::SaveCoefficientsToStream(Tools::ASCIIOutStream& AStream)
{
  AStream<<"StateInputCoefficients"<<FStateInputCoefficients
         <<"StateInputDiffCoefficients"<<FStateInputDiffCoefficients
         <<"StateDecayCoefficients"<<FStateDecayCoefficients
         <<"OutStateCoefficients"<<FOutStateCoefficients
         <<"OutStateDiffCoefficients"<<FOutStateDiffCoefficients
         <<"OutOutCoefficients"<<FOutOutCoefficients
         <<"Delay"<<FDelay;
}

void ASDBLRPreamplifier::Apply(Signal& ASource)
{
  AdjustSignalEnviroment(ASource);

  unsigned int DelayBins=FDelay/GetSignalBinTime()+1;

  for(unsigned int i=0;i<DelayBins;++i)
  {
    SetSignalBin(i,0);
  }

  double State       = 0;
  double State2      = 0;
  double OutVoltage  = 0;
  unsigned int SignalBins = GetSignalBins();

  for(unsigned int i=DelayBins;i<SignalBins;++i)
  {
    double Input      = ASource.GetSignalBin(i-DelayBins);
    double PrevState2 = State2;
    double Decay      = CalcPoly(FStateDecayCoefficients,State);
    State          += CalcPoly(FStateInputCoefficients,Input)+Decay;
    State2         += CalcPoly(FStateInputDiffCoefficients,Input-State2);
    OutVoltage     += CalcPoly(FOutStateCoefficients,Decay)
                    + CalcPoly(FOutStateDiffCoefficients,State2-PrevState2)
                    + CalcPoly(FOutOutCoefficients,OutVoltage);

    SetSignalBin(i,OutVoltage);
  }
}

}
