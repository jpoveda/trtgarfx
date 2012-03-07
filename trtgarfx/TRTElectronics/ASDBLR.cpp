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

#include "TRTElectronics/ASDBLR.hpp"

namespace TRT
{

void ASDBLR::SaveCoefficientsToStream(Tools::ASCIIOutStream AStream)
{
  FASDBLRPreamplifier.SaveCoefficientsToStream(AStream);
  FASDBLRShaper.SaveCoefficientsToStream(AStream);
  FASDBLRBaselineRestorer.SaveCoefficientsToStream(AStream);
  FASDBLRLowDiscIntegrator.SaveCoefficientsToStream(AStream);
  FASDBLRLowDiscriminator.SaveCoefficientsToStream(AStream);
  FASDBLRHighDiscIntegrator.SaveCoefficientsToStream(AStream);
  FASDBLRHighDiscriminator.SaveCoefficientsToStream(AStream);
}

void ASDBLR::SaveCoefficientsToFile(std::string AFileName)
{
  Tools::ASCIIOutStream Stream(AFileName);
  SaveCoefficientsToStream(Stream);
}

ASDBLR::ASDBLR():FASDBLRPreamplifier(0),
                                    FASDBLRShaper(0),
                                    FASDBLRBaselineRestorer(0),
                                    FASDBLRLowDiscIntegrator(0),
                                    FASDBLRHighDiscIntegrator(0),
                                    FASDBLRLowDiscriminator(0),
                                    FASDBLRHighDiscriminator(0)
{
}

}
