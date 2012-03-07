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

// Standard Library Headers
#include <iostream>
#include <stdlib.h>
// Root Library headers
#include <TROOT.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
// Garfield Library Headers
#include "MediumMagboltz.hh"
#include "GeometrySimple.hh"
#include "SolidTube.hh"
#include "ComponentAnalyticField.hh"
#include "ViewCell.hh"
#include "TrackHeed.hh"
#include "Sensor.hh"
#include "AvalancheMC.hh"
#include "AvalancheMicroscopic.hh"
#include "ViewSignal.hh"
#include "ViewDrift.hh"
// Own Headers
#include "tools.hpp"
#include "Signals/Signal.hpp"

using namespace Garfield;

namespace TRT
{

enum SIonCalculationMethod {IonCalculationMethodMicroscopic,IonCalculationMethodLookup};
enum SElectronCalculationMethod {ElectronCalculationMethodMicroscopic,ElectronCalculationMethodMonteCarlo};

//*****************************************************************************
// StrawAttenuation:
//  Simulates the signal propagation in the straw assuming the signal is split
//  into two components which travel in opposit direction at a speed of
//  SignalSpeed.
//  Both signal components are attenuated over traveled distance l using
//  exp(-l/AttenuationLength)
//  Time delay is l/SignalSpeed
// Units:
//  StrawLength       : meters
//  SignalSpeed       : meters/second
//  AttenuationLength : meters
//  StrawPos          : meters
//  SignalBinTime     : nano seconds
//*****************************************************************************
class StrawAttenuation:public Signals::Signal
{
  private:
  double FStrawLength;
  double FSignalSpeed;
  double FAttenuationLength;
  double FStrawPos;
  protected:
  virtual void Apply(Signals::Signal& ASource);
  public:
  StrawAttenuation(double ASignalBinTime,
                   double AStrawLength,
                   double ASignalSpeed,
                   double AAttenuationLength,
                   double AStrawPos);
};

//*****************************************************************************
// Straw:
//  Implements a simulation of a TRT straw
//*****************************************************************************
class Straw
{
  private:
  MediumMagboltz *         FGas;
  GeometrySimple *         FGeometry;
  SolidTube *              FStrawTube;
  ComponentAnalyticField * FComponentAnalyticField;
  Sensor *                 FSensor;
  AvalancheMC *            FAvalanche;
  AvalancheMicroscopic *   FAvalancheMicroscopic;
  AvalancheMC *            FDrift;
  TrackHeed *              FTrack;
  double                     FStrawTubeRadius;
  double	                   FStrawTubeAnodeRadius;
  unsigned int                    FSignalBins;     // Number of bins
  double                     FSignalTime;     // in Nanoseconds
  double                     FSignalBinTime;  // Time of a single bin in ns
  unsigned int                    FElectronsAtWire;
  bool                     FCalculateMicroscopic;
  double *                   FIonLookup;      // Contains precalculated signals for ion drift
  double                     FIonLookupStep;  // Radius Steps for Ion Signal
  unsigned int                    FIonLookupSize;  // Number of Steps

  Signals::Signal          FSignal;
  Signals::Signal          FElectronSignal;
  Signals::Signal          FIonSignal;

  std::string              FParticle;
  double                     FPlotXStart;
  double                     FPlotXEnd;
  double                     FPlotYStart;
  double                     FPlotYEnd;
  bool                     FPlotXRangeSet;
  bool                     FPlotYRangeSet;
  double                     FAttenuationLength; // in m
  double                     FSignalSpeed;       // in m/s
  double                     FStrawLength;       // in m
  double                     FSignalScalingFactor; // Unitless
  
  void ClearSignal();
  void AddIonLookupSignal(double ADistance, // cm
                          double ATime);    // ns
  void ProcessElectronSignal();
  
  public:
  void PlotFieldContour(std::string AFileName);
  void SetSignalScalingFactor(double ASignalScalingFactor);
  void SetPlotXRange(double AStart,double AEnd);
  void SetPlotYRange(double AStart,double AEnd);
  double GetSignalAtBin(unsigned int ABin);
  double GetIonSignalAtBin(unsigned int ABin);
  double GetElectronSignalAtBin(unsigned int ABin);
  void EnableMicroscopicCalculation();
  unsigned int GetElectronsAtWire();

  Straw(std::string AParticle, // Particle to be used
        double ABinTime,         // Length of a single bin in ns
        unsigned int ABins);           // Number of such bins
  ~Straw();

  Signals::Signal& GetSignal();
  Signals::Signal& GetElectronSignal();
  Signals::Signal& GetIonSignal();

  double GetStoppingPower(double AMomentum); // Carefull, this only works after a CalcTrack
  
  void SetIonDriftDistanceStep(double ADistance); // Propably in cm
  void EnableIonSignal();
  void DisableIonSignal();
  void PlotCell(TCanvas * ACanvas);

  // Compares currently present Ion Table to a file for identity (debugging!)
  void CheckIonLookupTable(std::string AFileName);
  void SaveIonLookupTable(std::string AFileName);
  void LoadIonLookupTable(std::string AFileName);
  void CalcIonLookupTable(double AStepSize);
  
  void CalcIonPulse(double Ax,
                    double Ay,
                    double Az,
                    double ATime,
                    unsigned int AIonCount,
                    bool AIgnoreLookup=false);
  
  void CalcPulse(double Ax,
                 double Ay,
                 double Az,
                 double Atime,
                 double Aee,
                 double Adx,
                 double Ady,
                 double Adz,
                 unsigned int AElectronCount,
                 bool AClearSignal=true,
                 unsigned int ACountStart=0);

  void CalcTrack(double AEnergy,        // Particle Energy in GeV
                 double ADistance,      // Distance from the anode in cm
                 double & aenergyloss,  // Var: Energy loss of the particle
                 unsigned int & atrackelectrons, // Var: Number of electrons by ionization
                 unsigned int & AClusters,       // Var: Number of clusters of electrons
                 bool ACalcSignal=true);

  unsigned int GetSignalBins();
  double GetSignalBinTime();
};

}
