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

#include "Signals/Signal.hpp"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <iostream>
#include <algorithm>
#include <TCanvas.h>
#include <TROOT.h>
#include <TH1D.h>
#include <TVirtualFFT.h>
#include <TMultiGraph.h>
#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TVectorD.h>

#ifdef USEGSL
#include <gsl/gsl_spline.h>
#endif

namespace Signals
{

Tools::ASCIIOutStream& operator<<(Tools::ASCIIOutStream& AStream,
                                  Signal& ASignal)
{
  AStream<<"SignalUnit"<<ASignal.FSignalUnit
         <<"TimeUnit"<<ASignal.FTimeUnit
         <<"BinTime"<<ASignal.FSignalBinTime
         <<"Size"<<(unsigned int)ASignal.FSignal.size();
  for(unsigned int i=0;i<ASignal.FSignal.size();++i) AStream<<ASignal.FSignal[i];
  return AStream;
}

Tools::ASCIIInStream& operator>>(Tools::ASCIIInStream& AStream,
                                 Signal& ASignal)
{
  unsigned int SignalBins;
  AStream>>"SignalUnit">>ASignal.FSignalUnit
         >>"TimeUnit">>ASignal.FTimeUnit
         >>"BinTime">>ASignal.FSignalBinTime
         >>"Size">>SignalBins;
  ASignal.FSignal.resize(SignalBins);
  for(unsigned int i=0;i<SignalBins;++i)
  {
    AStream>>ASignal.FSignal[i];
  }
  return AStream;
}

void Signal::AddExp(double AScale,double ACoeff,double AMin,double AMax)
{
  unsigned int MinBin=AMin/FSignalBinTime;
  unsigned int MaxBin=AMax/FSignalBinTime;
  if(MaxBin>=FSignal.size()) MaxBin=FSignal.size()-1;
  for(unsigned int i=MinBin;i<=MaxBin;++i)
  {
    FSignal[i] += AScale*exp(ACoeff*(i-MinBin)*FSignalBinTime);
  }  
}

void Signal::Add(TF1 * AF1,double AMin,double AMax)
{
  unsigned int MinBin=AMin/FSignalBinTime;
  unsigned int MaxBin=AMax/FSignalBinTime;
  if(MaxBin>=FSignal.size()) MaxBin=FSignal.size()-1;
  for(unsigned int i=MinBin;i<=MaxBin;++i)
  {
    FSignal[i] += AF1->Eval(i*FSignalBinTime);
  }
}

void Signal::Set(TF1 * AF1)
{
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    FSignal[i] = AF1->Eval(i*FSignalBinTime);
  }
}

void Signal::SaveCoefficientsToStream(Tools::ASCIIOutStream&)
{
  throw Tools::MsgException("Signal::SaveCoefficientsToStream : Coefficient streaming for pure signals not possible.");
}

void Signal::SaveCoefficientsToFile(std::string AFileName)
{
  Tools::ASCIIOutStream Stream(AFileName);
  SaveCoefficientsToStream(Stream);
}

void Signal::AdjustSignalEnviroment(Signal & ASource)
{
  FSignalUnit    = ASource.FSignalUnit;
  FTimeUnit      = ASource.FTimeUnit;
  FSignalBinTime = ASource.FSignalBinTime;
  FSignal.resize(ASource.FSignal.size());
}

Signal& Signal::operator>>(Signal& ADest)
{
  ADest.Apply(*this);
  return ADest;
}

void Signal::Apply(Signal&)
{
  throw Tools::MsgException("Signal::Apply : Cannot apply to simple signal without extension (operator >>)");
}

void Signal::Clear()
{
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    FSignal[i]=0;
  }
}

unsigned int Signal::LeadingBin()
{
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(FSignal[i]!=0) return i;
  }
  return 0;
}

// +1 hack for avoiding zero for legal hits
unsigned int Signal::TrailingBin()
{
  unsigned int i=FSignal.size();
  do{
    i--;
    if(FSignal[i]!=0) return (i+1);
  } while(i>0);
  return 0;
}

unsigned int Signal::LastBinBlock()
{
  unsigned int res=0;
  unsigned int i=FSignal.size()-1;
  while((i>=0) && (FSignal[i]==0)) --i;
  while((i>=0) && (FSignal[i]!=0))
  {
    --i;
    ++res;
  }
  return res;
}

