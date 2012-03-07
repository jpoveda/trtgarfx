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
#include "TRTElectronics/ASDBLRBaselineRestorer.hpp"

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

    SourceVar.push_back("shaper_out");
    DestVar.push_back("blr_out");

    std::vector<DWord> sections;
    sections.push_back(0);
    sections.push_back(1);
    sections.push_back(2);
    
    LoadSignals(TopDir+"sigs/ScaledSig/",
                TopDir+"sigs/ASDSigScaled.lis",
                sections);

    TRT::ASDBLRBaselineRestorer BaselineRestorer(3.125/SignalBinDiv);

    Real Bestd=1e30;
    TCanvas * canvas=new TCanvas();
    BaselineRestorer.FTopStateCoefficients.resize(3);
    BaselineRestorer.FTopDiffCoefficients.resize(3);
    BaselineRestorer.FBottomStateCoefficients.resize(5);
    BaselineRestorer.FBottomDiffCoefficients.resize(3);

    signalpairs.SetPlotScales(1,1,1);
    signalpairs.SetPlotColors(kBlack,kCyan,kBlue);

    for(DWord mc=0;mc<10000000;++mc)
    {
      BaselineRestorer.FTopStateCoefficients[0]    = 0;
      BaselineRestorer.FTopStateCoefficients[1]    = (-4E-4-2E-4*random->Rndm());
      BaselineRestorer.FTopStateCoefficients[2]    = -1E-4*random->Rndm();
      BaselineRestorer.FTopDiffCoefficients[0]     = 0;
      BaselineRestorer.FTopDiffCoefficients[1]     = 0.9+0.1*random->Rndm();
      BaselineRestorer.FTopDiffCoefficients[2]     = 0*5E0*random->Rndm();
      BaselineRestorer.FBottomStateCoefficients[0] = 0;
      BaselineRestorer.FBottomStateCoefficients[1] = -1E-2*random->Rndm();
      BaselineRestorer.FBottomStateCoefficients[2] = -2E-1*random->Rndm();
      BaselineRestorer.FBottomStateCoefficients[3] = 0*random->Rndm();
      BaselineRestorer.FBottomStateCoefficients[4] = 0*random->Rndm();
      BaselineRestorer.FBottomDiffCoefficients[0]  = 0;
      BaselineRestorer.FBottomDiffCoefficients[1]  = BaselineRestorer.FTopDiffCoefficients[1];//0.7+0.3*random->Rndm();
      BaselineRestorer.FBottomDiffCoefficients[2]  = BaselineRestorer.FTopDiffCoefficients[2];//1E1*random->Rndm();
      BaselineRestorer.FDelay=random->Rndm()*0.5;
      
      if(signalpairs.IterReset())
      {
        do
        {
          signalpairs.SetCurrentResult(signalpairs.GetCurrentInput()>>BaselineRestorer);
        } while(signalpairs.IterNext());
        Real d=signalpairs.CompareAll();
        if(d<Bestd)
        {
          Bestd=d;
          std::cout<<"*** "<<Bestd<<" ***"<<std::endl;
          BaselineRestorer.Debug();
          BaselineRestorer.SaveCoefficientsToFile("BLR"+IntToStr(mc)+".dat"); 
          signalpairs.PlotAll(canvas);
        }
      }
    }
    std::cout<<"Done"<<std::endl;
//    application.Run(kTRUE);
  }
  catch(const char * e)
  {
    std::cout<<e<<std::endl;
    throw;
  }

  return 0;
}
