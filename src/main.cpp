#include <stdio.h>
#include <string.h>
#include <float.h>
#include <iostream>
#include "..\libs\QdFtdcMdApi\QdFtdcMdApi.h"
using namespace std;


class CSimpleHandler : public CQdpFtdcMduserSpi
{
public:
	// ���캯������Ҫһ����Ч��ָ��CQdpFtdcMduserApiʵ����ָ��
	CSimpleHandler(CQdpFtdcMduserApi* pUserApi) : m_pUserApi(pUserApi) {}

	~CSimpleHandler() {}

	// ���ͻ��������鷢��������������ͨ�����ӣ��ͻ�����Ҫ���е�¼
	void OnFrontConnected()
	{
		CQdpFtdcReqUserLoginField reqUserLogin;
		strcpy(reqUserLogin.TradingDay, m_pUserApi->GetTradingDay());
		strcpy(reqUserLogin.BrokerID, "0001");
		strcpy(reqUserLogin.UserID, "t002");
		strcpy(reqUserLogin.Password, "111111");
		m_pUserApi->ReqUserLogin(&reqUserLogin, 0);
	}

	// ���ͻ��������鷢��������ͨ�����ӶϿ�ʱ���÷���������
	void OnFrontDisconnected()
	{
		// ��������������API���Զ��������ӣ��ͻ��˿ɲ�������
		printf("OnFrontDisconnected.\n");
	}

	// ���ͻ��˷�����¼����֮�󣬸÷����ᱻ���ã�֪ͨ�ͻ��˵�¼�Ƿ�ɹ�
	void OnRspUserLogin(CQdpFtdcRspUserLoginField* pRspUserLogin, CQdpFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
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
	void OnRtnDepthMarketData(CQdpFtdcDepthMarketDataField* pMarketData)
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
	void OnRspError(CQdpFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspError:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
		// �ͻ�������д�����
	}

	///���ĺ�Լ�������Ϣ
	void OnRspSubMarketData(CQdpFtdcSpecificInstrumentField* pSpecificInstrument, CQdpFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("Sub ���ض��ĺ�Լ��%s \n", pSpecificInstrument->InstrumentID);
	}

	///���ĺ�Լ�������Ϣ
	void OnRspUnSubMarketData(CQdpFtdcSpecificInstrumentField* pSpecificInstrument, CQdpFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("UnSub ���ض��ĺ�Լ��%s \n", pSpecificInstrument->InstrumentID);
	}

private:
	// ָ��CQdpFtdcMduserApiʵ����ָ��
	CQdpFtdcMduserApi* m_pUserApi;
};

int main()
{
	// ����һ��CQdpFtdcMduserApiʵ��
	CQdpFtdcMduserApi* pUserApi = CQdpFtdcMduserApi::CreateFtdcMduserApi();
	// ����һ���¼������ʵ��
	CSimpleHandler sh(pUserApi);
	// ע��һ�¼������ʵ��
	pUserApi->RegisterSpi(&sh);
	// ע����Ҫ�������������
	///        TERT_RESTART:�ӱ������տ�ʼ�ش�
	///        TERT_RESUME:���ϴ��յ�������
	///        TERT_QUICK:�ȴ��͵�ǰ�������,�ٴ��͵�¼���г����������	//pUserApi-> SubscribeMarketDataTopic (101, TERT_RESUME);
	//pUserApi-> SubscribeMarketDataTopic (110, QDP_TERT_RESTART);
	// �������鷢���������ĵ�ַ
	pUserApi->RegisterFront("tcp://192.168.1.100:7220");
	// ʹ�ͻ��˿�ʼ�����鷢����������������
	pUserApi->Init();
	//��������Ӵ���

	// �ͷ�useapiʵ��
	pUserApi->Release();
	return 0;
}
