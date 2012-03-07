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

#include "TRT/Straw.hpp"

#include <TStyle.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h> 
#include <unistd.h>

#include "GarfieldConstants.hh"

#include "Particles.h"
// Garfield includes
#include "ViewField.hh"

namespace TRT
{

void StrawAttenuation::Apply(Signals::Signal& ASource)
{
  AdjustSignalEnviroment(ASource);

  double SignalBinTime=GetSignalBinTime(); // Assumed to be in ns!

  // Calculate propagation time in bins for direct and reflected signal
  unsigned int DirectTimeBins    = floor( FStrawPos              *1E-9 / (FSignalSpeed*SignalBinTime) );
  unsigned int ReflectedTimeBins = floor((FStrawPos+FStrawLength)*1E-9 / (FSignalSpeed*SignalBinTime) );

  unsigned int SignalBins=GetSignalBins();

  if(ReflectedTimeBins>=SignalBins) throw Tools::MsgException("StrawAttenuation::Apply : Reflection takes longer than signal");

  double DirectAttenuation    = 0.5*exp(-FStrawPos/FAttenuationLength);
  double ReflectedAttenuation = 0.5*exp(-(FStrawPos+FStrawLength)/FAttenuationLength);

  for(unsigned int i=0;i<DirectTimeBins;++i) SetSignalBin(i,0);

  for(unsigned int i=DirectTimeBins;i<ReflectedTimeBins;++i)
  {
    SetSignalBin(i,
                 DirectAttenuation*ASource.GetSignalBin(i-DirectTimeBins));
  }

  for(unsigned int i=ReflectedTimeBins;i<SignalBins;++i)
  {
    SetSignalBin(i,
                 DirectAttenuation*ASource.GetSignalBin(i-DirectTimeBins)
                +ReflectedAttenuation*ASource.GetSignalBin(i-ReflectedTimeBins));
  }
}

StrawAttenuation::StrawAttenuation(double ASignalBinTime,
                                   double AStrawLength,
                                   double ASignalSpeed,
                                   double AAttenuationLength,
                                   double AStrawPos):Signal(ASignalBinTime)
{
  FStrawLength       = AStrawLength;
  FSignalSpeed       = ASignalSpeed;
  FAttenuationLength = AAttenuationLength;
  FStrawPos          = AStrawPos;
}

const unsigned int IonLookupTableFileSignature=0x0F77A2D0;

void Straw::PlotFieldContour(std::string AFileName)
{
  ViewField * viewfield=new ViewField;
  viewfield->SetSensor(FSensor);
  TCanvas * canvas=new TCanvas;
  canvas->SetRightMargin(-50);
  viewfield->SetCanvas(canvas);
  viewfield->PlotContour("v");
  canvas->Update();
  if(!AFileName.empty())
  {
    canvas->Print(AFileName.c_str());
  }
}

void Straw::SetSignalScalingFactor(double ASignalScalingFactor)
{
  FSignalScalingFactor=ASignalScalingFactor;
}

double Straw::GetStoppingPower(double AMomentum)
{
  // Not sure if this procedure works!
  FTrack->SetEnergy(MomentumToEnergy(AMomentum,GetParticleMass(FParticle)*1e-9));
  return FTrack->GetStoppingPower();
}

double Straw::GetIonSignalAtBin(unsigned int ABin)
{
  if(FIonLookup)
  {
    return FIonSignal.GetSignalBin(ABin)*FSignalScalingFactor;
  }
  else
  {
    return FSensor->GetIonSignal("s",ABin)*FSignalScalingFactor;
  }
}

void Straw::ProcessElectronSignal()
{
//  std::cout<<"PROCESS ELECTRON SIGNAL"<<std::endl;
  for(unsigned int i=0;i<FSignalBins;++i)
  {
//    std::cout<<"Bin("<<i<<"):"<<GetElectronSignalAtBin(i);
    FElectronSignal.AddSignalBin(i,GetElectronSignalAtBin(i));
    FSignal.AddSignalBin(i,GetElectronSignalAtBin(i));
  }
}

double Straw::GetElectronSignalAtBin(unsigned int ABin)
{
  return FSensor->GetElectronSignal("s",ABin)*FSignalScalingFactor;
}

double Straw::GetSignalAtBin(unsigned int ABin)
{
  if(FIonLookup)
  {
    return (FSensor->GetElectronSignal("s",ABin)+FIonSignal.GetSignalBin(ABin))*FSignalScalingFactor;
  }
  else
  {
    return FSensor->GetSignal("s",ABin)*FSignalScalingFactor;
  }
}

void Straw::CheckIonLookupTable(std::string AFileName)
{
  int Descriptor=open(AFileName.c_str(),O_RDONLY,S_IREAD);
  if(Descriptor==-1) throw "Straw::LoadIonLookupTable Unable to open file";
  unsigned int signature;
  read(Descriptor,&signature,sizeof(signature));
  if(signature!=IonLookupTableFileSignature) throw "Straw::LoadIonLookupTable Invalid signature";
  double radius;
  read(Descriptor,&radius,sizeof(radius));
  if(radius!=FStrawTubeRadius) throw "Straw::LoadIonLookupTable Invalid tube radius";
  double anoderadius;
  read(Descriptor,&anoderadius,sizeof(anoderadius));
  if(anoderadius!=FStrawTubeAnodeRadius) throw "Straw::LoadIonLookupTable Invalid Straw Tube Anode radius";
  unsigned int signalbins;
  read(Descriptor,&signalbins,sizeof(signalbins));
  if(signalbins!=FSignalBins) throw "Straw::LoadIonLookupTable Different number of Signal bins";
  double bintime;
  read(Descriptor,&bintime,sizeof(bintime));
  std::cout<<"bintime"<<bintime<<"?"<<FSignalBinTime<<"::"<<bintime-FSignalBinTime<<std::endl;
  if(bintime!=FSignalBinTime) throw "Straw::LoadIonLookupTable Different bin time";
  // MAYBE CHECK GAS AS WELL
  double IonLookupStep;
  read(Descriptor,&IonLookupStep,sizeof(FIonLookupStep));
  if(IonLookupStep!=FIonLookupStep) throw "Straw::CheckIonLookupTable Different Step";
  double * IonLookup = new double[FIonLookupSize*FSignalBins];
  read(Descriptor,IonLookup,sizeof(double)*FIonLookupSize*FSignalBins);
  for(unsigned int i=0;i<FIonLookupSize*FSignalBins;++i)
  {
    if(FIonLookup[i]!=IonLookup[i])
    {
      throw "Straw::CheckIonLookupTable Different Data";
    }
  }
  delete [] IonLookup;
  close(Descriptor);
}

void Straw::SaveIonLookupTable(std::string AFileName)
{
  int Descriptor = open(AFileName.c_str(),O_CREAT|O_TRUNC|O_WRONLY,S_IRWXU);
  if(Descriptor==-1) throw "Straw::SaveIonLookupTable Unable to open file";
  unsigned int signature=IonLookupTableFileSignature;
  write(Descriptor,&signature,sizeof(signature));
  write(Descriptor,&FStrawTubeRadius,sizeof(FStrawTubeRadius));
  write(Descriptor,&FStrawTubeAnodeRadius,sizeof(FStrawTubeAnodeRadius));
  write(Descriptor,&FSignalBins,sizeof(FSignalBins));
  write(Descriptor,&FSignalBinTime,sizeof(FSignalBinTime));
  // MAYBE ADD GAS AS WELL (constant at the moment)
  write(Descriptor,&FIonLookupStep,sizeof(FIonLookupStep));
  write(Descriptor,FIonLookup,sizeof(double)*FIonLookupSize*FSignalBins);
  close(Descriptor);
}

void Straw::LoadIonLookupTable(std::string AFileName)
{
  std::cout<<"Load ION LOOKUP :"<<AFileName<<std::endl;
  if(FIonLookup)
  {
    delete [] FIonLookup;
    FIonLookup=0;
  }
  int Descriptor=open(AFileName.c_str(),O_RDONLY,S_IREAD);
  if(Descriptor==-1) throw "Straw::LoadIonLookupTable Unable to open file";
  unsigned int signature;
  read(Descriptor,&signature,sizeof(signature));
  if(signature!=IonLookupTableFileSignature) throw "Straw::LoadIonLookupTable Invalid signature";
  double radius;
  read(Descriptor,&radius,sizeof(radius));
  if(radius!=FStrawTubeRadius) throw "Straw::LoadIonLookupTable Invalid Straw Tube radius";
  double anoderadius;
  read(Descriptor,&anoderadius,sizeof(anoderadius));
  if(anoderadius!=FStrawTubeAnodeRadius) throw "Straw::LoadIonLookupTable Invalid Straw Tube Anode radius";
  unsigned int signalbins;
  read(Descriptor,&signalbins,sizeof(signalbins));
  if(signalbins!=FSignalBins) throw "Straw::LoadIonLookupTable Different number of Signal bins";
  double bintime;
  read(Descriptor,&bintime,sizeof(bintime));
  if(bintime!=FSignalBinTime) throw "Straw::LoadIonLookupTable Different bin time";
  // MAYBE CHECK GAS AS WELL
  read(Descriptor,&FIonLookupStep,sizeof(FIonLookupStep));
  FIonLookupSize=(FStrawTubeRadius-FStrawTubeAnodeRadius)/FIonLookupStep;
  std::cout<<"FIonLookupSize:"<<FIonLookupSize<<std::endl;
  FIonLookup = new double[FIonLookupSize*FSignalBins];
  read(Descriptor,FIonLookup,sizeof(double)*FIonLookupSize*FSignalBins);
  close(Descriptor);
}

void Straw::CalcIonLookupTable(double AStepSize)
{
  if(FIonLookup) delete [] FIonLookup;
  FIonLookupStep = AStepSize;
  FIonLookupSize = (FStrawTubeRadius-FStrawTubeAnodeRadius)/AStepSize;
  FIonLookup     = new double[FIonLookupSize*FSignalBins];
  std::cout<<"Table size:"<<FIonLookupSize*FSignalBins*sizeof(double)/1024/1024<<"MB"<<std::endl;
  for(unsigned int i=0;i<FIonLookupSize;++i)
  {
    CalcIonPulse(0.0,
                 i*FIonLookupStep+FStrawTubeAnodeRadius,
                 0.0,
                 0.0,
                 1,
                 true); // Do not use this lookup (circle break)

    double sum=0;

    for(unsigned int j=0;j<FSignalBins;++j)
    {
      FIonLookup[i*FSignalBins+j] = FSensor->GetIonSignal("s",j);
      sum+=FIonLookup[i*FSignalBins+j];
    }
    std::cout<<"Sum("<<i*FIonLookupStep+FStrawTubeAnodeRadius<<"):"<<sum<<std::endl;
  }
}

void Straw::EnableMicroscopicCalculation()
{
  FCalculateMicroscopic=true;
}

unsigned int Straw::GetElectronsAtWire()
{
  return FElectronsAtWire;
}

void Straw::EnableIonSignal()
{
  FDrift->EnableSignalCalculation();
}

void Straw::DisableIonSignal()
{
  FDrift->DisableSignalCalculation();
}

double Straw::GetSignalBinTime()
{
  return FSignalBinTime;
}

unsigned int Straw::GetSignalBins()
{
  return FSignalBins;
}

Signals::Signal& Straw::GetIonSignal()
{
  return FIonSignal;
}

Signals::Signal& Straw::GetElectronSignal()
{
  return FElectronSignal;
}

Signals::Signal& Straw::GetSignal()
{
  return FSignal;
}

Straw::Straw(std::string AParticle,
             double ABinTime, // Time for a single bin in ns
             unsigned int ABins)    // Number of bins
{
  FSignalBinTime       = ABinTime;
  FSignalBins          = ABins;
  FSignalTime          = ABins*ABinTime;
  FSignalScalingFactor = 1;
  FParticle            = AParticle;
  
  FAttenuationLength = 4; // m
  FStrawLength       = 0.731; // m
  FSignalSpeed       = 2.25*1E8; // m/s
  
  FCalculateMicroscopic = false;

  FSignal.SetSignalBinTime(ABinTime);
  FElectronSignal.SetSignalBinTime(ABinTime);
  FIonSignal.SetSignalBinTime(ABinTime);
  
  FSignal.SetSignalBins(ABins);
  FElectronSignal.SetSignalBins(ABins);
  FIonSignal.SetSignalBins(ABins);

  FIonLookup       = 0;

  FStrawTubeRadius      = 0.2;     /* cm */
  FStrawTubeAnodeRadius = 15.5e-4; /* cm */
  
  FGas                    = new MediumMagboltz();
  FGas->DisableDebugging();
//  FGas->EnableDeexcitation(); // No effect noticed (1.8.2011)
  FGeometry               = new GeometrySimple();
  FGeometry->DisableDebugging();
  FStrawTube              = new SolidTube(0,                     // Center x
                                          0,                     // Center y
                                          0,                     // Center z
                                          FStrawTubeAnodeRadius, // Minimal Radius
                                          FStrawTubeRadius, // Maximal Radius
                                          2.); // Type Verified, Unverified Unit (Assumed cm)
  FStrawTube->DisableDebugging();
  FComponentAnalyticField = new ComponentAnalyticField;
  FComponentAnalyticField->DisableDebugging();
  FSensor                 = new Sensor;
  FSensor->DisableDebugging();

  FAvalanche              = new AvalancheMC;
  FAvalanche->DisableDebugging();

  FAvalancheMicroscopic   = new AvalancheMicroscopic;
  FAvalancheMicroscopic->DisableDebugging();

  FDrift                  = new AvalancheMC;
  FDrift->DisableDebugging();
  FTrack                  = new TrackHeed;
  FTrack->DisableDebugging();
  FGas->EnableCrossSectionOutput();
  FGas->LoadGasFile("/home/cluster/kn2a1/lu32cud/g/xenongas.gas");
  FGas->SetMaxElectronEnergy(10000.); // Unit =eV
  FGas->LoadIonMobility("/home/cluster/kn2a1/lu32cud/g/IonMobilityXe+.txt");
  
  FGeometry->AddSolid(FStrawTube,FGas);
  FGeometry->DisableDebugging();
  
  FComponentAnalyticField -> SetGeometry(FGeometry);
  FComponentAnalyticField -> AddWire(0.,
                                     0.,
                                     FStrawTubeAnodeRadius,
                                     0,
                                     "s"); // Completely Unverified
                                    
  FComponentAnalyticField -> AddTube(FStrawTubeRadius,
                                     -1530,  // Kathode Voltage in V
                                     0,      // Number of Edges, 0 should be a circle
                                     "t");   // Label //////::Verified Radius, Unverified Unit (Assumed cm)

  FComponentAnalyticField -> AddReadout("s");
  FComponentAnalyticField -> DisableDebugging();
  FComponentAnalyticField -> SetMagneticField(0.,
                                              0.,
                                              2.);

  FSensor -> AddComponent(FComponentAnalyticField);
  FSensor -> AddElectrode(FComponentAnalyticField,"s");
  
  FSensor -> SetTimeWindow(0,              // Start in Nanoseconds
                           FSignalBinTime, // Length in Nanoseconds
                           FSignalBins);   // Number of Bins

  FAvalanche -> SetSensor(FSensor);
  FAvalanche -> EnableMagneticField();
  FAvalanche -> EnableSignalCalculation();
  
  FAvalancheMicroscopic -> SetSensor(FSensor);
  FAvalancheMicroscopic -> EnableMagneticField();
  FAvalancheMicroscopic -> EnableSignalCalculation();
  
  FDrift -> SetSensor(FSensor);
  FDrift -> SetDistanceSteps(20.e-4); // Unit ? propably cm
  FDrift -> EnableSignalCalculation();
  FDrift -> DisableDiffusion();

  FTrack -> SetSensor(FSensor);
  FTrack -> EnableMagneticField();
  FTrack -> SetParticle(AParticle);
  FTrack->DisableDeltaElectronTransport(); // Try to understand their delta electron
                                           // algorithm
}

void Straw::SetIonDriftDistanceStep(double ADistance)
{
  FDrift -> SetDistanceSteps(ADistance);
}

Straw::~Straw()
{
  delete FTrack;
  delete FDrift;
  delete FAvalanche;
  delete FSensor;
  delete FStrawTube;
  delete FGeometry;
  delete FGas;
  if(!FIonLookup) delete [] FIonLookup;
}

void Straw::PlotCell(TCanvas * ACanvas)
{
  ViewCell * viewcell = new ViewCell();
  viewcell -> SetCanvas(ACanvas);
  viewcell -> SetComponent(FComponentAnalyticField);
  viewcell -> SetArea(-0.2,-0.2,-1,0.2,0.2,1);
  viewcell -> Plot2d();
}

void Straw::CalcIonPulse(double Ax,
                             double Ay,
                             double Az,
                             double Atime,
                             unsigned int AIonCount,
                             bool AIgnoreLookup)
{
  ClearSignal();
  for(unsigned int i=0;i<AIonCount;++i)
  {
    if (Ax*Ax+Ay*Ay>FStrawTubeRadius*FStrawTubeRadius)
    {
      std::cout<<"Ion "<<Ax<<":"<<Ay<<":"<<Az<<std::endl;
      throw "Out of range";
    }
    else
    {
      if(FIonLookup && !AIgnoreLookup)
      {
        AddIonLookupSignal(std::sqrt(std::pow(Ax,2)+std::pow(Ay,2)),Atime);
      }
      else
      {
        std::cout<<"!!!!!!!!!!!!!!Manual drift!!!!!!!!!!!!!!!"<<std::endl;
        FDrift->DriftIon(Ax,
                         Ay,
                         Az,
                         Atime);
      }
    }
  }  
}

void Straw::CalcPulse(double Ax,
                      double Ay,
                      double Az,
                      double Atime,
                      double Aee,
                      double Adx,
                      double Ady,
                      double Adz,
                      unsigned int AElectronCount,
                      bool AClearSignal,
                      unsigned int ACountStart) // Does influence electron counting!
{
  if(AClearSignal) ClearSignal();
//  std::cout<<"Aee"<<Aee<<" Adx"<<Adx<<" Ady"<<Ady<<" Adz"<<Adz<<std::endl;
  if(ACountStart==0) FElectronsAtWire = 0;
  for(unsigned int i=0;i<AElectronCount;++i)
  {
    if (Ax*Ax+Ay*Ay>FStrawTubeRadius*FStrawTubeRadius)
    {
      std::cout<<"Electron "<<Ax<<":"<<Ay<<":"<<Az<<std::endl;
      throw "TStrawTube::CalcPulse : Out of range";
    }
    else
    {
//      std::cout<<"Calc electron "<<i+ACountStart<<std::endl;
      int electronendpoints=100000000;
      if(FCalculateMicroscopic)
      {
        FAvalancheMicroscopic->AvalancheElectron(Ax,
                                                 Ay,
                                                 Az,
                                                 Atime,
                                                 Aee,
                                                 Adx,
                                                 Ady,
                                                 Adz);
        electronendpoints=FAvalancheMicroscopic->GetNumberOfElectronEndpoints();
      }
      else
      {
        FAvalanche->AvalancheElectron(Ax,
                                      Ay,
                                      Az,
                                      Atime,
                                      Aee);
        electronendpoints=FAvalanche->GetNumberOfElectronEndpoints();
      }

      for(int electronendpoint=0;electronendpoint<electronendpoints;++electronendpoint)
      {
        double electronendpointx1;
        double electronendpointy1;
        double electronendpointz1;
        double electronendpointt1;
        double electronendpointe1;
        double electronendpointx2;
        double electronendpointy2;
        double electronendpointz2;
        double electronendpointt2;
        double electronendpointe2;
        int electronendpointstatus;
        if(FCalculateMicroscopic)
        {
          FAvalancheMicroscopic->GetElectronEndpoint(electronendpoint,
                                                     electronendpointx1,
                                                     electronendpointy1,
                                                     electronendpointz1,
                                                     electronendpointt1,
                                                     electronendpointe1, 
                                                     electronendpointx2,
                                                     electronendpointy2,
                                                     electronendpointz2,
                                                     electronendpointt2,
                                                     electronendpointe2,
                                                     electronendpointstatus);
        }
        else
        {
          FAvalanche->GetElectronEndpoint(electronendpoint,
                                          electronendpointx1,
                                          electronendpointy1,
                                          electronendpointz1,
                                          electronendpointt1,
                                          electronendpointx2,
                                          electronendpointy2,
                                          electronendpointz2,
                                          electronendpointt2,
                                          electronendpointstatus);
        }
        if((electronendpointstatus==StatusLeftDriftMedium))
//           (fabs(electronendpointx2)<Epsilon) &&
//           (fabs(electronendpointy2)<Epsilon) &&
//           (fabs(electronendpointz2)<Epsilon))
        {
          FElectronsAtWire+=1;
        }
        if(FIonLookup)
        {
          AddIonLookupSignal(std::sqrt(std::pow(electronendpointx1,2)+std::pow(electronendpointy1,2)),electronendpointt1);
        }
        else
        {
          std::cout<<"Manual Drift"<<std::endl;
          FDrift->DriftIon(electronendpointx1,
                           electronendpointy1,
                           electronendpointz1,
                           electronendpointt1);
        }
      }
    }
  }
  if(AClearSignal)
  {
    ProcessElectronSignal();
  }
}

void Straw::ClearSignal()
{
  FSensor->ClearSignal();
  FSignal.Clear();
  FElectronSignal.Clear();
  FIonSignal.Clear();
}

void Straw::AddIonLookupSignal(double ADistance,
                                            double ATime)
{
  unsigned int LookupPos = std::floor(ADistance/FIonLookupStep)*FSignalBins;
  if((LookupPos<=(FIonLookupSize-1)*FSignalBins) && (LookupPos>0))
  {
    unsigned int TimeShift=ATime/FSignalBinTime;
    for(unsigned int i=0;i<FSignalBins-TimeShift-1;++i)
    {
      FIonSignal.AddSignalBin(i+TimeShift,FIonLookup[LookupPos+i]);
      FSignal.AddSignalBin(i+TimeShift,FIonLookup[LookupPos+i]);
    }
  }
}

void Straw::CalcTrack(double AEnergy,
                                   double ADistance,
                                   double & aenergyloss,
                                   unsigned int & atrackelectrons,
                                   unsigned int & AClusters,
                                   bool ACalcSignal)
{
  if(ADistance>=FStrawTubeRadius) throw "Straw::CalcTrack ADistance>=FStrawTubeRadius";
//  std::cout<<"Init Section CalcTrack"<<std::endl;
  double clusterx;
  double clustery;
  double clusterz;
  double clustertime;
  int    clusterelectroncount;
  double clusterenergyloss;
  double clusterextra;
  FElectronsAtWire = 0;
  ClearSignal();
  
  double x=sqrt(FStrawTubeRadius*FStrawTubeRadius-ADistance*ADistance)-0.00001; // - = Very small correction to avoid
                                                                                // hitting area with "no medium"
  
  FTrack -> SetEnergy(AEnergy*1.e9);
  
//  std::cout<<"New Track"<<std::endl;
  if(!FTrack -> NewTrack(ADistance, // Track start x (cm?)
                         -x,	   // Track start y (cm?)
                         0.,        // Track start z (cm?)
                         0.,        // t0
                         0.,        // Track dir x (cm?)
                         2*x,       // Track dir y (cm?)
                         0.))       // Track dir z (cm?)
  {
    throw "Failed Track!";
  }
  int clusternumber = 0;
  aenergyloss       = 0;
  atrackelectrons   = 0;
//  std::cout<<"Cluster Calc"<<std::endl;
  while (FTrack->GetCluster(clusterx,
                            clustery,
                            clusterz,
                            clustertime,
                            clusterelectroncount,
                            clusterenergyloss,
                            clusterextra))
  {
    aenergyloss += clusterenergyloss;
    clusternumber++;
    atrackelectrons += clusterelectroncount;
    if(ACalcSignal==true)
    {    
      std::cout<<"ClusterElectronCount:"<<clusterelectroncount<<std::endl;
      for (int i=0;i<clusterelectroncount;++i)
      {
        double heedelectronx;
        double heedelectrony;
        double heedelectronz;
        double heedelectrontime;
        double heedelectronee;
        double heedelectrondx;
        double heedelectrondy;
        double heedelectrondz;
        if (heedelectronx*heedelectronx+heedelectrony*heedelectrony>FStrawTubeRadius*FStrawTubeRadius)
        {
          std::cout<<"Cluster "<<clusterx<<":"<<clustery<<":"<<clusterz<<std::endl;
          std::cout<<"Electron "<<heedelectronx<<":"<<heedelectrony<<":"<<heedelectronz<<std::endl;
        }
        else
        {
          FTrack->GetElectron(i,
                             heedelectronx,heedelectrony,heedelectronz,
                             heedelectrontime,heedelectronee,
                             heedelectrondx,heedelectrondy,heedelectrondz);

          CalcPulse(heedelectronx,
                    heedelectrony,
                    heedelectronz,
                    heedelectrontime,
                    heedelectronee,
                    heedelectrondx,
                    heedelectrondy,
                    heedelectrondz,
                    1,
                    false,
                    clusternumber-1+i);
        }//endif heedelectron in range
      }
    }
  }
  AClusters=clusternumber;
  ProcessElectronSignal();
}

}
