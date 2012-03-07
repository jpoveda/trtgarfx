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

#ifndef _Tools_Stream_
#define _Tools_Stream_
#include <string>
#include <fstream>
#include <vector>

namespace Tools{

class ASCIIInStream
{
  private:
  std::ifstream FFile;
  public:
  void Close();
  ASCIIInStream(std::string AFileName);
  ASCIIInStream(ASCIIInStream& AStream);
  ~ASCIIInStream();
  ASCIIInStream& operator>>(const char * AString);
  ASCIIInStream& operator>>(double& ANumber);
  ASCIIInStream& operator>>(unsigned int& ANumber);
  ASCIIInStream& operator>>(std::string& AString);
  ASCIIInStream& operator>>(std::vector<double>& AVector);
  ASCIIInStream& operator=(ASCIIInStream& AStream);
};

class ASCIIOutStream
{
  private:
  std::ofstream FFile;
  public:
  void Close();
  ASCIIOutStream(std::string AFileName);
  ASCIIOutStream(ASCIIOutStream& AStream);
  ~ASCIIOutStream();
  ASCIIOutStream& operator<<(const char * AString);
  ASCIIOutStream& operator<<(double ANumber);
  ASCIIOutStream& operator<<(std::vector<double>& AVector);
  ASCIIOutStream& operator<<(std::string& AString);
  ASCIIOutStream& operator<<(unsigned int ANumber);
  ASCIIOutStream& operator=(ASCIIOutStream& AStream);
};

}

#endif