unsigned int Signal::FirstBinBlock()
{
  unsigned int res=0;
  unsigned int i=0;
  while((i<FSignal.size()) && (FSignal[i]==0)) ++i;
  while((i<FSignal.size()) && (FSignal[i]!=0))
  {
    ++i;
    ++res;
  }
  return res;
}

unsigned int Signal::MaxBinBlock()
{
  unsigned int res=0;
  unsigned int cur=0;
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(FSignal[i]!=0)
    {
      cur++;
    }
    else
    {
      if(cur>res) res=cur;
      cur=0;
    }
  }
  if(cur>res) res=cur;
  return res;
}

unsigned int Signal::SumBins()
{
  unsigned int res=0;
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(FSignal[i]!=0) res++;
  }
  return res;
}

void Signal::Add(Signal& ASource)
{
  if(ASource.FSignalBinTime!=FSignalBinTime) throw "Signal::Add : Different signal bin time.";
  if(ASource.FSignal.size()!=FSignal.size()) throw "Signal::Add : Different signal bin count.";
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    FSignal[i] += ASource.FSignal[i];
  }
}

void Signal::CreateGauss(double ACenter,
                         double AAMS,
                         double AAmplitude)
{
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    double time=i*FSignalBinTime;
    FSignal[i]=AAmplitude*exp(pow((time-ACenter)/AAMS,2));
  }
}

void Signal::CreateShaper(double AStages, double AScale, double ATimeConstant)
{
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    FSignal[i]=AScale*pow(i*FSignalBinTime/(ATimeConstant*AStages),AStages)*exp(AStages-i*FSignalBinTime/ATimeConstant);
  }
}

double Signal::IntegralBelow(double AThreshold)
{
  double result=0;
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(FSignal[i]<AThreshold) result+=FSignal[i];
  }
  return result;
}

double Signal::IntegralAbove(double AThreshold)
{
  double result=0;
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(FSignal[i]>AThreshold) result+=FSignal[i];
  }
  return result;
}

void Signal::CreateBlocks(std::vector<double> Ax,std::vector<double> Ay)
{
  if(Ax.size()!=Ay.size()) throw "Signal::CreateBlocks : Different number of x and y";
  if(Ax.size()<2) throw "Signal::CreateBlocks : Insufficient number of points";
  for(unsigned int i=0;i<Ax.size();++i) Ax[i]/=FSignalBinTime;
  for(unsigned int i=0;i<Ax[0];++i) FSignal[i]=0;
  unsigned int index = 1;
  unsigned int pos   = 0;
  while(pos<FSignal.size())
  {
    if(pos>=Ax[index])
    {
      index++;
      if(index>=Ax.size()) break;
    }
    FSignal[pos]=Ay[index];
    pos++;
  }
  while(pos<FSignal.size())
  {
    FSignal[pos]=0;
    pos++;
  }
}

#ifdef USEGSL
void Signal::CreateCSpline(std::vector<double> Ax,std::vector<double> Ay)
{
  if(Ax.size()!=Ay.size()) throw "Signal::CreateCSpline : Different number of x and y";
  if(Ax.size()<2) throw "Signal::CreateCSpline : Insufficient number of points";
  if(FSignal.size()==0) throw "Signal::CreateCSpline : Cannot create spline in signal of 0 length.";
  // Move the x axis into bin coordinate system
  for(unsigned int i=0;i<Ax.size();++i) Ax[i]/=FSignalBinTime;

  gsl_interp_accel * acc=gsl_interp_accel_alloc();
  gsl_spline * spline=gsl_spline_alloc(gsl_interp_cspline,Ax.size());
  gsl_spline_init(spline,&Ax[0],&Ay[0],Ax.size());

  for(unsigned int i=0;i<Ax[0];++i) FSignal[i]=0;
  double FNonZeroStart = ceil(Ax[0]);
  double FNonZeroEnd   = Ax[Ax.size()-1];
  if(FNonZeroEnd<=FNonZeroStart) throw "Signal::CreateCSpline : Spline array has End<=Start.";
  if(FNonZeroStart>=FSignal.size()) FNonZeroStart=0;
  if(FNonZeroEnd>=FSignal.size()) FNonZeroEnd=FSignal.size()-1;

  for(unsigned int i=FNonZeroStart;i<FNonZeroEnd;++i)
  {
    FSignal[i]=gsl_spline_eval(spline,i,acc);
  }
  for(unsigned int i=FNonZeroEnd+1;i<FSignal.size();++i) FSignal[i]=0;
  gsl_spline_free(spline);
  gsl_interp_accel_free(acc);
}
#endif USEGSL

