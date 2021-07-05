#pragma once

#include "MobileClient.hpp"
#include <regex>

using namespace std;
using namespace Netconfagent;

namespace
{
   string incNumOld;
   const string kState = "idle";
   const string xPath = "/mobile-network:core/subscribers[number='NUMBER']/LEAF";
   //regex
   auto xPathConstr = [](string number, string leaf)
   {
    string result;

    regex regexp1("NUMBER");
    regex regexp2("LEAF");

    result = regex_replace(xPath, regexp1, number);
    result = regex_replace(result, regexp2, leaf);
    return result;
   };
}

namespace mobileclient
{

    MobileClient::MobileClient() 
    : MobileClient::MobileClient{make_unique<Netconfagent::NetConfAgent>()}
  {}

  MobileClient::MobileClient(unique_ptr<Netconfagent::NetConfAgent> netConfAgent)
      : _state{kState},
        _callState {false}, 
        _agent{move(netConfAgent)}
  {}

    string MobileClient::getName()
    {
        return _name;
    };

    bool MobileClient::registerClient(std::string number)
    {
        _number = number;

        string xpathForSubscribe = "/mobile-network:core/subscribers[number='" + _number + "']";
        string xpathForFetch = "/mobile-network:core/subscribers";
        _agent->initSysrepo();

        if(_name == "")
        {
            cout << endl << "Name isn`t setted" << endl;
            return false;
        }
        else
        {
            _agent->changeData(xPathConstr(_number, "state"), _state);
            _agent->changeData(xPathConstr(_number, "number"), number);
            _agent->registerOperData(*this, xpathForSubscribe);
            _agent->subscribeForModelChanges(*this);
        }

        cout << endl << "Client " << _name << " registered with this number " << _number << endl;
    };

    void MobileClient::handleModuleChange(map < string, string >& mapFetchData) 
    {
        _incomingNumber = mapFetchData[xPathConstr(_number, "incomingNumber")];
        _state = mapFetchData[xPathConstr(_number, "state")];
        
        if(_incomingNumber != "" && _state == "active" && mapFetchData[xPathConstr(_incomingNumber, "state")] == "active")
        {        
            cout<<endl;
            cout<<"======Incoming number " << _incomingNumber << " call!!!======"<<endl;
            cout<<endl;
            incNumOld = _incomingNumber;
            _callState = false;
        }

        //I make call
        if(_state == "busy" && mapFetchData[xPathConstr(_outcomingNumber, "state")] == "busy")
        {
            cout<<endl;
            cout<<"==============Call is active=============="<<endl;
            cout<<endl;
            _callState = true;
        }
        //I receive call
        if(_state == "busy" && mapFetchData[xPathConstr(_incomingNumber, "state")] == "busy")
        {
            cout<<endl;
            cout<<"==============Call is active=============="<<endl;
            cout<<endl;
            _callState = true;
        }

        // I made call
        if(_callState == false && _state == "idle" && mapFetchData[xPathConstr(_outcomingNumber, "state")] == "idle" && _outcomingNumber != "")
        {
            cout<<endl;
            cout<<"==============Call rejected=============="<<endl;
            cout<<endl;
            _outcomingNumber = "";
        }
        // I recieved call
        if(_callState == false && _state == "idle" && mapFetchData[xPathConstr(incNumOld, "state")] == "idle" && incNumOld != "")
        {
            cout<<endl;
            cout<<"==============Call rejected=============="<<endl;
            cout<<endl;
            incNumOld = "";
        }

        //I made call
        if(_callState == true && _state == "idle" && mapFetchData[xPathConstr(_outcomingNumber, "state")] == "idle" )
        {
            cout<<endl;
            cout<<"==============Call ended=============="<<endl;
            cout<<endl;
            _outcomingNumber = "";
            _callState = false;
        }
        //I recieved call
        if(_callState == true && _state == "idle" && mapFetchData[xPathConstr(incNumOld, "state")] == "idle"  && incNumOld != "")//last change
        {
            cout<<endl;
            cout<<"==============Call ended=============="<<endl;
            cout<<endl;
            incNumOld = "";
        }
    };

    void MobileClient::handleOperData(std::string& name, std::string& xPath) const
    {
        name = _name;
    
        xPath = "/mobile-network:core/subscribers[number='" + _number + "']/userName";
    }

