#pragma once
#include <zinx.h>
#include <list>
#include <vector>


class ZinxTimerChannel :
    public Ichannel
{
private:
    int m_Timerfd = -1;  //记录定时器的文件描述符  
public:
    // 通过 Ichannel 继承
    virtual bool Init() override;
    virtual bool ReadFd(std::string& _input) override;
    virtual bool WriteFd(std::string& _output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

//超时任务
class TimerOutProc {
public:
    virtual void Proc() = 0;
    virtual int GetTimeSec() = 0;
    //所余的圈数
    int iCount = -1;
};

//时间轮 齿轮数
const int WHELLSIZE = 10;
//单例
class TimerOutMng : public AZinxHandler
{
private:
    //std::list<TimerOutProc*> m_task_list;
    std::vector<std::list<TimerOutProc* >> m_timer_wheel; //时间轮

    int cur_index = 0; //刻度 初始化的情况下 指针指向的位置(即 0点)
    static TimerOutMng single;

    TimerOutMng();
public:
    // 通过 AZinxHandler 继承
    //处理超时事件 遍历所有超时任务
    virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override;
    virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override;

    void AddTask(TimerOutProc* _ptask);
    void DelTask(TimerOutProc* _ptask);
    //返回单例
    static TimerOutMng& GetInstance()
    {
        return single;
    }
};