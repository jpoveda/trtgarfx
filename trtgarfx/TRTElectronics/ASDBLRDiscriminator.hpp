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

#ifndef _Signals_ASDBLRDiscriminator_
#define _Signals_ASDBLRDiscriminator_
#include "Signals/Signal.hpp"
#include "Tools/Stream.hpp"
namespace TRT
{

class ASDBLRDiscriminator:public Signals::Signal
{
  public:
  double FThreshold;
  double FDCHysteresis;
  double FACHysteresis;
  double FACHysteresisDecay;
  double FDelay;
  protected:
  virtual void Apply(Signal& ASource);
  public:
  ASDBLRDiscriminator(double ASignalBinTime):Signal(ASignalBinTime){};
  void SaveCoefficientsToStream(Tools::ASCIIOutStream& AStream);
  void Debug(std::string AIndent="");
};

}
#endif
