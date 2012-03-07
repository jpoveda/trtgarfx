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

Signals::SignalPairSet signalpairs;

const std::string TopDir="/home/cluster/kn2a1/lu32cud/";

std::vector<std::string> DestVar;
std::vector<std::string> SourceVar;

const Real SignalBins   = 64;
const Real SignalBinDiv = 256;

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
    DestVar.push_back("trdisc");

    std::vector<DWord> sections;
    sections.push_back(0);
    sections.push_back(1);
    sections.push_back(2);
    
    LoadSignals(TopDir+"sigs/ScaledSig/",
                TopDir+"sigs/ASDSclWtrk_tr.lis",
                sections);
                
    Real Bestd=1e30;
    TCanvas * canvas=new TCanvas();
    std::cout<<"RUN"<<std::endl;
    
    Signals::HighPass DiscIntegratorHigh(3.125/SignalBinDiv);

    Real FDelay;

    DiscIntegratorHigh.FDeltaCoefficients.resize(2);
    DiscIntegratorHigh.FDecayCoefficients.resize(2);
    DiscIntegratorHigh.FDeltaCoefficients[0] = 0;
    DiscIntegratorHigh.FDecayCoefficients[0] = 0;
    
    signalpairs.SetPlotScales(1,50,50);
    signalpairs.SetPlotColors(kBlack,kCyan,kBlue);

    for(DWord mc=0;mc<10000000;++mc)
    {
      DiscIntegratorHigh.FDeltaCoefficients[1] = 8E-5+3E-5*random->Rndm();
      DiscIntegratorHigh.FDecayCoefficients[1] = 5E-3*random->Rndm();
      DiscIntegratorHigh.FDelay                = random->Rndm()*2;

      if(signalpairs.IterReset())
      {
        do
        {
          signalpairs.SetCurrentResult(signalpairs.GetCurrentInput()>>DiscIntegratorHigh);
        } while(signalpairs.IterNext());
        Real d=signalpairs.CompareAll();
        if(d<Bestd)
        {
          Bestd=d;
          std::cout<<"*** "<<Bestd<<" ***"<<std::endl;
          DiscIntegratorHigh.Debug();
          DiscIntegratorHigh.SaveCoefficientsToFile("HighInt"+IntToStr(mc)+".dat");
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
