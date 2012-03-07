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

#include "TRTElectronics/ASDBLRShaper.hpp"

namespace TRT
{

void ASDBLRShaper::Apply(Signal& ASource)
{
  AdjustSignalEnviroment(ASource);
  ASource>>F1Scale>>F1HighPass>>F2PoleZero>>F2Scale>>F2LowPass;
  unsigned int SignalBins=GetSignalBins();
  // Possible to remove this by making ASDBLRShaper a descend of LowPass
  // But this should be done last
  for(unsigned int i=0;i<SignalBins;++i)
  {
    SetSignalBin(i,F2LowPass.GetSignalBin(i));
  }
}

void ASDBLRShaper::Debug(std::string AIndent)
{
  std::cout<<AIndent<<"ASDBLRShaper:"<<std::endl;
  F1HighPass.Debug(AIndent+" ");
  F2PoleZero.Debug(AIndent+" ");
  F2LowPass.Debug(AIndent+" ");
}

void ASDBLRShaper::SaveCoefficientsToStream(Tools::ASCIIOutStream& AStream)
{
  F1HighPass.SaveCoefficientsToStream(AStream);
  F2PoleZero.SaveCoefficientsToStream(AStream);
  F2LowPass.SaveCoefficientsToStream(AStream);
}

ASDBLRShaper::ASDBLRShaper(double ASignalBinTime):
 Signal(ASignalBinTime),
 F1ScaleCoefficients(F1Scale.FScaleCoefficients),
 F1DeltaCoefficients(F1HighPass.FDeltaCoefficients),
 F1DecayCoefficients(F1HighPass.FDecayCoefficients),
 F2Delta1Coefficients(F2PoleZero.FDelta1Coefficients),
 F2Decay1Coefficients(F2PoleZero.FDecay1Coefficients),
 F2Delta2Coefficients(F2PoleZero.FDelta2Coefficients),
 F2Decay2Coefficients(F2PoleZero.FDecay2Coefficients),
 F2Weight2Coefficients(F2PoleZero.FWeight2Coefficients),
 F2ScaleCoefficients(F2Scale.FScaleCoefficients),
 F2DiffCoefficients(F2LowPass.FDiffCoefficients),
 F2DecayCoefficients(F2LowPass.FDecayCoefficients),
 FDelay(F2LowPass.FDelay),
 F1Scale(ASignalBinTime),
 F1HighPass(ASignalBinTime),
 F2PoleZero(ASignalBinTime),
 F2Scale(ASignalBinTime),
 F2LowPass(ASignalBinTime)
{
  F1HighPass.FDelay = 0;
  F2PoleZero.FDelay = 0;
}

}
