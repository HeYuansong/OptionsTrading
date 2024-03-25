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
	// ���캯������Ҫһ����Ч��ָ��CQdpFtdcMduserApiʵ����ָ��
	CSimpleHandler(CQdFtdcMduserApi* pUserApi) : m_pUserApi(pUserApi) {}

	~CSimpleHandler() {}

	// ���ͻ��������鷢��������������ͨ�����ӣ��ͻ�����Ҫ���е�¼
	void OnFrontConnected()
	{
		cout << "������������OnFrontConnected" << endl;
		
		CQdFtdcReqUserLoginField reqUserLogin;

		//CThostFtdcReqUserLoginField loginReq;
		//memset(&loginReq, 0, sizeof(loginReq));

		
		
		strcpy(reqUserLogin.TradingDay, m_pUserApi->GetTradingDay());
		strcpy(reqUserLogin.BrokerID, "9999");
		strcpy(reqUserLogin.UserID, "212266");
		strcpy(reqUserLogin.Password, "Ysd123456");



		int res = 12;
			res=m_pUserApi->ReqUserLogin(reqUserLogin, rand()%100);
cout << "��¼���ش���" << res << endl;
cout << "m_pUserApi->GetTradingDay()" << m_pUserApi->GetTradingDay() << endl;
	}

	// ���ͻ��������鷢��������ͨ�����ӶϿ�ʱ���÷���������
	void OnFrontDisconnected()
	{
		cout << "OnFrontDisconnected" << endl;
		// ��������������API���Զ��������ӣ��ͻ��˿ɲ�������
		printf("OnFrontDisconnected.\n");
	}

	// ���ͻ��˷�����¼����֮�󣬸÷����ᱻ���ã�֪ͨ�ͻ��˵�¼�Ƿ�ɹ�
	void OnRspUserLogin(CQdFtdcRspUserLoginField* pRspUserLogin, CQdFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		cout << "OnRspUserLogin" << endl;
		printf("OnRspUserLogin:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (pRspInfo->ErrorID != 0)
		{
			// �˵�ʧ�ܣ��ͻ�������д�����
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

	// �������֪ͨ�����������������֪ͨ�ͻ���
	void OnRtnDepthMarketData(CQdFtdcDepthMarketDataField* pMarketData)
	{
		// �ͻ��˰��账���ص�����
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

	// ����û�����ĳ���֪ͨ
	void OnRspError(CQdFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		cout << "onerror" << endl;
		printf("OnRspError:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
		// �ͻ�������д�����
	}

	///���ĺ�Լ�������Ϣ
	void OnRspSubMarketData(CQdFtdcSpecificInstrumentField* pSpecificInstrument, CQdFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("Sub ���ض��ĺ�Լ��%s \n", pSpecificInstrument->InstrumentID);
	}

	///���ĺ�Լ�������Ϣ
	void OnRspUnSubMarketData(CQdFtdcSpecificInstrumentField* pSpecificInstrument, CQdFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("UnSub ���ض��ĺ�Լ��%s \n", pSpecificInstrument->InstrumentID);
	}

private:
	// ָ��CQdpFtdcMduserApiʵ����ָ��
	CQdFtdcMduserApi* m_pUserApi;
};

int main()
{
	// ����һ��CQdpFtdcMduserApiʵ��
	CQdFtdcMduserApi* pUserApi = CQdFtdcMduserApi::CreateFtdcMduserApi();
	// ����һ���¼������ʵ��
	CSimpleHandler sh(pUserApi);
	//��ӡ
	

		
	


	// ע��һ�¼������ʵ��
	pUserApi->RegisterSpi(&sh);
	// ע����Ҫ�������������
	///        TERT_RESTART:�ӱ������տ�ʼ�ش�
	///        TERT_RESUME:���ϴ��յ�������
	///        TERT_QUICK:�ȴ��͵�ǰ�������,�ٴ��͵�¼���г����������	//pUserApi-> SubscribeMarketDataTopic (101, TERT_RESUME);
	//pUserApi-> SubscribeMarketDataTopic (110, QD_TERT_RESTART);
	// �������鷢���������ĵ�ַ
	pUserApi->RegisterFront("tcp://180.168.146.187:10131");	
	//pUserApi->RegisterFront("tcp://192.168.1.100:7220");	

	// ʹ�ͻ��˿�ʼ�����鷢����������������
	pUserApi->Init();

	CQdFtdcReqUserLoginField* loginfield = new CQdFtdcReqUserLoginField;
	//loginfield->TradingDay = "111";
	//loginfield->BrokerID = "9999";
	//loginfield->ParticipantID = "212266";
	//loginfield->Password ="Ysd123456!";
	

	//pUserApi->ReqUserLogin(loginfield, 23);
	//��������Ӵ���
	const char* ii = pUserApi->GetTradingDay();
	cout << "������Ϊ" << ii << endl;
	

	// �ͷ�useapiʵ��
	pUserApi->Release();
	cout << "release" << endl;
	return 0;
}
