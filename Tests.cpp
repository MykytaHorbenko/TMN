#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "NetConfAgent_Test.hpp"
#include "MobileClient.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::DoAll;
using ::testing::SetArgReferee;


class MobileClientTest : public testing::Test
{
protected:
void SetUp() override
{
 auto tempMock = std::make_unique<testing::StrictMock<MockNetConfAgent>>();
 _mock = tempMock.get();
_mobileClient = std::make_unique<mobileclient::MobileClient>(std::move(tempMock));
}
 
void registerTestClient(std::string name, std::string number)
{
    EXPECT_CALL(*_mock, initSysrepo())
            .WillOnce(Return(true));
    EXPECT_CALL(*_mock, changeData(_,_))
            .Times(2)
            .WillRepeatedly(Return(true));
    EXPECT_CALL(*_mock, registerOperData(_,_))
            .WillOnce(Return(true));
    EXPECT_CALL(*_mock, subscribeForModelChanges(_))
            .WillOnce(Return(true));

    _mobileClient->setName(name);
    _mobileClient->registerClient(number);
}

//This func is using only for setting private fields ("state", "incNumber") by number
void callHandleModuleChange(std::string number, std::string state, std::string incomingNumber)
{
    std::string xpathState = "/mobile-network:core/subscribers[number='"+number+"']/state";
    std::string xpathIncNumber = "/mobile-network:core/subscribers[number='"+number+"']/incomingNumber";

    std::map <std::string, std::string> testMap = {{xpathState, state}, {xpathIncNumber, incomingNumber}};

    _mobileClient->handleModuleChange(testMap);
}

//This func is using only for setting private field ("outcomNumber") 
void makeCallTest(std::string number)
{
    
    std::string state = "idle";

    std::string xpathNumber = "/mobile-network:core/subscribers[number='"+number+"']/number";
    std::string xpathState = "/mobile-network:core/subscribers[number='"+number+"']/state";

    std::map <std::string, std::string> testMap = {{xpathNumber, number}, {xpathState, state}};

    EXPECT_CALL(*_mock, fetchData(_))
                .WillOnce(DoAll(SetArgReferee<0>(testMap),
                            Return(true)));

    EXPECT_CALL(*_mock, changeData(_,_))
               .Times(3);

    EXPECT_TRUE(_mobileClient->makeCall(number));
}

protected:
std::unique_ptr<mobileclient::MobileClient> _mobileClient;
testing::StrictMock<MockNetConfAgent>* _mock;
};

TEST_F(MobileClientTest, SetNameTrue)
{
std::string name = "Max";
_mobileClient->setName(name);
EXPECT_EQ(_mobileClient->getName(),"Max");
}

TEST_F(MobileClientTest, RegisterClientTrue)
{
    std::string name = "Max";
    std::string number = "111";
    
_mobileClient->setName(name);

EXPECT_CALL(*_mock, initSysrepo())
            .WillOnce(Return(true));
EXPECT_CALL(*_mock, changeData(_,_))
            .Times(2)
            .WillRepeatedly(Return(true));
EXPECT_CALL(*_mock, registerOperData(_,_))
            .WillOnce(Return(true));
EXPECT_CALL(*_mock, subscribeForModelChanges(_))
            .WillOnce(Return(true));

EXPECT_TRUE(_mobileClient->registerClient(number));
}

TEST_F(MobileClientTest, RegisterClientFalse)
{
//     std::string name = "Max";
// _mobileClient->setName(name);
     std::string number = "111";

EXPECT_CALL(*_mock, initSysrepo())
            .WillOnce(Return(true));

EXPECT_FALSE(_mobileClient->registerClient(number));
}

TEST_F(MobileClientTest, makeCallTrue)
{
    std::string number = "222";
    std::string state = "idle";

    std::string xpathNumber = "/mobile-network:core/subscribers[number='222']/number";
    std::string xpathState = "/mobile-network:core/subscribers[number='222']/state";

    std::map <std::string, std::string> testMap = {{xpathNumber, number}, {xpathState, state}};

    EXPECT_CALL(*_mock, fetchData(_))
                .WillOnce(DoAll(SetArgReferee<0>(testMap),
                            Return(true)));

    EXPECT_CALL(*_mock, changeData(_,_))
               .Times(3);

    EXPECT_TRUE(_mobileClient->makeCall("222"));
}

TEST_F(MobileClientTest, makeCallFalse_MyNumber)
{
    std::cout<<"Calling on my number"<<std::endl;

    registerTestClient("Max","222");

    std::string number = "222";
    std::string state = "idle";

    std::string xpathNumber = "/mobile-network:core/subscribers[number='222']/number";
    std::string xpathState = "/mobile-network:core/subscribers[number='222']/state";

    std::map <std::string, std::string> testMap = {{xpathNumber, number}, {xpathState, state}};

    EXPECT_CALL(*_mock, fetchData(_))
                .WillOnce(DoAll(SetArgReferee<0>(testMap),
                            Return(true)));

    EXPECT_FALSE(_mobileClient->makeCall("222"));
}

