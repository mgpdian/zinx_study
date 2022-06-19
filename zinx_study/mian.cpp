#include <zinx.h>
#include <iostream>

/*读标准输入 回响到标准输出*/
/*2-写功能处理类*/
//负责写操作的类
//AZinxHandler 重写处理信息方法和获取下一个处理着方法
class Echo : public AZinxHandler {
	// 通过 AZinxHandler 继承
	/*信息处理函数，开发者重写该函数实现信息的处理，
	当有需要一个环节继续处理时，应该创建新的信息对象（堆对象）并返回指针*/
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		/*回响到标准输出*/

		//GET_REF2DATA 作用 将_oInput 转换成 BytesMsg 的类型, 写进 input
		GET_REF2DATA(BytesMsg, input, _oInput);

		std::cout << input.szData << std::endl;

		return nullptr;
	}

	/*获取下一个处理环节函数，开发者重写该函数，
	可以指定当前处理环节结束后下一个环节是什么，
	通过参数信息对象，可以针对不同情况进行分别处理*/

	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return nullptr;
	}
}*poEcho = new Echo();

///*3-写通道类*/
//管道类
//接收信息类?
//Ichannel 通道类，派生自基础处理者类，提供基于系统调用的数据收发功能
//一般地，用户应该根据处理的文件（信息源）不同而
//创建通道类的子类或选用合适的实用类（已经提供的通道类子类）来完成系统级文件IO
class TestStdin : public Ichannel
{
	// 通过 Ichannel 继承
	/*通道初始化函数，应该重写这个函数实现打开文件和一定的参数配置
	该函数会在通道对象添加到zinxkernel时被调用
	也就是 初始化  要将其改为true*/
	virtual bool Init() override
	{
		return true;
	}
	/*读取数据， 开发者应该根据目标文件不同，重写这个函数，以实现将fd中的数据读取到参数_string中
	该函数会在数据源所对应的文件有数据到达时被调用
	ReadFd  为 获取信息 也就是最初步的地点*/
	virtual bool ReadFd(std::string& _input) override
	{
		std::cin >> _input;
		return true;
	}
	/*写出数据， 开发者应该将数据写出的操作通过重写该函数实现
	该函数会在调用zinxkernel::sendout函数后被调用（通常不是直接调用而是通过多路复用的反压机制调用）*/
	virtual bool WriteFd(std::string& _output) override
	{
		return false;
	}
	/*通道去初始化，开发者应该在该函数回收相关资源
	该函数会在通道对象从zinxkernel中摘除时调用
	资源回收  */
	virtual void Fini() override
	{
	}
	/*获取文件描述符函数， 开发者应该在该函数返回当前关心的文件，
	一般地，开发者应该在派生类中定义属性用来记录数据来记录当前IO所用的文件，在此函数中只是返回该属性
	读取信息的位置 现在我们传入的信息地点是屏幕 所以文件描述符是0  也就是 STDIN_FILENO*/
	virtual int GetFd() override
	{
		//return 0; //这个也可以
		return STDIN_FILENO;
	}
	/*获取通道信息函数，开发者可以在该函数中返回跟通道相关的一些特征字符串，方便后续查找和过滤*/
	virtual std::string GetChannelInfo() override
	{
		return "stdin";
	}
	/*获取下一个处理环节，开发者应该重写该函数，指定下一个处理环节
	一般地，开发者应该在该函数返回一个协议对象，用来处理读取到的字节流
	这个函数  是决定 获取数据信息后  接下来要去那个处理函数  */
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override
	{
		return poEcho;
	}
};
int main()
{
	/*1 - 初始化框架*/
	//初始化框架
	ZinxKernel::ZinxKernelInit();


	/*4-将通道对象添加到框架*/
	//管道初始化
	TestStdin* poStdin = new TestStdin();
	//添加管道
	ZinxKernel::Zinx_Add_Channel(*poStdin);
	//ZinxKernel::Zinx_Add_Channel(*poOut);


	/*5-运行框架*/
	ZinxKernel::Zinx_Run();


	//框架资源的回收 也就是结束框架
	ZinxKernel::ZinxKernelFini();
	return 0;
}