void Signal::CreateConstant(double AValue)
{
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    FSignal[i] = AValue;
  }
}

// Create triangle
// raising slope
// falling slope
// both share a certain maximum
// Therefore both are determined by the time they are active

void Signal::CreateTriangle(double AAmplitude,
                             double ARaisingTime, // in ns
                             double AFallingTime) // in ns
{
  unsigned int RaisingBins=floor(ARaisingTime/FSignalBinTime);
  unsigned int FallingBins=floor(AFallingTime/FSignalBinTime);
  unsigned int SuccRaisingBin=std::min(size_t(RaisingBins),FSignal.size());
  unsigned int SuccFallingBin=std::min(size_t(RaisingBins+FallingBins),FSignal.size());
  double RaisingSlope=AAmplitude/ARaisingTime;
  double FallingSlope=AAmplitude/AFallingTime;
  for(unsigned int i=0;i<SuccRaisingBin;++i)
  {
    FSignal[i] = i*RaisingSlope;
  }
  for(unsigned int i=RaisingBins;i<SuccFallingBin;++i)
  {
    FSignal[i] = AAmplitude-(i-RaisingBins)*FallingSlope;
  }
   for(unsigned int i=SuccFallingBin;i<FSignal.size();++i)
  {
    FSignal[i] = 0;
  }
}

double Signal::MinimumPos()
{
  double v=DBL_MAX;
  double result=0;
  double almostcount=0;
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(FSignal[i]<v)
    {
      v      = FSignal[i];
      result = i;
    }
  }
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(FSignal[i]<0.8*v) almostcount++;
  }
  if(almostcount>1) result=-1;
  return result;
}

double Signal::MaximumPos()
{
  double v=DBL_MIN;
  double result=0;
  double almostcount=0;
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(FSignal[i]>v)
    {
      v      = FSignal[i];
      result = i;
    }
  }
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(FSignal[i]>0.8*v) almostcount++;
  }
  if(almostcount>1) result=-1;
  return result;
}

double Signal::Minimum()
{
  double result=DBL_MAX;
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(FSignal[i]<result) result=FSignal[i];
  }
  return result;
}

double Signal::Maximum()
{
  double result=DBL_MIN;
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(FSignal[i]>result) result=FSignal[i];
  }
  return result;
}

void Signal::Flatten(unsigned int ABins)
{
  for(unsigned int i=0;i<ABins;++i)
  {
    FSignal[i] = FSignal[ABins];
  }
}

void Signal::Scale(double AScale)
{
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    FSignal[i] *= AScale;
  }
}

void Signal::Shift(double AShift)
{
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    FSignal[i] += AShift;
  }
}

double Signal::SignalCompare(Signal& ACompare)
{
  double result = 0;
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    result += pow(FSignal[i]-ACompare.FSignal[i],2);
  }
  return result;
}

unsigned int Signal::GetSignalBins()
{
  return FSignal.size();
}

void Signal::AddSignalBin(unsigned int ABin,double AValue)
{
  if(ABin>=FSignal.size()) throw Tools::MsgException("Signal::AddSignalBin : Access out of range.");
  FSignal[ABin]+=AValue;
}

void Signal::SetSignalBin(unsigned int ABin,double AValue)
{
  if(ABin>=FSignal.size()) throw Tools::MsgException("Signal::SetSignalBin : Access out of range.");
  FSignal[ABin]=AValue;
}

double Signal::GetSignalBin(unsigned int ABin)
{
  if(ABin>=FSignal.size()) throw Tools::MsgException("Signal::GetSignalBin Access out of range.");
  return FSignal[ABin];
}

void Signal::SetSignalBinTime(double ASignalBinTime)
{
  FSignalBinTime=ASignalBinTime;
}

double Signal::GetSignalBinTime()
{
  return FSignalBinTime;
}

void Signal::CopyFromVectors(std::vector<double> ATimes,std::vector<double> AFunc)
{
  if(ATimes.size()==0) throw "Signal::CopyFromVectors : No data";
  SetSignalLength(ATimes[ATimes.size()-1]+FSignalBinTime);
  unsigned int bin       = 0;
  double  amplitude;
  for(unsigned int i=0;i<ATimes.size()-2;++i)
  {
    amplitude = AFunc[i];
    while(bin*FSignalBinTime<ATimes[i+1]-FSignalBinTime/2)
    {
      if(bin<FSignal.size()) FSignal[bin]=amplitude;
      bin++;
    }
  }
  amplitude = AFunc[ATimes.size()-1];
  while(bin<FSignal.size())
  {
    if(bin<FSignal.size()) FSignal[bin] = amplitude;
    bin++;
  }
}

