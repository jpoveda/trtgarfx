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
    
    boost::filesystem::create_directory(Directory.c_str());

    TF1 * gaussfit=new TF1("mygauss","[0]*exp([1]*(x-20.5)^2)",0,3.125*256);
    gaussfit->SetParameter(0,-0.18);
    gaussfit->SetParameter(1,-0.12);
    Signals::Signal gauss(3.125/SignalBinDiv);
    gauss.SetSignalLength(3.125*SignalBins);
    gauss.Set(gaussfit);
    Signals::Signal exp(3.125/SignalBinDiv);
    exp.SetSignalLength(3.125*SignalBins);
    exp.Clear();
    // Add Exponential component to each "part" of the "electron" gauss
    double gaussint=gauss.IntegralBelow(0);
    std::cout<<"Create Exp"<<std::endl;
    for(unsigned int i=0;i<gauss.GetSignalBins();++i)
    {
      exp.AddExp(gauss.GetSignalBin(i)*(-4.9645E-02/gaussint),-3.85286E-03,i*gauss.GetSignalBinTime(),3.125*256);
      exp.AddExp(gauss.GetSignalBin(i)*(-4.42067E-02/gaussint),-2.17964E-02,i*gauss.GetSignalBinTime(),3.125*256);
    }
    std::cout<<"::::"<<std::endl;
    Signals::Signal res(3.125/SignalBinDiv);
    res.SetSignalLength(3.125*SignalBins);
    res.Clear();
    res.Add(gauss);
    res.Add(exp);
    std::cout<<"Integral for res:"<<res.IntegralBelow(0)<<"+"<<res.IntegralAbove(0)<<std::endl;
    Tools::ASCIIOutStream batch(Directory+"/exec.sh");
    batch<<"#!/bin/bash";
    unsigned int id=0;
    for(double scale=1;scale<10000;scale*=1.5)
    {
      std::cout<<"Scale:"<<scale<<std::endl;
      Signals::Scale scaled(3.125);
      scaled.FScaleCoefficients.resize(2);
      scaled.FScaleCoefficients[0]=0;
      scaled.FScaleCoefficients[1]=scale;
      res>>scaled;
      scaled.WriteHSpiceSignalInclude(Directory+"/"+IntToStr(id)+".inc","isource","n1a","0");
      batch<<("cp "+IntToStr(id)+".inc $HSPICEINCLUDE").c_str();
      batch<<("hspice $HSPICEHSP >"+IntToStr(id)+".out").c_str();
      std::cout<<"Integral:"<<scaled.IntegralBelow(0)<<"+"<<scaled.IntegralAbove(0)<<std::endl;
      id+=1;
    }
    batch<<"rm $HSPICEINCLUDE";
    batch.Close();
    chmod((Directory+"/exec.sh").c_str(),0x1ff);
  }
  catch(const char * e)
  {
    std::cout<<e<<std::endl;
    throw;
  }
  return 0;
}
