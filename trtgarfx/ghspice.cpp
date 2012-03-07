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

#include <iostream>
#include <sys/stat.h>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

#include "Signals/Signal.hpp"
#include "Signals/SignalSet.hpp"

int main(int argc,char * argv[])
{

  boost::program_options::options_description ProgramOptions("Options");

  std::string FileName;
  std::string DirectoryName;

  ProgramOptions.add_options()
   ("help,h","Show help message")
   ("file,f",boost::program_options::value<std::string>(&FileName)->default_value(""),"SignalSet file name")
   ("create","Create Jobs from a SignalSet")
   ("collect","Collect Jobs for a SignalSet")
  ;

  boost::program_options::variables_map ProgramVariables;

  boost::program_options::store(
   boost::program_options::parse_command_line(argc,argv,ProgramOptions),
   ProgramVariables);
  boost::program_options::notify(ProgramVariables);

  if(ProgramVariables.count("help"))
  {
    std::cout<<ProgramOptions;
    return 0;
  }

  if(FileName=="")
  {
    std::cout<<"Please specify a filename. use -h for help."<<std::endl;
    return -1;
  }

  DirectoryName=FileName+".hspice";

  if(!boost::filesystem::exists(FileName))
  {
    std::cout<<"File specified does not exist."<<std::endl;
    return -1;
  }

  if(ProgramVariables.count("create"))
  {
    if(ProgramVariables.count("collect"))
    {
      std::cout<<"Cannot create and collect at the same time"<<std::endl;
      return -1;
    }
    // Create Jobs
    if(boost::filesystem::exists(DirectoryName))
    {
      std::cout<<"Job directory name allready exists."<<std::endl;
      return -1;
    }
    boost::filesystem::create_directory(DirectoryName);
    if(!boost::filesystem::is_directory(DirectoryName))
    {
      std::cout<<"Unable to create job directory:"<<DirectoryName<<std::endl;
      return -1;
    }
    Signals::SignalSet signals("");
    Tools::ASCIIInStream f(FileName);
    f>>signals;
    f.Close();
    Tools::ASCIIOutStream batch(DirectoryName+"/exec.sh");
    batch<<"#!/bin/bash";
    for(unsigned int i=0;i<signals.GetGroupCount();++i)
    {
      Signals::Signal signal=signals.GetSignal(i,"signal");
      std::string SignalName="signal"+IntToStr(i);
      signal.WriteHSpiceSignalInclude(DirectoryName+"/"+SignalName+".inc","isource","n1a","0");
      batch<<("cp "+SignalName+".inc $HSPICEINCLUDE").c_str();
      batch<<("hspice $HSPICEHSP >"+SignalName+".out").c_str();
    }
    batch<<"rm $HSPICEINCLUDE";
    batch.Close();
    chmod((DirectoryName+"/exec.sh").c_str(),0x1ff);
  }
  else
  {
    if(!ProgramVariables.count("collect"))
    {
      std::cout<<"Please specify if you wish to create or collect a job. Use -h for help."<<std::endl;
      return -1;
    }
    // Collect Jobs
    if(!boost::filesystem::is_directory(DirectoryName))
    {
      std::cout<<"Job directory missing.";
      return -1;
    }
  }
  return 0;
}
