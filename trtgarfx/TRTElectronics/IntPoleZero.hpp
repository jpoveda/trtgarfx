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

#ifndef _TRTElectronics_IntPoleZero_
#define _TRTElectronics_IntPoleZero_

#include "Signals/Signal.hpp"

namespace TRT
{

class IntPoleZero:public Signals::Signal
{
  protected:
  void Apply(Signal& ASignal);
  public:
  std::vector<double> FDelta1Coefficients;
  std::vector<double> FDecay1Coefficients;
  std::vector<double> FDelta2Coefficients;
  std::vector<double> FDecay2Coefficients;
  std::vector<double> FWeight2Coefficients;
  double FDelay;
  IntPoleZero(double ASignalBinTime):Signal(ASignalBinTime){};
  void SaveCoefficientsToStream(Tools::ASCIIOutStream& AStream);
  void Debug(std::string AIndent="");
};

}

#endif
