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

#include "tools.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

#ifdef USEROOT
#include <TColor.h>
#include <TStyle.h>
#include <TROOT.h>
#endif

double CalcPoly(std::vector<double> ACoefficients,double AValue)
{
  double result=0;
  for(unsigned int i=0;i<ACoefficients.size();++i)
  {
    result+=ACoefficients[i]*pow(AValue,i);
  }
  return result;
}

bool HasNonEmpty(std::vector<std::string> AVector)
{
  for(unsigned int i=0;i<AVector.size();++i)
  {
    if(AVector[i]!="") return true;
  }
  return false;
}

// Ugly hack! Necessary for .lis processing only!
std::vector<std::string> TriggeredSplitString(std::string AString,std::string ATrigger)
{
  std::vector<std::string> result;
  std::string current="";
  bool PostTrigger=false;
  for(unsigned int pos=0;pos<AString.size() && pos<ATrigger.size();++pos)
  {
    if(PostTrigger)
    {
      if(AString[pos]!=' ')
      {
        current+=AString[pos];
      }
      else
      {
        Trim(current);
        result.push_back(current);
        current = "";
        PostTrigger=false;
      }      
    }
    if(!PostTrigger)
    {
      switch(ATrigger[pos])
      {
        case ' ':case '\r':
          if(current!="")
          {
            if(current[current.size()-1]!=' ')
            {
              PostTrigger=true;
              current+=AString[pos];
            }
            else
            {
              Trim(current);
              result.push_back(current);
              current = "";
            }
          }
          break;
        default:
          current+=AString[pos];
          break;
      }
    }
  }
  if(current!="")
  {
    Trim(current);
    result.push_back(current);
  }
  return result;
}

std::vector<std::string> SplitString(std::string AString)
{
  std::vector<std::string> result;
  std::string current="";
  for(unsigned int pos=0;pos<AString.size();++pos)
  {
    switch(AString[pos])
    {
      case ' ':case '\r':
        if(current!="")
        {
          result.push_back(current);
          current="";
        }
        
        break;
      default:
        current+=AString[pos];
        break;
    }
  }
  if(current!="") result.push_back(current);
  return result;
}

void Trim(std::string & AString)
{
  size_t p1=AString.find_first_not_of(' ');
  if(p1!=std::string::npos)
  {
    size_t p2=AString.find_last_not_of(' ');
    AString=AString.substr(p1,p2-p1+1);
  }
  else
  {
    if((AString.size()!=0) && (AString[0]==' ')) AString="";
  }
}

void FilterCharacter(std::string & AString,char ACharacter)
{
  unsigned int i=0;
  while(i<AString.size())
  {
    if(AString[i]==ACharacter) AString.erase(i);
    i++;
  }
}

#ifdef USEROOT
void SetGradientPalette()
{
  Double_t Red[3]  = { 1, 0, 0};
  Double_t Green[3]= { 0, 1, 0};
  Double_t Blue[3] = { 1, 0, 1};
  Double_t Len[3]  = { 0, 0.5, 1};
  Int_t nb=50;
  Int_t C=3;
  TColor::CreateGradientColorTable(C,Len,Red,Green,Blue,nb); 
}
#endif

#ifdef USEROOT
void InitRoot()
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptTitle(kFALSE);
}
#endif

double MomentumToEnergy(double AMass,    // x/c^2
                    double AImpuls)  // x/c
{
  return sqrt(AImpuls*AImpuls+AMass*AMass);
}

double BetaGammaToEnergy(double AMass,       // x/c^2
                       double ABetaGamma)  // unitless
{
  return AMass*sqrt(1+ABetaGamma*ABetaGamma);
}

std::string CompletePath(std::string APath)
{
  if((APath.empty()) || (APath[0]!='/'))
  {
     char * buf    = getcwd(0,4096); // Damn old c...
     std::string b = buf;
     free(buf);
     return CombinePath(b,APath);
  }
  else
  {
    return APath;
  }
}

std::string CombinePath(std::string ALeft, std::string ARight)
{
  if(ALeft.empty())return ARight;
  if(ARight.empty())return ALeft;
  if(ALeft[ALeft.length()-1]=='/')
  {
    if(ARight[0]=='/') ALeft.erase(ALeft.length()-1,1);
    return ALeft+ARight;
  }
  else
  {
    if(ARight[0]=='/')
    {
      return ALeft+ARight;
    }
    else
    {
      return ALeft+'/'+ARight;
    }
  }
}

std::string IntToStr(int value)
{
  std::ostringstream o;
  if (!(o<<value)) return "error";
  return o.str();
}

std::string DoubleToStr(double value)
{
  std::ostringstream o;
  if (!(o<<value)) return "error";
  return o.str();
}
