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

#include "SSH/SSH.hpp"
#include "Tools/Exception.hpp"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

namespace SSH
{
  void SSH::ReceiveFile(std::string ASource,std::string ADest)
  {
    ssh_scp scp;
    scp=ssh_scp_new(FSession,SSH_SCP_READ,ASource.c_str());
    if(scp==NULL) throw Tools::MsgException("SSH::ReceiveFile : Cannot create scp");
    int err=ssh_scp_init(scp);
    if(err!=SSH_OK) throw Tools::MsgException("SSH::ReceiveFile : Cannot initialize scp");
    err=ssh_scp_pull_request(scp);
    if(err!=SSH_SCP_REQUEST_NEWFILE) Tools::MsgException("SSH::ReceiveFile : Expected new file");
    int filesize=ssh_scp_request_get_size(scp);
    void * buffer=malloc(65536);
    int fileh=open(ADest.c_str(),O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU);
    ssh_scp_accept_request(scp);
    int blocksize;
    std::cout<<"fileh:"<<fileh<<"::"<<ADest<<std::endl;

    while(filesize!=0)
    {
      if(filesize>65536)
      {
        blocksize=65536;
      }
      else
      {
        blocksize=filesize;
      }
      err=ssh_scp_read(scp,buffer,blocksize);
      if(err==SSH_ERROR) Tools::MsgException("SSH::ReceiveFile : SSH_ERROR");
      int writtenbytes=write(fileh,buffer,blocksize);
      std::cout<<"**"<<blocksize<<"**"<<writtenbytes<<std::endl;
      if(writtenbytes!=blocksize) throw Tools::MsgException("SSH::ReceiveFile : Writtenbytes!=Filesize");
      filesize-=blocksize;
    }

    if(close(fileh)!=0) throw Tools::MsgException("SSH::ReceiveFile : Failed close file");
    free(buffer);
    ssh_scp_free(scp);
  }

  // This will only copy files up to 64k for the moment (sad limit!)
  void SSH::SendFile(std::string ASource,std::string ADest)
  {
    ssh_scp scp;
    scp=ssh_scp_new(FSession,SSH_SCP_WRITE,".");
    if(scp==NULL) throw Tools::MsgException("SSH::SendFile : Cannot create scp");
    int err=ssh_scp_init(scp);
    if(err!=SSH_OK) throw Tools::MsgException("SSH::SendFile : Cannot initialize scp");
    int fileh=open(ASource.c_str(),O_RDONLY);
    if(fileh<-1) throw Tools::MsgException("SSH::SendFile : Cannot open file for scp");
    struct stat filestat;
    if(fstat(fileh,&filestat)<0) throw Tools::MsgException("SSH::SendFile : Failed stat");
    int filesize=filestat.st_size;
    std::cout<<"File Size:"<<filesize<<std::endl;
    err=ssh_scp_push_file(scp,"ASDBLR/hspice.inc",filesize,S_IRWXU);
    if(err!=SSH_OK) throw Tools::MsgException("SSH::SendFile : Cannot 'push' file");
    void * buffer=malloc(filesize);
    int readbytes=read(fileh,buffer,filesize);
    if(readbytes!=filesize) throw Tools::MsgException("SSH::SendFile : Readbytes!=FileSize");
    if(close(fileh)!=0) throw Tools::MsgException("SSH::SendFile : Failed close");
    ssh_scp_write(scp,buffer,filesize);
    free(buffer);
    ssh_scp_free(scp);
  }

  void SSH::ExecuteCommand(std::string ACommand)
  {
    ssh_channel channel=ssh_channel_new(FSession);
    if(channel==NULL) throw Tools::MsgException("Cannot create channel");
    int err=ssh_channel_open_session(channel);
    if(err!=SSH_OK)
    {
      ssh_channel_free(channel);
      throw Tools::MsgException("Channel not OK (open session failed)");
    }
    ssh_channel_request_exec(channel,ACommand.c_str());
    std::string s="";
    time_t start_time=time(NULL);

    while(ssh_channel_is_open(channel) &&
          !ssh_channel_is_eof(channel))
    {
//      std::cout<<ssh_channel_is_open(channel)<<"."<<ssh_channel_is_eof(channel)<<std::endl;
      char buffer[1024];
      int nbytes=ssh_channel_read_nonblocking(channel,buffer,sizeof(buffer),1);
      FOutput.clear();
      while(nbytes>0)
      {
        std::cout<<"Bytes:"<<nbytes<<std::endl;
        for(int i=0;i<nbytes;++i)
        {
          switch(buffer[i])
          {
            case '\n':
              FOutput.push_back(s);
              s="";
              break;
            default:
              s += buffer[i];
              break;
          }
        }
        nbytes=ssh_channel_read(channel,buffer,sizeof(buffer),0);
      }
      while(difftime(time(NULL),start_time)<1.0){};
      
    }
    std::cout<<"********************"<<std::endl;
    std::cout<<ssh_channel_is_open(channel)<<"."<<ssh_channel_is_eof(channel)<<std::endl;
    std::cout<<"********************"<<std::endl;
    if(s!="") FOutput.push_back(s);
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
  }

  SSH::SSH(std::string AHost,std::string AUserName,std::string APassword)
  {
    FSession=ssh_new();
    if(FSession==NULL) throw new Tools::MsgException("Failed to create a session");
    ssh_options_set(FSession,SSH_OPTIONS_HOST,AHost.c_str());
    ssh_options_set(FSession,SSH_OPTIONS_USER,AUserName.c_str());
    int err=ssh_connect(FSession);
    if(err!=SSH_OK) throw new Tools::MsgException("Failed to connect to server");
    int knownstate=ssh_is_server_known(FSession);
    if(knownstate!=SSH_SERVER_KNOWN_OK) throw new Tools::MsgException("Public key not available");
    err=ssh_userauth_password(FSession,NULL,APassword.c_str());
    if(err!=SSH_AUTH_SUCCESS) throw new Tools::MsgException("Failed to authenticate");
  }
  SSH::~SSH()
  {
    ssh_disconnect(FSession);
    ssh_free(FSession);
  }
}