    bool MobileClient::makeCall(std::string number)
    {

        map < string, string > mapCheckNumber;
        _agent->fetchData(mapCheckNumber);

    if(mapCheckNumber[xPathConstr(number, "number")] != number)
    {
        cout<<"this number doesn't exist!!!"<<endl;
        return false;
    }
    else 
    {
        if(mapCheckNumber[xPathConstr(number, "state")] != "idle")
        {
            cout<<"Client busy!!!"<<endl;
            return false;
        }

        if(mapCheckNumber[xPathConstr(number, "number")] == _number)
        {
            cout<<"This is your number!!!"<<endl;
            return false;
        }

        if(_state != "idle")
        {
            cout<<"You already have call!!!"<<endl;
            return false;
        }

        else if(mapCheckNumber[xPathConstr(number, "number")] == number)
        {
            _agent->changeData(xPathConstr(number, "incomingNumber"), _number);
            _agent->changeData(xPathConstr(_number, "state"), "active");
            _agent->changeData(xPathConstr(number, "state"), "active");
        }
    }
        _outcomingNumber = number;
    }

    void MobileClient::setName(std::string name)
    {
        _name = name;

        cout << endl << "Name " << _name << " setted " << endl;
    }

    bool MobileClient::answerCall()
    {
        if(_incomingNumber != "")
        {
        _agent->changeData(xPathConstr(_number, "state"), "busy");
        _agent->changeData(xPathConstr(_incomingNumber, "state"), "busy");
        }
        else
        {
            cout<<"You don`t have incoming call"<<endl;
            return false;
        }
    }

    bool MobileClient::rejectCall()
    {
        incNumOld = _incomingNumber;

        if(_state == "idle")
        {
            cout<<"Call doesn't exist"<<endl;
            return false;
        }

        if(_state == "busy")
        {
            cout<<"You already answer the call!!!"<<endl;
            return false;
        }

        if(_state == "active" && _incomingNumber != "" && _outcomingNumber == "")
        {
        _agent->changeData(xPathConstr(_incomingNumber, "state"), "idle");
        _agent->changeData(xPathConstr(_number, "state"), "idle");
        _agent->changeData(xPathConstr(_number, "incomingNumber"), "");
        }

        if(_state == "active" && _incomingNumber == "" && _outcomingNumber != "")
        {
        _agent->changeData(xPathConstr(_outcomingNumber, "incomingNumber"), "");
        _agent->changeData(xPathConstr(_number, "state"), "idle");
        _agent->changeData(xPathConstr(_outcomingNumber, "state"), "idle");
        }
    }

    bool MobileClient::endCall()
    {
        incNumOld = _incomingNumber;

        if(_state == "idle")
        {
            cout<<"Call doesn't exist"<<endl;
            return false;
        }

        if(_state == "active")
        {
            cout<<"You didn't answer the call!!!"<<endl;
            return false;
        }

        if(_state == "busy" && _incomingNumber != "" && _outcomingNumber == "")
        {
        _agent->changeData(xPathConstr(_incomingNumber, "state"), "idle");
        _agent->changeData(xPathConstr(_number, "state"), "idle");
        _agent->changeData(xPathConstr(_number, "incomingNumber"), "");
        }

        if(_state == "busy" && _incomingNumber == "" && _outcomingNumber != "")
        {
        _agent->changeData(xPathConstr(_number, "state"), "idle");
        _agent->changeData(xPathConstr(_outcomingNumber, "incomingNumber"), "");
        _agent->changeData(xPathConstr(_outcomingNumber, "state"), "idle");
        }
    }

    bool MobileClient::unRegisterClient()
    {
        string xpathForDelete = "/mobile-network:core/subscribers[number='" + _number + "']";

        if(_state == "idle" && _incomingNumber == "")
        {
            _agent->deleteData(xpathForDelete);
   
            _name.erase();
            _number.erase();
            _incomingNumber.erase();
            _outcomingNumber.erase();
            _state.erase();

            _agent->closeSysrepo();
            _agent.reset();

            cout<<endl;
            cout<<"==========Client unregistered========="<<endl;
            cout<<endl;
        }
        else
        {
            cout<<"Client busy!!!"<<endl;
            return false;
        }
    }
}