void Signal::ReadHSpiceSignalInclude(std::string AFileName)
{
  std::cout<<"Signal::ReadHSpiceSignalInclude "<<AFileName<<std::endl;
  std::ifstream f;
  f.open(AFileName.c_str());
  if(!f.is_open()) throw "Signal::ReadHSpiceSignalInclude : File not open";
  std::string s;
  std::vector<double> times;
  std::vector<double> func;
  getline(f,s);
  while(1)
  {
    if(!f.good()) throw "Signal::ReadHSpiceSignalInclude : Unexpected end of file";
    getline(f,s);
    std::vector<std::string> data=SplitString(s);
    if(data.size()<1) throw "Signal::ReadHSpiceSignalInclude : Unexpected empty line";
    if(data[0]==')') break;
    if(data[0]!='+') throw "Signal::ReadHSpiceSignalInclude : Expect + at beginning of each data line";
    if(data.size()<3) throw "Signal::ReadHSpiceSignalInclude : Incomplete data line";
    times.push_back(atof(data[1].c_str())*1E9);
    func.push_back(atof(data[2].c_str())*1E6);
  }
  f.close();
  CopyFromVectors(times,func);
}

void Signal::WriteHSpiceSignalInclude(std::string AFileName,
                                       std::string AKeyWord,
                                       std::string APotential1,
                                       std::string APotential2)
{
  std::ofstream f;
  f.open(AFileName.c_str());
  f<<AKeyWord<<" "<<APotential1<<" "<<APotential2<<" pwl("<<std::endl;
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    f<<"  +  "<<i*FSignalBinTime*1E-9<<" "<<FSignal[i]*1E-6<<std::endl;
  }
  f<<")"<<std::endl;
  f.close();
}

void Signal::ReadSignalFromHSpicelis(std::string AFileName,
                                      std::vector<std::string> AVarNames,
                                      unsigned int ASectionNumber)
{
  if(AVarNames.size()>3) throw "Signal::ReadSignalFromHSpicelis : Cannot have more than 3 var names";
  std::vector<double> times;
  std::vector<double> func;
  ifstream f;
  f.open(AFileName.c_str());
  if(!f.is_open()) throw "Signal::ReadSignalFromHSpicelis : Cannot open file.";
  std::string s;
  while(f.good())
  {
    getline(f,s);
    if(s.find("******  transient analysis")!=std::string::npos)
    {
      if(ASectionNumber==0) break;
      ASectionNumber--;
    }
  }
  if(!f.good()) throw "Signal::ReadSignalFromHSpicelis : Section not found.";
  while(f.good())
  {
    getline(f,s);
    if(s.find("******  transient analysis")!=std::string::npos) throw "Signal::No more tables, variable not found.";
    FilterCharacter(s,'\r');
    if(s=="x")
    {
      getline(f,s);
      std::string s1;
      std::string s2;
      signed int index    = -1;
      unsigned int VarComponents = 1;
      getline(f,s1);
      std::vector<std::vector<std::string> > vars;
      std::vector<std::string> varc=SplitString(s1);
      vars.push_back(varc);
      getline(f,s2);
      varc = TriggeredSplitString(s2,s1);
      if(HasNonEmpty(varc))
      {
        VarComponents++;
        vars.push_back(varc);
        if(AVarNames.size()>2) // An alternative would be to ask wether or not this line contains numbers and push it back (URGH)
        {
          std::string s3;
          getline(f,s3);
          varc=TriggeredSplitString(s3,s1);
          if(HasNonEmpty(varc))
          {
            VarComponents++;
            vars.push_back(varc);
          }
        }
      }
      if(AVarNames.size()==vars.size())
      {
        for(unsigned int i=0;i<vars[0].size();++i)
        {
          bool equaltest=true;
          for(unsigned int component=0;component<vars.size();++component)
          {
            equaltest = (equaltest && (vars[component][i]==AVarNames[component]));
          }
          if(equaltest)
          {
            index=i;
            break;
          }
        }
      }
      getline(f,s);
      while(f.good())
      {
        getline(f,s);
        FilterCharacter(s,'\r');
        if(s=="y") break;
        std::vector<std::string> data=SplitString(s);
        if((index>=0) && ((unsigned int)index<data.size()))
        {
          times.push_back(atof(data[0].c_str())*1E9);
          func.push_back(atof(data[index].c_str()));
        }
      }
      if(index>=0)
      {
        CopyFromVectors(times,func);
        f.close();
        return;
      }
    } // End of "if s=="x", therefore end of table
  } // End of while(1) loop to go through all sections
  f.close();
  throw "Signal::ReadSignalFromHSpicelis : Unexpected end of file";
}

