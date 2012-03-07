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

#ifndef _Tools_SSH_
#define _Tools_SSH_
#include "libssh/libssh.h"
#include <string>
#include <list>

namespace SSH
{
  class SSH
  {
    private:
    ssh_session            FSession;
    public:
    std::list<std::string> FOutput;
    void ReceiveFile(std::string ASource,std::string ADest);
    void SendFile(std::string ASource,std::string ADest);
    void ExecuteCommand(std::string ACommand);
    SSH(std::string AHost,std::string AUserName,std::string APassword);
    ~SSH();
  };
}
#endif
