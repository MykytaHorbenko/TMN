#pragma once

#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "libsysrepocpp/headers/Session.hpp"

using namespace sysrepo;
namespace mobileclient
{
    class MobileClient;
}

namespace Netconfagent
{

class NetConfAgent
{
    public:
/*
*@brief initSysrepo. 
*This method initializes a session with the server
*
*@return true if ok, otherwise false
*/
    bool initSysrepo();
/*
*@brief brief fetchData. 
*This method receives data from the model and writes to the container
*
*@param xpath the path on which the data will be received
*@param map the container to which the data will be written
*
*@return true if ok, otherwise false
*/
    bool fetchData(std::string xpath, std::map <std::string, std::string>& map);
/*
*@brief subscribeForModelChanges. 
*This method reacts to changes in the model and prints them
*
*@param module_name the name of module to which the subscription is created
*
*@return true if ok, otherwise false
*/
    bool subscribeForModelChanges(mobileclient::MobileClient& client, std::string& xpathForFetch);
/*
*@brief registerOperData. 
*This method receives operdata from the model and set them
*
*@param module_name the name of module to which the subscription is created
*
*@return true if ok, otherwise false
*/
    bool registerOperData(mobileclient::MobileClient& client, std::string xpathForSubscribe);
/*
*@brief subscribeForRpc. 
*This method makes RPC call and receives return value from the model
*
*@param module_name the name of module to which the subscription is created
*@param rpc_xpath the path on which the return value will be received
*
*@return true if ok, otherwise false
*/
    bool subscribeForRpc(std::string _module_name, std::string _rpc_xpath);
/*
*@brief notifySysrepo. 
*This method sends notification in model
*
*@param module_name the name of module to which the notification is sent
*
*@return true if ok, otherwise false
*/
    bool notifySysrepo(std::string _module_name);
 /*
*@brief changeData. 
*This method change value of leaf in model
*
*@param xpath the path on which the data will be changed
*@param newValue new value of leaf
*
*@return true if ok, otherwise false
*/   
    bool changeData(std::string _xpath, std::string newValue);
    private:
    S_Connection Connection;
    S_Session Session;
    S_Subscribe Subscribe;
    
};

}
