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
#include <TStyle.h>

const unsigned int SignalBins    = 256;
const unsigned int SignalBinDiv  = 256;

unsigned int leadingb=0;
unsigned int trailingb=0;
unsigned int lastbinblock=0;

TCanvas * canvas;

unsigned int ProcessSignal(Signals::Signal& signal,std::string signalid)
{
  signal.Graph()->Draw("AL");
  canvas->Update();
  canvas->Print(("debs/"+signalid+"sig.pdf").c_str());
  signal.WriteHSpiceSignalInclude("../ASDBLR/hspice.inc","isource","n1a","0");

  system("hspice ../ASDBLR/ASDSigs.hsp >hspice.out");

  Signals::Signal Ternary(0.5);
  std::vector<std::string> TernaryVar;
  TernaryVar.push_back("ternary");
  TernaryVar.push_back("");
  TernaryVar.push_back("");
  Ternary.ReadSignalFromHSpicelis("hspice.out",TernaryVar);
  Ternary.Shift(-Ternary.GetSignalBin(0));
  Ternary.Graph()->Draw("AL");
  canvas->Update();
  canvas->Print(("debs/"+signalid+"ter.pdf").c_str());
  Signals::Discriminate disc(0.5,0.4);
  Signals::Rebin rebin(25);
  Signals::Discriminate lowdisc(0.5,0.1);
  Signals::Rebin lowrebin(3.125);
  Ternary>>disc>>rebin;
  Ternary>>lowdisc>>lowrebin;
  leadingb=disc.LeadingBin()*disc.GetSignalBinTime();
  trailingb=disc.TrailingBin()*disc.GetSignalBinTime();
  lastbinblock=disc.LastBinBlock()*disc.GetSignalBinTime();
  return disc.FirstBinBlock()*disc.GetSignalBinTime();
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

void CreateCubicPart(Signals::Signal& signal,double startx,double endx,
                     double startamplitude,double startderivate,
                     double endamplitude,double endderivate)
{
  double intervallength=endx-startx;
  startderivate*=intervallength;
  endderivate*=intervallength;
  double coeff0=startamplitude;
  double coeff1=startderivate;
  double coeff2=-3*startamplitude-2*startderivate+3*endamplitude-1*endderivate;
  double coeff3=2*startamplitude+1*startderivate-2*endamplitude+1*endderivate;
  std::cout<<coeff0<<":"<<coeff1<<":"<<coeff2<<":"<<coeff3<<std::endl;
  int startbin=startx/signal.GetSignalBinTime();
  int endbin=endx/signal.GetSignalBinTime();
  for(int i=startbin;i<endbin;++i)
  {
    double nx=(i-startbin)*signal.GetSignalBinTime()/intervallength;
    double value=coeff0+coeff1*nx+coeff2*nx*nx+coeff3*nx*nx*nx;
    signal.SetSignalBin(i,value);
  }
}

void CreateLSSignal(Signals::Signal& signal,double startx,double intv1,double intv2,double intv3,double 
amplitude,double expamplitude,double expdecay)
{
  signal.Clear();
  signal.AddExp(expamplitude,expdecay,startx+intv1+intv2+intv3,signal.GetSignalBins()*signal.GetSignalBinTime());
  CreateCubicPart(signal,startx,startx+intv1,0,0,amplitude,0);
  CreateCubicPart(signal,startx+intv1,startx+intv1+intv2,amplitude,0,amplitude,0);
  CreateCubicPart(signal,startx+intv1+intv2,startx+intv1+intv2+intv3,amplitude,0,expamplitude,expdecay*expamplitude);
}

int main(int argc,char * argv[])
{
  try
  {
    InitRoot();
    gStyle->SetOptStat(0);
//    TApplication application("app",&argc,argv);
    Signals::Signal sig(3.125/SignalBinDiv);
    sig.SetSignalLength(3.125*SignalBins);
    sig.SetSignalUnit("fC/0.5ns");
    CreateLSSignal(sig,10,10,15,20,
                   40,8,-0.1);
    std::cout<<"Integral :"<<sig.IntegralAbove(0)*sig.GetSignalBinTime()<<std::endl;
    TGraph * graph=sig.Graph();
    canvas=new TCanvas;
    graph->Draw("AL");
    canvas->Update();
    canvas->Print("CustomSignal.pdf");
    TH2D * charge=new TH2D("charge","charge",10,0,100,10,0,100);
    charge->GetXaxis()->SetTitle("plateau time/ns");
    charge->GetXaxis()->SetTitleOffset(1.5);
    charge->GetYaxis()->SetTitle("plateau amplitude/(fC/0.5ns)");
    charge->GetYaxis()->SetTitleOffset(1.5);
    charge->GetZaxis()->SetTitle("-total charge/pC");
    charge->GetZaxis()->SetTitleOffset(1.2);
    TH2D * toht=new TH2D("toht","toht",10,0,100,10,0,100);
    toht->GetXaxis()->SetTitle("plateau time/ns");
    toht->GetXaxis()->SetTitleOffset(1.5);
    toht->GetYaxis()->SetTitle("plateau amplitude/(fC/0.5ns)");
    toht->GetYaxis()->SetTitleOffset(1.5);
    toht->GetZaxis()->SetTitle("first time over threshold/ns");
    toht->GetZaxis()->SetTitleOffset(1.2);
    TH2D * leading=new TH2D("leading","leading",10,0,100,10,0,100);
    leading->GetXaxis()->SetTitle("plateau time/ns");
    leading->GetXaxis()->SetTitleOffset(1.5);
    leading->GetYaxis()->SetTitle("plateau amplitude/(fC/0.5ns)");
    leading->GetYaxis()->SetTitleOffset(1.5);
    leading->GetZaxis()->SetTitle("Leading high treshold time/ns");
    leading->GetZaxis()->SetTitleOffset(1.2);
    TH2D * trailing=new TH2D("trailing","trailing",10,0,100,10,0,100);
    trailing->GetXaxis()->SetTitle("plateau time/ns");
    trailing->GetXaxis()->SetTitleOffset(1.5);
    trailing->GetYaxis()->SetTitle("plateau amplitude/(fC/0.5ns)");
    trailing->GetYaxis()->SetTitleOffset(1.5);
    trailing->GetZaxis()->SetTitle("trailing high threshold time/ns");
    trailing->GetZaxis()->SetTitleOffset(1.2);
    TH2D * ltoht=new TH2D("ltoht","ltoht",10,0,100,10,0,100);
    ltoht->GetXaxis()->SetTitle("plateau time/ns");
    ltoht->GetXaxis()->SetTitleOffset(1.5);
    ltoht->GetYaxis()->SetTitle("plateau amplitude/(fC/0.5ns)");
    ltoht->GetYaxis()->SetTitleOffset(1.5);
    ltoht->GetZaxis()->SetTitle("last time over threshold/ns");
    ltoht->GetZaxis()->SetTitleOffset(1.2);
    for(unsigned int time=0;time<100;time+=10)
    {
      for(unsigned int amplitude=10;amplitude<100;amplitude+=10)
      {
        CreateLSSignal(sig,10,10,time,20,amplitude,amplitude*0.2,-0.1);
        sig.Scale(-1);
        charge->SetBinContent(time/10+1,amplitude/10+1,-sig.IntegralBelow(0)*sig.GetSignalBinTime()*1E-3);
        unsigned int resulttoht=ProcessSignal(sig,IntToStr(time)+"x"+IntToStr(amplitude));
        std::cout<<"set"<<time<<":"<<amplitude<<":"<<resulttoht<<std::endl;
        toht->SetBinContent(time/10+1,amplitude/10+1,resulttoht);
        leading->SetBinContent(time/10+1,amplitude/10+1,leadingb);
        trailing->SetBinContent(time/10+1,amplitude/10+1,trailingb);
        ltoht->SetBinContent(time/10+1,amplitude/10+1,lastbinblock);
      }
    }
    charge->Draw("lego1");
    canvas->Update();
    canvas->Print("charge.pdf");
    toht->Draw("lego1");
    canvas->Update();
    canvas->Print("toht.pdf");
    leading->Draw("lego1");
    canvas->Update();
    canvas->Print("leading.pdf");
    trailing->Draw("lego1");
    canvas->Update();
    canvas->Print("trailing.pdf");
    ltoht->Draw("lego1");
    canvas->Update();
    canvas->Print("ltoht.pdf");
//    application.Run(kTRUE);
  }
  catch(const char *e)
  {
    std::cout<<e<<std::endl;
    throw;
  }

  return 0;
}
