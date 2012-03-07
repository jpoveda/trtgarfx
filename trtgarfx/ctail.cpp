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

#include "Signals/Scale.hpp"
#include "Signals/Discriminate.hpp"
#include "Signals/Rebin.hpp"
#include "TRT/Straw.hpp"
#include "Tools/Stream.hpp"
#include "sys/stat.h"
#include <TApplication.h>
#include <TStyle.h>
#include <TMultiGraph.h>

#include "boost/filesystem.hpp"

const unsigned int SignalBins    = 256;
const unsigned int SignalBinDiv  = 256;
const std::string Directory="Tailsigs";

int main(int argc,char * argv[])
{
 try
  {
    InitRoot();
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    
    unsigned int id=0;

    std::vector<std::string> TernaryVar;
    TernaryVar.push_back("ternary");
    TernaryVar.push_back("");
    TernaryVar.push_back("");
    std::vector<double> Charge;
    std::vector<double> ToT;

    for(double scale=1;scale<10000;scale*=1.5)
    {
      std::cout<<"Scale:"<<scale<<std::endl;
      Signals::Signal In(3.125);
      In.ReadHSpiceSignalInclude(Directory+"/"+IntToStr(id)+".inc");
      std::cout<<"Read Ternary"<<std::endl;
      Signals::Signal Ternary(3.125);
      Ternary.ReadSignalFromHSpicelis(Directory+"/"+IntToStr(id)+".out",TernaryVar);
      Ternary.Shift(-Ternary.GetSignalBin(0));
      std::cout<<"Create Objects"<<std::endl;
      Signals::Discriminate disc(3.125,0.4);
      Signals::Rebin rebin(25);
      Ternary>>disc>>rebin;
      std::cout<<"Push values"<<std::endl;
      Charge.push_back(-In.IntegralBelow(0)-In.IntegralAbove(0));
      ToT.push_back(rebin.FirstBinBlock());
      id+=1;
    }
    TCanvas * canvas=new TCanvas;
    canvas->SetLogx(1);
    TGraph * gr=new TGraph(id,&Charge[0],&ToT[0]);
    gr->GetXaxis()->SetTitle("Integrated Current/fC");
    gr->GetYaxis()->SetTitle("High threshold ToT/25ns");
    gr->Draw("AL*");
    canvas->Update();
    canvas->Print("HighThresholdOverScale.pdf");
  }
  catch(const char * e)
  {
    std::cout<<e<<std::endl;
    throw;
  }
  return 0;
}
