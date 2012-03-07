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

#ifndef _tools_
#define _tools_

#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "math.h"
#include <exception>

double CalcPoly(std::vector<double> ACoefficients,double AValue);

// Resets Root into a sane state (plots etc)
void InitRoot();

// Calculates the separation power for two given histograms using
// |mean1-mean2|/sqrt(sigma1²+sigma2²)

double BetaGammaToEnergy(double AMass, // x/c^2 Mass unit dictates result unit
                         double ABetaGamma);

double MomentumToEnergy(double AMass,    // x/c^2 Units must be equal and dictate result unit
                        double AEnergy); // x/c

std::string IntToStr(int value);
std::string DoubleToStr(double value);

// Combines an absolute or relative path with a relativ path. Takes care of slashes.
std::string CombinePath(std::string ALeft, std::string ARight);
// Extends a relative path to an absolute path. Absolute paths are returned as they are.
std::string CompletePath(std::string APath);

void Trim(std::string & AString);
void FilterCharacter(std::string & AString,char ACharacter);
std::vector<std::string> SplitString(std::string AString);
std::vector<std::string> TriggeredSplitString(std::string AString, std::string ATrigger);
bool HasNonEmpty(std::vector<std::string> AVector);

#endif
