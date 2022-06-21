#include "ZinxTimer.h"
#include <sys/timerfd.h>
#include <unistd.h>
//创建定时器文件描述符
bool ZinxTimerChannel::Init()
{

    bool bRet = false;
    //创建文件描述符
    int iFd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (0 <= iFd)
    {
        //设置定时周期
        struct itimerspec period = {
            {1, 0}, {1, 0}
        };
        if (0 == timerfd_settime(iFd, 0, &period, NULL))
        {
            bRet = true;
            m_Timerfd = iFd;
        }
        
    }
    
    return bRet;
}

//读取超时次数
bool ZinxTimerChannel::ReadFd(std::string& _input)
{
    bool bRet = false;;
    char buff[8] = { 0 };

    if (sizeof(buff) == read(m_Timerfd, buff, sizeof(buff)))
    {
        bRet = true;
        _input.assign(buff, sizeof(buff));
    }
    return bRet;
}

bool ZinxTimerChannel::WriteFd(std::string& _output)
{
    return false;
}

//关闭文件描述符
void ZinxTimerChannel::Fini()
{
    close(m_Timerfd);
    m_Timerfd = -1;
}

//返回读取的定时器文件描述符
int ZinxTimerChannel::GetFd()
{
    return m_Timerfd;
}

std::string ZinxTimerChannel::GetChannelInfo()
{
    return "TimerFd";
}

class output_hello : public AZinxHandler {
    // 通过 AZinxHandler 继承
    virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
    {
        auto pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
        std::string output = "hello world";
        ZinxKernel::Zinx_SendOut(output, *pchannel);
        return nullptr;
    }
    virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
    {
        return nullptr;
    }
} *pout_hello = new output_hello();



//返回处理超时事件的对象
AZinxHandler* ZinxTimerChannel::GetInputNextStage(BytesMsg& _oInput)
{
    return &TimerOutMng::GetInstance();
}




//设置单例
TimerOutMng TimerOutMng::single;


TimerOutMng::TimerOutMng()
{
    //创建10个齿轮
    for (int i = 0; i < WHELLSIZE; i++)
    {
        std::list<TimerOutProc *> tmp;
        m_timer_wheel.emplace_back(tmp);
    }
}

IZinxMsg* TimerOutMng::InternelHandle(IZinxMsg& _oInput)
{
    //因为定时器会返回 距离上次接收 超时多少次 
    //所以我们将超时次数记录下来 防止因为处理多个定时器导致 
    //没有接收到定时器超时信号
    uint64_t iTimeOutCout = 0;
    GET_REF2DATA(BytesMsg, oBytes, _oInput);
    oBytes.szData.copy((char*)&iTimeOutCout, sizeof(iTimeOutCout), 0);

    while (iTimeOutCout-- > 0)
    {
        //移动刻度 
        cur_index = (++cur_index) % WHELLSIZE;
        std::list<TimerOutProc*> m_cache;
        //遍历当前刻度的所有节点 有圈数的话 -1 或者执行处理函数
        //for (auto task : m_timer_wheel[cur_index])
        //不能使用for(auto) 因为 当超时后 需要将定时器删除然后再添加下一个位置
        // 这样会导致迭代器生效  需要记录list原本的长度来遍历 防止迭代器失效
        for (auto itr = m_timer_wheel[cur_index].begin(); itr != m_timer_wheel[cur_index].end();)
        {
            if ((*itr)->iCount <= 0)
            {
                //这样直接运行定时器的处理函数 存在安全隐患
                //(*itr)->Proc() : TimerOutMng::GetInstance().DelTask(this); 删除他自己
                //这样做会导致 接下来的迭代器全部失效 直接报错
                //解决方法 根据缓存的思路   可以将超时的定时器 放入一个容器中 等到遍历完成一次 后再执行
                //(*itr)->Proc();
                m_cache.emplace_back(*itr);
                auto ptmp = *itr;
                itr = m_timer_wheel[cur_index].erase(itr);
                //删除迭代器后 会出现迭代器失效 因为他不知道下一个是那里
                //而erase 会返回下一个有效迭代器的值
                //所以我们不用操作  将返回值给itr即可

                AddTask(ptmp);
            }
            else {
                (*itr)->iCount--;
                ++itr;
            }
        }

        //统一处理本次超时任务
        for (auto& task : m_cache)
        {
            task->Proc();
        }
    }
    
    return nullptr;
}

AZinxHandler* TimerOutMng::GetNextHandler(IZinxMsg& _oNextMsg)
{
    return nullptr;
}

void TimerOutMng::AddTask(TimerOutProc* _ptask)
{
    //计算当前任务需要放到哪个齿轮上
    int index = (_ptask->GetTimeSec() + cur_index )% WHELLSIZE;
    //把任务存放到该齿轮上
    m_timer_wheel[index].emplace_back(_ptask);
    //计算所需圈数
    _ptask->iCount =  _ptask->GetTimeSec() / WHELLSIZE;

    /*m_task_list.emplace_back(_ptask);
    _ptask->iCount = _ptask->GetTimeSec();*/
}

void TimerOutMng::DelTask(TimerOutProc* _ptask)
{
    // 遍历时间轮所有齿轮 删掉任务
    //m_task_list.remove(_ptask);

     // 默认拷贝记得 &
    for (auto& chi : m_timer_wheel)
    {
        for (auto task : chi)
        {
            if (task == _ptask)
            {
                chi.remove(_ptask);
                break;
            }
        }
    }
}
