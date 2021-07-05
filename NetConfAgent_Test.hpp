#pragma once

#include "gmock/gmock.h"
#include "header/NetConfAgent.hpp"

using namespace Netconfagent;
using namespace mobileclient;

class MockNetConfAgent : public NetConfAgent
{
    public:
    
    MOCK_METHOD(bool, initSysrepo, (), (override));

    MOCK_METHOD(bool, fetchData, ((std::map <std::string, std::string>& map)), (override));

    MOCK_METHOD(bool, registerOperData, (mobileclient::MobileClient& client, (std::string xpathForSubscribe)), (override));

    MOCK_METHOD(bool, subscribeForModelChanges, (mobileclient::MobileClient& client), (override));

    MOCK_METHOD(bool, subscribeForRpc, (std::string _module_name, (std::string _rpc_xpath)), (override));

    MOCK_METHOD(bool, changeData, (std::string _xpath, (std::string newValue)), (override));

    MOCK_METHOD(bool, deleteData, (std::string _xpath), (override));

    MOCK_METHOD(bool, closeSysrepo, (), (override));
};