TH1D * Signal::H1D(double AScaling)
{
  TH1D * result=new TH1D("signal","signal",FSignal.size(),0,(FSignal.size()-1)*FSignalBinTime);
  result->SetDirectory(0);
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    result->SetBinContent(i,AScaling*FSignal[i]);
  }
  return result;
}

TGraph * Signal::Graph(double AScaling)
{
  double * Times     = new double[FSignal.size()];
  double * ModValues = new double[FSignal.size()];
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    Times[i]     = i*FSignalBinTime;
    ModValues[i] = AScaling*FSignal[i];
  }
  TGraph * graph=new TGraph(FSignal.size(),Times,ModValues);
  graph->GetXaxis()->SetTitle(FTimeUnit.c_str());
  graph->GetYaxis()->SetTitle(FSignalUnit.c_str());
  delete [] Times;
  return graph;
}

void Signal::Plot(std::string AFileName)
{
  TGraph * graph=Graph();
  TCanvas * canvas=0;
  canvas=new TCanvas();
  graph->Draw("AL");
//  canvas->Update();
  if(AFileName!="")
  {
    canvas->Print(AFileName.c_str());
    delete graph;
    delete canvas;
  }
}

void Signal::SetSignalBins(unsigned int ASignalBins)
{
  FSignal.resize(ASignalBins);

  for(unsigned int i=0;i<FSignal.size();++i) FSignal[i]=0;
}

void Signal::ChangeSignalBins(unsigned int ASignalBins)
{
  double OldSignalBins=FSignal.size();

  FSignal.resize(ASignalBins);

  for(unsigned int i=OldSignalBins;i<ASignalBins;++i) FSignal[i]=0;
}

void Signal::ChangeSignalLength(double ASignalLength)
{
  double OldSignalBins=FSignal.size();

  double SignalBins = ASignalLength/FSignalBinTime;
  if(SignalBins*FSignalBinTime<ASignalLength) SignalBins++;

  FSignal.resize(SignalBins);
  for(unsigned int i=OldSignalBins;i<FSignal.size();++i) FSignal[i]=0;
}

void Signal::SetSignalLength(double ASignalLength)
{

  double SignalBins = ASignalLength/FSignalBinTime;

  if(SignalBins*FSignalBinTime<ASignalLength) SignalBins++;

  FSignal.resize(SignalBins);

  for(unsigned int i=0;i<FSignal.size();++i) FSignal[i]=0;
}

void Signal::CreateDeltaPulse(double ATime,double AHeight)
{
  unsigned int bin=ATime/FSignalBinTime;
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    if(i==bin)
    {
      FSignal[i]=AHeight;
    }
    else
    {
      FSignal[i]=0;
    }
  } 
}

void Signal::CreateDecayingHeaviside(double AStart,double AZeroValue,double AOneValue,double ADecayTime)
{
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    double Time=i*FSignalBinTime;
    if(Time<AStart)
    {
      FSignal[i]=AZeroValue;
    }
    else
    {
      FSignal[i]=AZeroValue+(AOneValue-AZeroValue)*exp((AStart-Time)/ADecayTime);
    }
  }
}

void Signal::CreateHeaviside(double AStart,double AZeroValue,double AOneValue)
{
  for(unsigned int i=0;i<FSignal.size();++i)
  {
    double Time=i*FSignalBinTime;
    if(Time<AStart)
    {
      FSignal[i]=AZeroValue;
    }
    else
    {
      FSignal[i]=AOneValue;
    }
  }
}

void Signal::SetSignalUnit(std::string ASignalUnit)
{
  FSignalUnit=ASignalUnit;
}

void Signal::Initialize()
{
  FSignalUnit       = "V";
  FTimeUnit         = "ns";
}

Signal::Signal()
{
  FSignalBinTime = 0;
  Initialize();
}

Signal::Signal(double ASignalBinTime)
{
  FSignalBinTime = ASignalBinTime;
  Initialize();
}

Signal::~Signal()
{
}

}
