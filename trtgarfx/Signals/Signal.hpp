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

#ifndef _Signals_
#define _Signals_
#include "Tools/Exception.hpp"
#include "Tools/Stream.hpp"
#include "tools.hpp"
#ifdef USEROOT
#include <TH2D.h>
#include <TGraph.h>
#include <TRandom.h>
#endif

namespace Signals
{

class Signal{
  private:
  std::string FSignalUnit;
  std::string FTimeUnit;
  std::vector<double> FSignal;
  double              FSignalBinTime;
  void Initialize();
  // Times and Function values must be in identical units to the ones used here!
  // The vectors must be sorted
  void CopyFromVectors(std::vector<double> ATimes,std::vector<double> AFunc);
  protected:
  virtual void Apply(Signal & ASource);
  public:
  
  void AdjustSignalEnviroment(Signal& ASource);
  
  virtual void SaveCoefficientsToStream(Tools::ASCIIOutStream& AStream);
  void SaveCoefficientsToFile(std::string AFileName);

  // This is a special euler, which allows calculation of signal development for full bridges
  // It takes in a number of coefficients for an equation:
  //  delta U_a=+-Sum_i ASlopeCoefficients_i delta U_e(t-i*dt)
  //             +Sum_i ADiffCoefficients_i (U_e-U_a)(t-i*dt)
  //             -Sum_i ACoefficients_i*U_a^i
  //
  // The Equation has issues!
  //  1. We are assuming small signal
  //  2. We are assuming U_e to be equal for both bridges (see line two of the eq.)
  //
  // The time step is by the bin time since we cannot calculate signals inbetween (other then interpolate)
  // AOutputType allows to either select difference of the two half bridges (0) or either one
  // of the bridges (1 and 2)

  void Discriminate(Signal * ASource,
                    double ATreshold,
                    double ADCHysteresis,
                    double AACHysteresis,
                    double AACHysteresisDecay,
                    double ADelay);

  unsigned int LeadingBin();
  unsigned int TrailingBin();
  unsigned int MaxBinBlock();
  unsigned int FirstBinBlock();
  unsigned int LastBinBlock();
  unsigned int SumBins();

  double IntegralAbove(double AThreshold);
  double IntegralBelow(double AThreshold);

  // Returns bin with smallest value, if there is no bin which has at least 0.8 of it (returns -1 then)
  double MinimumPos();
  // Returns bin with biggest value, if there is no bin which has at least 0.8 of it (returns -1 then)
  double MaximumPos();

  // Returns smallest bin content
  double Minimum();
  // Returns biggest bin content
  double Maximum();
  
  // Returns signal for a given bin
  double GetSignalBin(unsigned int ABin);
  void SetSignalBin(unsigned int ABin,double AValue);
  void AddSignalBin(unsigned int ABin,double AValue);

  // Returns width in ns of a bin
  double GetSignalBinTime();
  void SetSignalBinTime(double ASignalBinTime);
  // Returns number of bins
  unsigned int GetSignalBins();
  // Write some special include file for HSpice, converts from ns to s and multiplies amplitude with 1E-6
  void WriteHSpiceSignalInclude(std::string AFileName,
                                std::string AKeyWord,
                                std::string APotential1,
                                std::string APotential2);
  
  // Read some special include file for HSpice, reverts changes (back to ns and 1E6)
  void ReadHSpiceSignalInclude(std::string AFileName);
  
  // Reads in a HSpice result file, extracts the content of the variable
  // Will adjust length of the signal
  void ReadSignalFromHSpicelis(std::string AFileName,
                               std::vector<std::string> AVarNames,
                               unsigned int ASectionNumber=0);
  void Clear();
  void Shift(double AShift);
  void Flatten(unsigned int ABins);
  void Scale(double AScale);
  void Add(Signal& ASource);
  
  #ifdef USEROOT
  void Set(TF1 * AF1);
  void Add(TF1 * AF1,double AMinTime,double AMaxTime);
  #endif
  void AddExp(double AScale,double ACoeff,double AMinTime,double AMaxTime);

  double SignalCompare(Signal& ASignal);
  #ifdef USEROOT
  TGraph * Graph(double AScale=1);
  TH1D * H1D(double AScale=1);
  #endif

  void SetSignalUnit(std::string ASignalUnit);

  void Plot(std::string AFileName="");
  void SetSignalBins(unsigned int ASignalBins);
  void SetSignalLength(double ASignalLength);
  void ChangeSignalLength(double ASignalLength);
  void ChangeSignalBins(unsigned int ASignalBins);

  void CreateShaper(double AStages,double AScale,double ATimeConstant);
  void CreateBlocks(std::vector<double> Ax,std::vector<double> Ay);
  #ifdef USEGSL
  void CreateCSpline(std::vector<double> Ax,std::vector<double> Ay);
  #endif
  void CreateTriangle(double AAmplitude,double ARaisingTime,double AFallingTime);
  void CreateDeltaPulse(double ATime,double AHeight);
  void CreateHeaviside(double AStart,double AZeroValue,double AOneValue);
  void CreateDecayingHeaviside(double AStart,double AZeroValue,double AOneValue,double ADecayTime);
  void CreateConstant(double AValue);
  void CreateGauss(double ACenter,double AAMS,double AAmplitude);
  Signal(double ASignalBinTime);
  Signal();
  ~Signal();
  Signal& operator>>(Signal& ADest);

  friend Tools::ASCIIOutStream& operator<<(Tools::ASCIIOutStream& AStream,
                                           Signal& ASignal);

  friend Tools::ASCIIInStream& operator>>(Tools::ASCIIInStream& AStream,
                                          Signal& ASignal);
};

Tools::ASCIIOutStream& operator<<(Tools::ASCIIOutStream& AStream,
                                  Signal& ASignal);

Tools::ASCIIInStream& operator>>(Tools::ASCIIInStream& AStream,
                                 Signal& ASignal);

}
#endif
