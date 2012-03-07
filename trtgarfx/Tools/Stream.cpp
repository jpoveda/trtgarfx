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
#include "Tools/Exception.hpp"
#include "Tools/Tools.hpp"
#include "tools.hpp"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

namespace Tools
{

ASCIIOutStream::ASCIIOutStream(ASCIIOutStream&)
{
  throw Tools::MsgException("ASCIIOutStream::ASCIIOutStream : Copy not allowed");
}

ASCIIInStream::ASCIIInStream(ASCIIInStream&)
{
  throw Tools::MsgException("ASCIIInStream::ASCIIInStream : Copy not allowed");
}

ASCIIOutStream& ASCIIOutStream::operator=(ASCIIOutStream& AStream)
{
  throw Tools::MsgException("ASCIIOutStream::operator = : Copy not allowed");
  return AStream;
}

ASCIIInStream& ASCIIInStream::operator=(ASCIIInStream& AStream)
{
  throw Tools::MsgException("ASCIIInStream::operator = : Copy not allowed");
  return AStream;
}

ASCIIInStream& ASCIIInStream::operator>>(std::vector<double>& AVector)
{
  std::string s;
  if(!FFile.good()) throw Tools::MsgException("ASCIIInStream::operator >> std::vector<double> : Unexpected end of file");
  getline(FFile,s);
  unsigned int Count            = atoi(s.c_str());
  std::vector<std::string> data = SplitString(s);
  while(Count!=0)
  {
    if(!FFile.good()) throw Tools::MsgException("ASCIIInStream::operator >> std::vector<double> : End of file before end of vector");
    getline(FFile,s);
    AVector.push_back(atof(s.c_str()));
    Count--;
  }
  return *this;
}

ASCIIInStream& ASCIIInStream::operator>>(unsigned int& ANumber)
{
  std::string s;
  if(!FFile.good()) throw Tools::MsgException("ASCIIInStream::operator >> unsigned int : Unexpected end of file");
  getline(FFile,s);
  ANumber = atoi(s.c_str());
  return *this;
}

ASCIIInStream& ASCIIInStream::operator>>(double& ANumber)
{
  std::string s;
  if(!FFile.good()) throw Tools::MsgException("ASCIIInStream::operator >> double : Unexpected end of file");
  getline(FFile,s);
  ANumber = atof(s.c_str());
  return *this;
}

ASCIIInStream& ASCIIInStream::operator>>(const char * AString)
{
  std::string s;
  if(!FFile.good()) throw Tools::MsgException("ASCIIInStream::operator >> const char * : Unexpected end of file");
  getline(FFile,s);
  s = UnescapeString(s);
  if(s!=AString) throw Tools::MsgException("ASCIIInStream::operator >> const char * : Check failed");
  return *this;
}

ASCIIInStream& ASCIIInStream::operator>>(std::string& AString)
{
  if(!FFile.good()) throw Tools::MsgException("ASCIIInStream::operator >> std::string : Unexpected end of file");
  getline(FFile,AString);
  AString = UnescapeString(AString);
  return *this;
}

ASCIIInStream::ASCIIInStream(std::string AFileName)
{
  FFile.open(AFileName.c_str());
  if(!FFile.good()) throw Tools::MsgException("ASCIIInStream::ASCIIInStream : Cannot open file for reading :"+AFileName);
}

void ASCIIInStream::Close()
{
  FFile.close();
}

ASCIIInStream::~ASCIIInStream()
{
  FFile.close();
}

ASCIIOutStream& ASCIIOutStream::operator<<(std::vector<double>& AVector)
{
  FFile<<AVector.size()<<std::endl;
  for(unsigned int i=0;i<AVector.size();++i)
  {
    FFile<<AVector[i]<<std::endl;
  }
  return *this;
}

ASCIIOutStream& ASCIIOutStream::operator<<(std::string& AString)
{
  // Missing check for invalid std::endl in AString
  FFile<<EscapeString(AString)<<std::endl;
  return *this;
}

ASCIIOutStream& ASCIIOutStream::operator<<(unsigned int ANumber)
{
  FFile<<ANumber<<std::endl;
  return *this;
}

ASCIIOutStream& ASCIIOutStream::operator<<(const char * AString)
{
  FFile<<EscapeString(AString)<<std::endl;
  return *this;
}

ASCIIOutStream& ASCIIOutStream::operator<<(double ANumber)
{
  FFile<<ANumber<<std::endl;
  return *this;
}

ASCIIOutStream::ASCIIOutStream(std::string AFileName)
{
  FFile.open(AFileName.c_str());
  if(!FFile.good()) throw Tools::MsgException("ASCIIOutStream::ASCIIOutStream : Cannot open file for writing :"+AFileName);
}

void ASCIIOutStream::Close()
{
  FFile.close();
}

ASCIIOutStream::~ASCIIOutStream()
{
  FFile.close();
}

}
