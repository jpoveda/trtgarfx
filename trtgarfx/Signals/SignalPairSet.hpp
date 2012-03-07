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

#ifndef _Signals_SignalPairSet_
#define _Signals_SignalPairSet_
#include "Signals/Signal.hpp"
#include <TCanvas.h>

//*****************************************************************************
// SignalPair
//  Organizes a set of signal pairs
//*****************************************************************************

namespace Signals
{

struct SignalPair
{
  Signal FInput;
  Signal FOutput;
  Signal FResult;
  Signal FExtra1;
  struct SignalPair * FNext;
};

class SignalPairSet
{
  private:
  struct SignalPair * FPairs;
  struct SignalPair * FCurrentPair;
  double FSignalBinTime;
  double FSignalBins;

  double FScaleInput;
  double FScaleOutput;
  double FScaleResult;
  double FScaleExtra1;

  unsigned int FColorInput;
  unsigned int FColorOutput;
  unsigned int FColorResult;
  unsigned int FColorExtra1;

  public:

  void SetPlotScales(double AScaleInput,
                     double AScaleOutput,
                     double AScaleResult=1,
                     double AScaleExtra1=1);
  void SetPlotColors(unsigned int AColorInput,
                     unsigned int AColorOutput,
                     unsigned int AColorResult=0,
                     unsigned int AColorExtra1=0);
  void PlotAll(TCanvas * canvas);

  double CompareAll();

  unsigned int GetSignalBins();
  double GetSignalBinTime();
  unsigned int GetSignalPairCount();

  void SetCurrentResult(Signal& ASignal);
  void SetCurrentExtra1(Signal& ASignal);

  Signal& GetCurrentExtra1();
  Signal& GetCurrentResult();
  Signal& GetCurrentInput();
  Signal& GetCurrentOutput();

  bool IterReset();
  bool IterNext();
  void Clear();
  void AddSignalPair(Signal& AInput,Signal& AOutput);

  SignalPairSet();
  ~SignalPairSet();

};

}

#endif
