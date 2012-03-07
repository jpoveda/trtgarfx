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

#ifndef _TRTElectronics_ASDBLR_
#define _TRTElectronics_ASDBLR_
#include "TRTElectronics/ASDBLRPreamplifier.hpp"
#include "TRTElectronics/ASDBLRShaper.hpp"
#include "TRTElectronics/ASDBLRBaselineRestorer.hpp"
#include "TRTElectronics/ASDBLRDiscriminator.hpp"
#include "Signals/HighPass.hpp"

namespace TRT
{

class ASDBLR
{
  public:
  ASDBLRPreamplifier     FASDBLRPreamplifier;
  ASDBLRShaper           FASDBLRShaper;
  ASDBLRBaselineRestorer FASDBLRBaselineRestorer;
  Signals::HighPass      FASDBLRLowDiscIntegrator;
  Signals::HighPass      FASDBLRHighDiscIntegrator;
  ASDBLRDiscriminator    FASDBLRLowDiscriminator;
  ASDBLRDiscriminator    FASDBLRHighDiscriminator;
  ASDBLR();
  void SaveCoefficientsToStream(Tools::ASCIIOutStream AStream);
  void SaveCoefficientsToFile(std::string AFileName);
};

}

#endif
