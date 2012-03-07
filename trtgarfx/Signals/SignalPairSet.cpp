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

#include "Signals/SignalPairSet.hpp"
#include "Tools/Exception.hpp"

#ifdef USEROOT
#include <TMultiGraph.h>
#endif

namespace Signals
{

double SignalPairSet::CompareAll()
{
  double d=0;
  if(IterReset())
  {
    do
    {
      d+=FCurrentPair->FResult.SignalCompare(GetCurrentOutput());
    } while(IterNext());
  }
  return d;
}

void SignalPairSet::SetPlotScales(double AScaleInput,
                                  double AScaleOutput,
                                  double AScaleResult,
                                  double AScaleExtra1)
{
  FScaleInput  = AScaleInput;
  FScaleOutput = AScaleOutput;
  FScaleResult = AScaleResult;
  FScaleExtra1 = AScaleExtra1;
}

void SignalPairSet::SetPlotColors(unsigned int AColorInput,
                                  unsigned int AColorOutput,
                                  unsigned int AColorResult,
                                  unsigned int AColorExtra1)
{
  FColorInput  = AColorInput;
  FColorOutput = AColorOutput;
  FColorResult = AColorResult;
  FColorExtra1 = AColorExtra1;
}

#ifdef USEROOT
void SignalPairSet::PlotAll(TCanvas * canvas)
{
  if(canvas)
  {
    delete canvas;
    canvas = 0;
  }
  if(IterReset())
  {
    canvas=new TCanvas("SignalPairSet::PlotAll","SignalPairSet::PlotAll",0,0,1024,1400);
    canvas->Divide(1,GetSignalPairCount());
    unsigned int pos=1;
    do
    {
      canvas->cd(pos);
      TMultiGraph * multigraph=new TMultiGraph();
      if(FCurrentPair->FInput.GetSignalBins()!=0)
      {
        TGraph * inputgraph=FCurrentPair->FInput.Graph(FScaleInput);
        inputgraph->SetLineColor(FColorInput);
        multigraph->Add(inputgraph);
      }
      if(FCurrentPair->FOutput.GetSignalBins()!=0)
      {
        TGraph * outputgraph=FCurrentPair->FOutput.Graph(FScaleOutput);
        outputgraph->SetLineColor(FColorOutput);
        multigraph->Add(outputgraph);
      }
      if(FCurrentPair->FResult.GetSignalBins()!=0)
      {
        TGraph * resultgraph=FCurrentPair->FResult.Graph(FScaleResult);
        resultgraph->SetLineColor(FColorResult);
        multigraph->Add(resultgraph);
      }
      if(FCurrentPair->FExtra1.GetSignalBins()!=0)
      {
        TGraph * extra1graph=FCurrentPair->FExtra1.Graph(FScaleExtra1);
        extra1graph->SetLineColor(FColorExtra1);
        multigraph->Add(extra1graph);
      }
      multigraph->Draw("AL");
      canvas->Update();
      pos++;
    } while(IterNext());
  }
}
#endif

unsigned int SignalPairSet::GetSignalBins()
{
  return FSignalBins;
}

double SignalPairSet::GetSignalBinTime()
{
  return FSignalBinTime;
}

Signal& SignalPairSet::GetCurrentOutput()
{
  if(FCurrentPair==0) throw Tools::MsgException("SignalPairSet::GetCurrentOutput : No pair selected");
  return FCurrentPair->FOutput;
}

Signal& SignalPairSet::GetCurrentInput()
{
  if(FCurrentPair==0) throw Tools::MsgException("SignalPairSet::GetCurrentInput : No pair selected");
  return FCurrentPair->FInput;
}

Signal& SignalPairSet::GetCurrentExtra1()
{
  if(FCurrentPair==0) throw Tools::MsgException("SignalPairSet::GetExtraSignal1 : No pair selected");
  return FCurrentPair->FExtra1;
}

Signal& SignalPairSet::GetCurrentResult()
{
  if(FCurrentPair==0) throw Tools::MsgException("SignalPairSet::GetresultSignal : No pair selected");
  return FCurrentPair->FResult;
}

void SignalPairSet::SetCurrentResult(Signal& ASignal)
{
  if(FCurrentPair==0) throw Tools::MsgException("SignalPairSet::SetResultSignal : No pair selected");
  FCurrentPair->FResult=ASignal;
}

void SignalPairSet::SetCurrentExtra1(Signal& ASignal)
{
  if(FCurrentPair==0) throw Tools::MsgException("SignalPairSet::SetExtraSignal1 : No pair selected");
  FCurrentPair->FExtra1=ASignal;
}

bool SignalPairSet::IterNext()
{
  if(FCurrentPair==0) throw Tools::MsgException("SignalPairSet::IterNext : No pair selected");
  FCurrentPair=FCurrentPair->FNext;
  return(FCurrentPair!=0);
}

bool SignalPairSet::IterReset()
{
  FCurrentPair=FPairs;
  return(FCurrentPair!=0);
}

unsigned int SignalPairSet::GetSignalPairCount()
{
  struct SignalPair * Entry=FPairs;
  unsigned int result=0;
  while(Entry!=0)
  {
    result++;
    Entry=Entry->FNext;
  }
  return result;
}

void SignalPairSet::AddSignalPair(Signal& AInput,Signal& AOutput)
{
  if(AInput.GetSignalBinTime()!=AOutput.GetSignalBinTime()) throw Tools::MsgException("SignalPairSet::AddSignalPair : Input and Output have different bin time");
  if(AInput.GetSignalBins()!=AOutput.GetSignalBins()) throw Tools::MsgException("SignalPairSet::AddSignalPair : Input and Output have different bin count");
  if(FPairs)
  {
    if(AInput.GetSignalBinTime()!=FSignalBinTime) throw Tools::MsgException("SignalPairSet::AddSignalPair : Different bin time for new signal");
    if(AInput.GetSignalBins()!=FSignalBins) throw Tools::MsgException("SignalPairSet::AddSignalPair : Different bin count for new signal");
  }
  else
  {
    FSignalBinTime = AInput.GetSignalBinTime();
    FSignalBins    = AInput.GetSignalBins();
  }
  FCurrentPair = new struct SignalPair;
  FCurrentPair->FInput=AInput;
  FCurrentPair->FOutput=AOutput;
  FCurrentPair->FNext=FPairs;
  FPairs = FCurrentPair;
}

void SignalPairSet::Clear()
{
  while(FPairs)
  {
    struct SignalPair * Next=FPairs->FNext;
    delete FPairs;
    FPairs = Next;
  }
}

SignalPairSet::SignalPairSet()
{
  FPairs       = 0;
  FCurrentPair = 0;
}

SignalPairSet::~SignalPairSet()
{
  Clear();
}

}
