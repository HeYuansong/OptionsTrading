#include <stdio.h>
#include <string.h>
#include <float.h>
#include <iostream>
#include "../libs/QdFtdcMdApi/QdFtdcMdApi.h"
#include "../libs\TraderApi\apitraderapi\QdpFtdcTraderApi.h"
using namespace std;


class CSimpleHandler : public CQdFtdcMduserSpi
{
public:
	// 构造函数，需要一个有效的指向CQdpFtdcMduserApi实例的指针
	CSimpleHandler(CQdFtdcMduserApi* pUserApi) : m_pUserApi(pUserApi) {}

	~CSimpleHandler() {}

	// 当客户端与行情发布服务器建立起通信连接，客户端需要进行登录
	void OnFrontConnected()
	{
		cout << "建立起来连接OnFrontConnected" << endl;
		
		CQdFtdcReqUserLoginField reqUserLogin;

		//CThostFtdcReqUserLoginField loginReq;
		//memset(&loginReq, 0, sizeof(loginReq));

		
		
		strcpy(reqUserLogin.TradingDay, m_pUserApi->GetTradingDay());
		strcpy(reqUserLogin.BrokerID, "9999");
		strcpy(reqUserLogin.UserID, "212266");
		strcpy(reqUserLogin.Password, "Ysd123456");



		int res = 12;
			res=m_pUserApi->ReqUserLogin(reqUserLogin, rand()%100);
cout << "登录返回代码" << res << endl;
cout << "m_pUserApi->GetTradingDay()" << m_pUserApi->GetTradingDay() << endl;
	}

	// 当客户端与行情发布服务器通信连接断开时，该方法被调用
	void OnFrontDisconnected()
	{
		cout << "OnFrontDisconnected" << endl;
		// 当发生这个情况后，API会自动重新连接，客户端可不做处理
		printf("OnFrontDisconnected.\n");
	}

	// 当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
	void OnRspUserLogin(CQdFtdcRspUserLoginField* pRspUserLogin, CQdFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		cout << "OnRspUserLogin" << endl;
		printf("OnRspUserLogin:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (pRspInfo->ErrorID != 0)
		{
			// 端登失败，客户端需进行错误处理
			printf("Failed to login, errorcode=%d errormsg=%s requestid=%d chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
			return;
		}
		char* contracts[3] = { "","","" };
		contracts[0] = "IF1503";
		contracts[1] = "IF1504";
		contracts[2] = "IF1506";
		m_pUserApi->SubMarketData(contracts, 3);

		char* uncontracts[2] = { "","" };
		uncontracts[0] = "IF1509";
		uncontracts[1] = "IH1503";
		m_pUserApi->UnSubMarketData(uncontracts, 2);
	}

	// 深度行情通知，行情服务器会主动通知客户端
	void OnRtnDepthMarketData(CQdFtdcDepthMarketDataField* pMarketData)
	{
		// 客户端按需处理返回的数据
		printf("%s,%s,%d,", pMarketData->InstrumentID, pMarketData->UpdateTime, pMarketData->UpdateMillisec);
		if (pMarketData->AskPrice1 == DBL_MAX)
			printf("%s,", "");
		else
			printf("%f,", pMarketData->AskPrice1);

		if (pMarketData->BidPrice1 == DBL_MAX)
			printf("%s \n", "");
		else
			printf("%f \n", pMarketData->BidPrice1);
	}

	// 针对用户请求的出错通知
	void OnRspError(CQdFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		cout << "onerror" << endl;
		printf("OnRspError:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
		// 客户端需进行错误处理
	}

	///订阅合约的相关信息
	void OnRspSubMarketData(CQdFtdcSpecificInstrumentField* pSpecificInstrument, CQdFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("Sub 返回订阅合约：%s \n", pSpecificInstrument->InstrumentID);
	}

	///订阅合约的相关信息
	void OnRspUnSubMarketData(CQdFtdcSpecificInstrumentField* pSpecificInstrument, CQdFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("UnSub 返回订阅合约：%s \n", pSpecificInstrument->InstrumentID);
	}

private:
	// 指向CQdpFtdcMduserApi实例的指针
	CQdFtdcMduserApi* m_pUserApi;
};

int main()
{
	// 产生一个CQdpFtdcMduserApi实例
	CQdFtdcMduserApi* pUserApi = CQdFtdcMduserApi::CreateFtdcMduserApi();
	// 产生一个事件处理的实例
	CSimpleHandler sh(pUserApi);
	//打印
	

		
	


	// 注册一事件处理的实例
	pUserApi->RegisterSpi(&sh);
	// 注册需要的深度行情主题
	///        TERT_RESTART:从本交易日开始重传
	///        TERT_RESUME:从上次收到的续传
	///        TERT_QUICK:先传送当前行情快照,再传送登录后市场行情的内容	//pUserApi-> SubscribeMarketDataTopic (101, TERT_RESUME);
	//pUserApi-> SubscribeMarketDataTopic (110, QD_TERT_RESTART);
	// 设置行情发布服务器的地址
	pUserApi->RegisterFront("tcp://180.168.146.187:10131");	
	//pUserApi->RegisterFront("tcp://192.168.1.100:7220");	

	// 使客户端开始与行情发布服务器建立连接
	pUserApi->Init();

	CQdFtdcReqUserLoginField* loginfield = new CQdFtdcReqUserLoginField;
	//loginfield->TradingDay = "111";
	//loginfield->BrokerID = "9999";
	//loginfield->ParticipantID = "212266";
	//loginfield->Password ="Ysd123456!";
	

	//pUserApi->ReqUserLogin(loginfield, 23);
	//在这里添加代码
	const char* ii = pUserApi->GetTradingDay();
	cout << "交易日为" << ii << endl;
	

	// 释放useapi实例
	pUserApi->Release();
	cout << "release" << endl;
	return 0;
}
