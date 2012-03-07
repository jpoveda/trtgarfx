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
#include "TRTElectronics/ASDBLRPreamplifier.hpp"
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

void LoadSignals(std::string ASignalIncPath,std::string ASignalLis,std::vector<DWord> ASections)
{
  for(DWord i=0;i<ASections.size();++i)
  {
    Signals::Signal source(3.125/SignalBinDiv);
    Signals::Signal dest(3.125/SignalBinDiv);
    source.ReadHSpiceSignalInclude(ASignalIncPath+"sig"+IntToStr(ASections[i])+".inc");
    dest.ReadSignalFromHSpicelis(ASignalLis,DestVar,ASections[i]);
    dest.Flatten(SignalBinDiv);
    dest.Shift(-dest.GetSignalBin(0));
    std::cout<<"::"<<source.GetSignalBins()<<":"<<dest.GetSignalBins()<<std::endl;
    source.ChangeSignalBins(dest.GetSignalBins());
    signalpairs.AddSignalPair(source,dest);
  }
}

int main(int argc,char * argv[])
{
  try
  {
    TApplication application("app",&argc,argv);
    InitRoot();

    DestVar.push_back("pre_out");
    
    std::vector<DWord> sections;
    sections.push_back(0);
    sections.push_back(1);
    sections.push_back(2);
    sections.push_back(3);
    sections.push_back(4);
    sections.push_back(5);

    LoadSignals(TopDir+"sigs/ScaledSig/",
                TopDir+"sigs/ASD_ScaledSigs_Aug15.lis",
                sections);

    TRandom3 * random  = new TRandom3();
    Real Bestd=1e30;
    TCanvas * canvas = new TCanvas();
    TRT::ASDBLRPreamplifier Preamp(signalpairs.GetSignalBinTime());

    Preamp.FStateInputCoefficients.resize(3);
    Preamp.FStateInputCoefficients[0]   = 0;
    Preamp.FStateInputDiffCoefficients.resize(3);
    Preamp.FStateInputCoefficients[0] = 0;
    Preamp.FStateDecayCoefficients.resize(4);
    Preamp.FStateDecayCoefficients[0] = 0;
    Preamp.FOutStateCoefficients.resize(3);
    Preamp.FOutStateCoefficients[0] = 0;
    Preamp.FOutStateDiffCoefficients.resize(2);
    Preamp.FOutStateDiffCoefficients[0] = 0;
    Preamp.FOutOutCoefficients.resize(2);
    Preamp.FOutOutCoefficients[0] = 0;

    signalpairs.SetPlotScales(0.005,1,1);
    signalpairs.SetPlotColors(kBlack,kCyan,kBlue);

    for(DWord mc=0;mc<2000000;mc++)
    {
      Preamp.FStateInputCoefficients[1]   = -1;
      Preamp.FStateInputCoefficients[2]   = 0;//0.01*random->Rndm();
      Preamp.FStateInputDiffCoefficients[1] = 0;//0.5*random->Rndm();
      Preamp.FStateInputDiffCoefficients[2] = 0;//1E-3*random->Rndm();
      Preamp.FStateDecayCoefficients[1]   = -1.9E-3-0.3E-3*random->Rndm();
      Preamp.FStateDecayCoefficients[2]   = 0*(0.5E-7+1E-7*random->Rndm());
      Preamp.FStateDecayCoefficients[3]   = 0;//1E-10*random->Rndm();
      Preamp.FOutStateCoefficients[1]     = -0.001*random->Rndm();
      Preamp.FOutStateCoefficients[2]     = 0*(-0.5E-6-1E-6*random->Rndm());
      Preamp.FOutStateDiffCoefficients[1] = 0;
      Preamp.FOutOutCoefficients[1]       = -0.02-0.04*random->Rndm();
      Preamp.FDelay=0.5*random->Rndm()+1;

      Real d = 0;
      if(signalpairs.IterReset())
      {
        do
        {
          signalpairs.SetCurrentResult(signalpairs.GetCurrentInput()>>Preamp);
        } while(signalpairs.IterNext());
        Real d=signalpairs.CompareAll();
        if(d<Bestd)
        {
          Bestd = d;
          Preamp.Debug();
          Preamp.SaveCoefficientsToFile("Preamp"+IntToStr(mc)+".dat");
          std::cout<<"New best value:"<<Bestd<<std::endl;
          delete canvas;
          canvas=new TCanvas("Name","Title",0,0,1024,1500);
          signalpairs.PlotAll(canvas);
        }
      }
    }
  }
  catch(const char * e)
  {
    std::cout<<e<<std::endl;
    throw;
  }

  return 0;
}
