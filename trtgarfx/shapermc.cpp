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

#include <iostream>
#include <stdio.h>
#include <float.h>
#include "Signals/Signal.hpp"
#include "Signals/SignalPairSet.hpp"
#include "TRTElectronics/ASDBLRShaper.hpp"
#include "tools.hpp"
#include <TApplication.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TMultiGraph.h>
#include <TRandom.h>
#include <TRandom3.h>

#include "boost/filesystem.hpp"

Signals::SignalPairSet signalpairs;

const std::string TopDir="/home/cluster/kn2a1/lu32cud/";

const Real SignalBins   = 64;
const Real SignalBinDiv = 256;

std::vector<std::string> SourceVar;
std::vector<std::string> DestVar;

void Show(std::string ADesc,std::vector<Real> & AData)
{
  for(DWord i=0;i<AData.size();++i)
  {
    std::cout<<ADesc<<"["<<i<<"]"<<AData[i]<<std::endl;
  }
}

void LoadSignals(std::string ASignalIncPath,std::string ASignalLis,std::vector<DWord> ASections)
{
  for(DWord i=0;i<ASections.size();++i)
  {
    Signals::Signal source(3.125/SignalBinDiv);
    Signals::Signal dest(3.125/SignalBinDiv);
    source.ReadSignalFromHSpicelis(ASignalLis,SourceVar,ASections[i]);
    source.Shift(-source.GetSignalBin(0));
    source.Scale(1);
    dest.ReadSignalFromHSpicelis(ASignalLis,DestVar,ASections[i]);
    dest.Flatten(SignalBinDiv);
    dest.Shift(-dest.GetSignalBin(0));
    dest.Scale(1);
    signalpairs.AddSignalPair(source,dest);
  }
}

int main(int argc,char * argv[])
{
  try
  {
    TApplication application("app",&argc,argv);
    InitRoot();

    SourceVar.push_back("pre_out");

    DestVar.push_back("shaper_out");
    
    std::vector<DWord> sections;
    sections.push_back(0);
    sections.push_back(1);
    sections.push_back(2);


    LoadSignals(TopDir+"sigs/ScaledSig/",
                TopDir+"sigs/ASDSigScaled.lis",
                sections);
//    LoadSignals(TopDir+"sigs/PulseSignals4/",//"sigs/ScaledSig/",
//                TopDir+"sigs/ASDSig4.lis",//"sigs/ASDSclWtrk_tr.lis",
//                sections);

    std::cout<<"Load Complete"<<std::endl;

    TCanvas * canvas=new TCanvas;

    TRandom3 * random  = new TRandom3();
    Real Bestd=1e30;

    TRT::ASDBLRShaper Shaper(signalpairs.GetSignalBinTime());

    Shaper.F1ScaleCoefficients.resize(2);
    Shaper.F1DeltaCoefficients.resize(2);
    Shaper.F1DecayCoefficients.resize(2);

    Shaper.F2Delta1Coefficients.resize(2);
    Shaper.F2Decay1Coefficients.resize(2);
    Shaper.F2Delta2Coefficients.resize(2);
    Shaper.F2Decay2Coefficients.resize(2);
    Shaper.F2Weight2Coefficients.resize(2);
    Shaper.F2ScaleCoefficients.resize(4);
    Shaper.F2DiffCoefficients.resize(2);
    Shaper.F2DecayCoefficients.resize(2);

    std::cout<<"Init done"<<std::endl;
    for(DWord mc=0;mc<2000000;mc++)
    {
    
      Shaper.F1ScaleCoefficients[0]=0;
      Shaper.F1ScaleCoefficients[1]=2.2+0.1*random->Rndm();
      Shaper.F1DeltaCoefficients[0]=0;
      Shaper.F1DeltaCoefficients[1]=7E-3+1E-3*random->Rndm();
      Shaper.F1DecayCoefficients[0]=0;
      Shaper.F1DecayCoefficients[1]=0;

      Shaper.F2Delta1Coefficients[0] = 0;
      Shaper.F2Delta1Coefficients[1] = 2.75847E-3;
      Shaper.F2Decay1Coefficients[0] = 0;
      Shaper.F2Decay1Coefficients[1] = 1.95876E-4;
      Shaper.F2Delta2Coefficients[0] = 0;
      Shaper.F2Delta2Coefficients[1] = 6.77112E-3;
      Shaper.F2Decay2Coefficients[0] = 0;
      Shaper.F2Decay2Coefficients[1] = 9.05344E-4;
      Shaper.F2Weight2Coefficients[0] = 0;
      Shaper.F2Weight2Coefficients[1] = 1.12632;

      Shaper.F2ScaleCoefficients[0] = 0;
      Shaper.F2ScaleCoefficients[1] = 10+2*random->Rndm();
      Shaper.F2ScaleCoefficients[2] = 1E0+2E0*random->Rndm();
      Shaper.F2ScaleCoefficients[3] = 1E1*random->Rndm();
      Shaper.F2ScaleCoefficients[4] = 1E2*random->Rndm();
      Shaper.F2DiffCoefficients[0]=0;
      Shaper.F2DiffCoefficients[1]=1.8+0.6*random->Rndm();
      Shaper.F2DecayCoefficients[0]=0;
      Shaper.F2DecayCoefficients[1]=1.5E-5-1E-5*random->Rndm();
      Shaper.FDelay=2+1*random->Rndm();

      signalpairs.SetPlotScales(1,1,1,1);
      signalpairs.SetPlotColors(kBlack,kCyan,kBlue,kRed);

      if(signalpairs.IterReset())
      {
        do
        {
          signalpairs.SetCurrentResult(signalpairs.GetCurrentInput()>>Shaper);
//          signalpairs.SetCurrentExtra1(Shaper.F2LowPass);
        } while(signalpairs.IterNext());
        Real d=signalpairs.CompareAll();
        if((d<Bestd) || (0))
        {
          std::cout<<std::endl;
          Shaper.Debug();
          Shaper.SaveCoefficientsToFile("Shaper"+IntToStr(mc)+".dat");
          Bestd = d;
          std::cout<<"New best value:"<<Bestd<<std::endl;
          signalpairs.PlotAll(canvas);
        }
      }
    }
    std::cout<<"Done"<<std::endl;
    application.Run(kTRUE);
  }
  catch(const char * e)
  {
    std::cout<<e<<std::endl;
    throw;
  }

  return 0;
}
