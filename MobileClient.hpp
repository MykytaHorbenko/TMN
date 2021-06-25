#pragma once

#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "NetConfAgent.hpp"
#include "libsysrepocpp/headers/Session.hpp"

using namespace sysrepo;
using namespace Netconfagent;

namespace mobileclient
{

class MobileClient
{
    public:
/*
*@brief initSysrepo. 
*This method initializes a session with the server
*
*@return true if ok, otherwise false
*/
    MobileClient();
    
    bool registerClient(std::string number);

    bool unRegisterClient();

    void handleModuleChange(std::map < std::string, std::string >& mapFetchData) ;

    void handleOperData(std::string& name, std::string& xPath) const;

    void handleRpc();

    void handleNotification();

    bool makeCall(std::string number);

    bool answerCall();

    bool rejectCall();

    bool endCall();

    void setName(std::string name);

    private:
    std::unique_ptr<Netconfagent::NetConfAgent> _agent;
    std::string _name;
    std::string _number;
    std::string _incomingNumber;
    std::string _outcomingNumber;
    std::string _state;
    bool _callState;
    


};

}