TEST_F(MobileClientTest, makeCallFalse_State)
{
    std::cout<<"State other client isn't 'idle'"<<std::endl;

    std::string number = "222";
    std::string state = "busy";

    std::string xpathNumber = "/mobile-network:core/subscribers[number='222']/number";
    std::string xpathState = "/mobile-network:core/subscribers[number='222']/state";

    std::map <std::string, std::string> testMap = {{xpathNumber, number}, {xpathState, state}};

    EXPECT_CALL(*_mock, fetchData(_))
                .WillOnce(DoAll(SetArgReferee<0>(testMap),
                            Return(true)));

    EXPECT_FALSE(_mobileClient->makeCall("222"));
}

TEST_F(MobileClientTest, makeCallFalse_OutNumber)
{
    std::cout<<"Outcoming number doesn't exist"<<std::endl;

    std::string number = "";
    std::string state = "busy";

    std::string xpathNumber = "/mobile-network:core/subscribers[number='222']/number";
    std::string xpathState = "/mobile-network:core/subscribers[number='222']/state";

    std::map <std::string, std::string> testMap = {{xpathNumber, number}, {xpathState, state}};

    EXPECT_CALL(*_mock, fetchData(_))
                .WillOnce(DoAll(SetArgReferee<0>(testMap),
                            Return(true)));

    EXPECT_FALSE(_mobileClient->makeCall("222"));
}

TEST_F(MobileClientTest, answerCallTrue)
{
registerTestClient("Max","111");
callHandleModuleChange("111", "busy", "222");

EXPECT_CALL(*_mock, changeData(_,_))
            .Times(2)
            .WillRepeatedly(Return(true));

EXPECT_TRUE(_mobileClient->answerCall());
}

TEST_F(MobileClientTest, answerCallFalse)
{
registerTestClient("Max","111");
callHandleModuleChange("111", "busy", "");

EXPECT_FALSE(_mobileClient->answerCall());
}

//I recieved call
TEST_F(MobileClientTest, rejectCallTrue_ByMe)
{
registerTestClient("Max","111");
callHandleModuleChange("111", "active", "222");

EXPECT_CALL(*_mock, changeData(_,_))
            .Times(3)
            .WillRepeatedly(Return(true));

EXPECT_TRUE(_mobileClient->rejectCall());
}

//I made call
TEST_F(MobileClientTest, rejectCallTrue_ByOther)
{
registerTestClient("Max","111");
callHandleModuleChange("111", "idle", "");
registerTestClient("Anna","222");

makeCallTest("111");
callHandleModuleChange("222", "active", "");


EXPECT_CALL(*_mock, changeData(_,_))
            .Times(3)
            .WillRepeatedly(Return(true));

EXPECT_TRUE(_mobileClient->rejectCall());
}

TEST_F(MobileClientTest, rejectCallFalse_WithoutCall)
{
std::cout<<"Call doesn't exist"<<std::endl;

registerTestClient("Max","111");
callHandleModuleChange("111", "idle", "");

EXPECT_FALSE(_mobileClient->rejectCall());
}

TEST_F(MobileClientTest, rejectCallFalse_CallActive)
{
std::cout<<"Call already exist"<<std::endl;

registerTestClient("Max","111");
callHandleModuleChange("111", "busy", "");

EXPECT_FALSE(_mobileClient->rejectCall());
}

//I recieved call
TEST_F(MobileClientTest, endCallTrue_ByMe)
{
registerTestClient("Max","111");
callHandleModuleChange("111", "busy", "222");

EXPECT_CALL(*_mock, changeData(_,_))
            .Times(3)
            .WillRepeatedly(Return(true));

EXPECT_TRUE(_mobileClient->endCall());
}

//I made call
TEST_F(MobileClientTest, endCallTrue_ByOther)
{
registerTestClient("Max","111");
callHandleModuleChange("111", "idle", "");
registerTestClient("Anna","222");
makeCallTest("111");
callHandleModuleChange("222", "busy", "");


EXPECT_CALL(*_mock, changeData(_,_))
            .Times(3)
            .WillRepeatedly(Return(true));

EXPECT_TRUE(_mobileClient->endCall());
}

TEST_F(MobileClientTest, endCallFalse_WithoutCall)
{
registerTestClient("Max","111");
callHandleModuleChange("111", "idle", "");

EXPECT_FALSE(_mobileClient->endCall());
}

TEST_F(MobileClientTest, endCallFalse_CallNotActive)
{
registerTestClient("Max","111");
callHandleModuleChange("111", "active", "");

EXPECT_FALSE(_mobileClient->endCall());
}

TEST_F(MobileClientTest, unregisterClientTrue)
{
registerTestClient("Max","111");
callHandleModuleChange("111", "idle", "");

EXPECT_CALL(*_mock, deleteData(_))
            .Times(1)
            .WillRepeatedly(Return(true));

EXPECT_CALL(*_mock, closeSysrepo())
            .Times(1)
            .WillRepeatedly(Return(true));

EXPECT_TRUE(_mobileClient->unRegisterClient());
}

TEST_F(MobileClientTest, unregisterClientFalse)
{
registerTestClient("Max","111");
callHandleModuleChange("111", "busy", "");

EXPECT_FALSE(_mobileClient->unRegisterClient());
}





