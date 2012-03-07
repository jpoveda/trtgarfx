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
#include "Signals/HighPass.hpp"
#include "tools.hpp"
#include <TApplication.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TMultiGraph.h>
#include <TRandom.h>
#include <TRandom3.h>

#include "boost/filesystem.hpp"


const std::string TopDir="/home/cluster/kn2a1/lu32cud/";

std::vector<std::string> DestVar;
std::vector<std::string> SourceVar;

const Real SignalBins   = 64;
const Real SignalBinDiv = 256;

Signals::SignalPairSet signalpairs;

void LoadSignals(std::string ASignalIncPath,std::string ASignalLis,std::vector<DWord> ASections)
{
  for(DWord i=0;i<ASections.size();++i)
  {
    Signals::Signal signal(3.125/SignalBinDiv);
    Signals::Signal dest(3.125/SignalBinDiv);
    signal.ReadSignalFromHSpicelis(ASignalLis,SourceVar,ASections[i]);
    signal.Shift(-signal.GetSignalBin(0));
    dest.ReadSignalFromHSpicelis(ASignalLis,DestVar,ASections[i]);
    dest.Flatten(SignalBinDiv);
    dest.Shift(-dest.GetSignalBin(0));
    signalpairs.AddSignalPair(signal,dest);
  }
}

int main(int argc,char * argv[])
{
  try
  {
    TApplication application("app",&argc,argv);
    InitRoot();
    TRandom3 * random=new TRandom3();

    SourceVar.push_back("blr_out");
    DestVar.push_back("trkdisc");

    std::vector<DWord> sections;
    sections.push_back(0);
    sections.push_back(1);
    sections.push_back(2);
    
    LoadSignals(TopDir+"sigs/ScaledSig/",
                TopDir+"sigs/ASDSigScaled.lis",
                sections);
                
    Real Bestd=1e30; TCanvas * canvas=new TCanvas();
    std::cout<<"RUN"<<std::endl;

    Signals::HighPass DiscIntegratorLow(3.125/SignalBinDiv);

    DiscIntegratorLow.FDeltaCoefficients.resize(2);
    DiscIntegratorLow.FDecayCoefficients.resize(2);
    DiscIntegratorLow.FDeltaCoefficients[0] = 0;
    DiscIntegratorLow.FDecayCoefficients[0] = 0;

    signalpairs.SetPlotScales(1,5,5);
    signalpairs.SetPlotColors(kBlack,kCyan,kBlue);

    for(DWord mc=0;mc<10000000;++mc)
    {
      DiscIntegratorLow.FDeltaCoefficients[1] = 5E-3*random->Rndm();
      DiscIntegratorLow.FDecayCoefficients[1] = 5E-3*random->Rndm();
      DiscIntegratorLow.FDelay                = random->Rndm()*2;

      if(signalpairs.IterReset())
      {
        do
        {
          signalpairs.SetCurrentResult(signalpairs.GetCurrentInput()>>DiscIntegratorLow);
        } while(signalpairs.IterNext());
        Real d=signalpairs.CompareAll();
        if(d<Bestd)
        {
          Bestd=d;
          std::cout<<"*** "<<Bestd<<" ***"<<std::endl;
          DiscIntegratorLow.Debug();
          DiscIntegratorLow.SaveCoefficientsToFile("LowInt"+IntToStr(mc)+".dat");
          signalpairs.PlotAll(canvas);
        }
      }
    }
    application.Run(kTRUE);
  }
  catch(const char * e)
  {
    std::cout<<e<<std::endl;
    throw;
  }

  return 0;
}
