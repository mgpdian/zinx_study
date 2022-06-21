#pragma once
#include <zinx.h>
#include <list>
#include <vector>


class ZinxTimerChannel :
    public Ichannel
{
private:
    int m_Timerfd = -1;  //��¼��ʱ�����ļ�������  
public:
    // ͨ�� Ichannel �̳�
    virtual bool Init() override;
    virtual bool ReadFd(std::string& _input) override;
    virtual bool WriteFd(std::string& _output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

//��ʱ����
class TimerOutProc {
public:
    virtual void Proc() = 0;
    virtual int GetTimeSec() = 0;
    //�����Ȧ��
    int iCount = -1;
};

//ʱ���� ������
const int WHELLSIZE = 10;
//����
class TimerOutMng : public AZinxHandler
{
private:
    //std::list<TimerOutProc*> m_task_list;
    std::vector<std::list<TimerOutProc* >> m_timer_wheel; //ʱ����

    int cur_index = 0; //�̶� ��ʼ��������� ָ��ָ���λ��(�� 0��)
    static TimerOutMng single;

    TimerOutMng();
public:
    // ͨ�� AZinxHandler �̳�
    //����ʱ�¼� �������г�ʱ����
    virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override;
    virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override;

    void AddTask(TimerOutProc* _ptask);
    void DelTask(TimerOutProc* _ptask);
    //���ص���
    static TimerOutMng& GetInstance()
    {
        return single;
    }
};