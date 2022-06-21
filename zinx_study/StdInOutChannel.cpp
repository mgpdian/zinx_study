#include "StdInOutChannel.h"
#include "CmdCheck.h"
#include <iostream>


bool StdInChannel::Init()
{
    return true;
}

bool StdInChannel::ReadFd(std::string& _input)
{
    std::cin >> _input;
    return true;
}

bool StdInChannel::WriteFd(std::string& _output)
{
    return false;
}

void StdInChannel::Fini()
{
}

int StdInChannel::GetFd()
{
    //0
    return STDIN_FILENO;
}

std::string StdInChannel::GetChannelInfo()
{
    return "stdin";
}

AZinxHandler* StdInChannel::GetInputNextStage(BytesMsg& _oInput)
{
    /*返回协议对象*/
    return CmdCheck::GetInstance();
}


bool StdOutChannel::Init()
{
    return true;
}

bool StdOutChannel::ReadFd(std::string& _input)
{
    return false;
}

bool StdOutChannel::WriteFd(std::string& _output)
{
    std::cout << _output << std::endl;
    return true;
}

void StdOutChannel::Fini()
{
}

int StdOutChannel::GetFd()
{   
    //1
    return STDOUT_FILENO;
}

std::string StdOutChannel::GetChannelInfo()
{
    return "stdout";
}

AZinxHandler* StdOutChannel::GetInputNextStage(BytesMsg& _oInput)
{

    return nullptr;
}

AZinxHandler* myTcpData::GetInputNextStage(BytesMsg& _oInput)
{
    /*返回协议对象*/
    return CmdCheck::GetInstance(); //转到协议类
}

ZinxTcpData* myFact::CreateTcpDataChannel(int _fd)
{
    return new myTcpData(_fd);
}
