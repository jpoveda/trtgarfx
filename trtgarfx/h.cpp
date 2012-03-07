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

#include "Tools/Stream.hpp"
#include "Signals/Signal.hpp"
#include "Signals/SignalSet.hpp"
#include "Signals/Discriminate.hpp"
#include "Signals/Rebin.hpp"

#include <TRandom3.h>
#include <TApplication.h>
#include <TFile.h>
#include <iostream>
#include <TCanvas.h>

const unsigned int SignalBins    = 64;
const unsigned int SignalBinDiv  = 256;

TH1D * lowdist;
TH1D * highdist;

Signals::Signal signal;

void ProcessSignal()
{
  signal.Scale(3);
  signal.WriteHSpiceSignalInclude("../ASDBLR/hspice.inc","isource","n1a","0");

  system("hspice ../ASDBLR/ASDSigs.hsp >hspice.out");

  Signals::Signal Ternary(0.5);
  std::vector<std::string> TernaryVar;
  TernaryVar.push_back("ternary");
  TernaryVar.push_back("");
  TernaryVar.push_back("");
  Ternary.ReadSignalFromHSpicelis("hspice.out",TernaryVar);
  Ternary.Shift(-Ternary.GetSignalBin(0));
  Signals::Discriminate disc(0.5,0.4);
  Signals::Rebin rebin(25);
  Signals::Discriminate lowdisc(0.5,0.1);
  Signals::Rebin lowrebin(3.125);
  Ternary>>disc>>rebin;
  Ternary>>lowdisc>>lowrebin;
  std::cout<<"FirstBins:"<<rebin.FirstBinBlock()<<std::endl;
  std::cout<<"LowFirstBins:"<<lowrebin.FirstBinBlock()<<std::endl;
  std::cout<<"Ternary"<<Ternary.IntegralBelow(0)<<std::endl;
  std::cout<<lowdist<<std::endl;
  lowdist->Fill(lowrebin.FirstBinBlock());
  std::cout<<highdist<<std::endl;
  highdist->Fill(rebin.FirstBinBlock());
  std::cout<<"FILL//"<<std::endl;
}

TRandom3 rndmgen(0);

int rndm(int range)
{
  int x;
  do
  {
    x=rndmgen.Rndm()*range;
  } while(x==range);
  return x;
}

int main(int argc,char * argv[])
{
  try
  {
  Signals::SignalSet signalset1("");
  Signals::SignalSet signalset2("");
  Signals::SignalSet signalset3("");
//  TApplication application("app",&argc,argv);
  Tools::ASCIIInStream f1("pion5GeV0.1cm.sigs");
  f1>>signalset1;
  f1.Close();
  Tools::ASCIIInStream f2("pion5GeV0.1cm.sigs");
  f2>>signalset2;
  f2.Close();
  Tools::ASCIIInStream f3("pion5GeV0.15cm.sigs");
  f3>>signalset3;
  f3.Close();

  TFile r("toht.root");
  if(!r.IsZombie())
  {
    lowdist=(TH1D*)r.Get("lowdist");
    highdist=(TH1D*)r.Get("highdist");
    lowdist->SetDirectory(0);
    highdist->SetDirectory(0);
    std::cout<<"READ PREVIOUS HISTOGRAMS"<<std::endl;
  }
  else
  {
    lowdist=new TH1D("lowdist","lowdist",19,0,19);
    highdist=new TH1D("highdist","highdist",19,0,19);
  }
  r.Close();

  unsigned int size1=signalset1.GetGroupCount();
  unsigned  int size2=signalset2.GetGroupCount();
  unsigned int size3=signalset3.GetGroupCount();

  for(unsigned int i=0;i<1000;++i)
  {
    int s1=rndm(3);
    int s2=rndm(3);
    std::cout<<"s1:"<<s1<<" s2:"<<s2<<std::endl;
    switch(s1)
    {
      case 0:
        signal=signalset1.GetSignal(rndm(size1),"signal");
        break;
      case 1:
        signal=signalset2.GetSignal(rndm(size2),"signal");
        break;
      case 2:
        signal=signalset3.GetSignal(rndm(size3),"signal");
        break;
    }
    switch(s2)
    {
      case 0:
        signal.Add(signalset1.GetSignal(rndm(size1),"signal"));
        break;
      case 1:
        signal.Add(signalset2.GetSignal(rndm(size2),"signal"));
        break;
      case 2:
        signal.Add(signalset3.GetSignal(rndm(size3),"signal"));
        break;
    }
    ProcessSignal();
  }
  
  TCanvas * canvas=new TCanvas;
  lowdist->GetXaxis()->SetTitle("ToT/3.125ns");
  lowdist->GetYaxis()->SetTitle("Entries");
  lowdist->Draw("");
  canvas->Update();
  canvas->Print("ToLTDist.pdf");
  highdist->GetXaxis()->SetTitle("ToT/3.125ns");
  highdist->GetYaxis()->SetTitle("Entries");
  highdist->Draw("");
  canvas->Update();
  canvas->Print("TOHTDist.pdf");
  
  TFile f("toht.root","recreate");
  lowdist->Write("lowdist");
  highdist->Write("highdist");
  f.Close();
  }
  catch(const char *e)
  {
    std::cout<<e<<std::endl;
    throw;
  }

  return 0;
}
