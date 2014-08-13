#include "PrimeryAndSecondary.h"
#ifdef SIMULATION
void PrimeryAndSecondary::OpeningScndStateAsyncEventGenerator(void)
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	string lCurTime = "2000-01-01 "+(string)lPrim.updateTime;
	time_duration lOpenDuration = time_from_string(lCurTime)-time_from_string(mScndOpenTime);
	if(lOpenDuration.total_milliseconds()>stgArg.scndOpenTime)
	{
		tempStream.clear();
		tempStream.str("");
		tempStream<<"[EVENT]: SCND_OPEN_TIMEOUT (from async poster)";
		SetEvent(SCND_OPEN_TIMEOUT);
	}
	if(OPEN_COND1 == mOpenCond)
	{
		if(mScndEnterPrice>=lScnd.askPrice)
		{
			mScndEnterPrice = lScnd.askPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_OPENED (from async poster) at: "<<mScndEnterPrice;
			SetEvent(SCND_OPENED);
		}// �򿪣����ҵĳ��۴��ڵ����̿�����ʱ�ɽ�
	}// �������
	else
	{
		if(mScndEnterPrice<=lScnd.bidPrice)
		{
			mScndEnterPrice = lScnd.bidPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_OPENED (from async poster) at: "<<mScndEnterPrice;
			SetEvent(SCND_OPENED);
		}// ���������ҵĳ���С�ڵ����̿����ʱ�ɽ�
	}// ��������
}
void PrimeryAndSecondary::OpeningPrimStateAsyncEventGenerator(void)
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	string lCurTime = "2000-01-01 "+(string)lPrim.updateTime;
	time_duration lOpenDuration = time_from_string(lCurTime)-time_from_string(mPrimOpenTime);
	if(lOpenDuration.total_milliseconds()>stgArg.primOpenTime)
	{
		tempStream.clear();
		tempStream.str("");
		tempStream<<"[EVENT]: PRIM_OPEN_TIMEOUT (from async poster)";
		SetEvent(PRIM_OPEN_TIMEOUT);
	}
	if(OPEN_COND2 == mOpenCond)
	{
		if(mPrimEnterPrice>=lPrim.askPrice)
		{
			mPrimEnterPrice = lPrim.askPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_OPENED (from async poster) at: "<<mPrimEnterPrice;
			SetEvent(PRIM_OPENED);
		}// �򿪣����ҵĳ��۴��ڵ����̿�����ʱ�ɽ�
	}// ������
	else
	{
		if(mPrimEnterPrice<=lPrim.bidPrice)
		{
			mPrimEnterPrice = lPrim.bidPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_OPENED (from async poster) at: "<<mPrimEnterPrice;
			SetEvent(PRIM_OPENED);
		}// ���������ҵĳ���С�ڵ����̿����ʱ�ɽ�
	}// ������
}
void PrimeryAndSecondary::ClosingBothStateAsyncEventGenerator()
{
	ClosingScndStateAsyncEventGenerator();
	ClosingPrimStateAsyncEventGenerator();
}
void PrimeryAndSecondary::ClosingScndStateAsyncEventGenerator()
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	if(OPEN_COND1 == mOpenCond)
	{
		if(mScndClosePrice<=lScnd.bidPrice)
		{
			mScndClosePrice = lScnd.bidPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_CLOSED (from async poster) at: "<<mScndClosePrice;
			SetEvent(SCND_CLOSED);
		}// ��ƽ�����ҵĳ���С�ڵ����̿����ʱ�ɽ�
	}// ��ƽ������
	else
	{
		if(mScndClosePrice>=lScnd.askPrice)
		{
			mScndClosePrice = lScnd.askPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: SCND_CLOSED (from async poster) at: "<<mScndClosePrice;
			SetEvent(SCND_CLOSED);
		}// ��ƽ�����ҵĳ��۴��ڵ����̿�����ʱ�ɽ�
	}// ��ƽ������
}
void PrimeryAndSecondary::ClosingPrimStateAsyncEventGenerator()
{
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	if(OPEN_COND2 == mOpenCond)
	{
		if(mPrimClosePrice<=lPrim.bidPrice)
		{
			mPrimClosePrice = lPrim.bidPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_CLOSED (from async poster) at: "<<mPrimClosePrice;
			SetEvent(PRIM_CLOSED);
		}// ��ƽ�����ҵĳ���С�ڵ����̿����ʱ�ɽ�
	}// ��ƽ����
	else
	{
		if(mPrimClosePrice>=lPrim.askPrice)
		{
			mPrimClosePrice = lPrim.askPrice;
			tempStream.clear();
			tempStream.str("");
			tempStream<<"[EVENT]: PRIM_CLOSED (from async poster) at: "<<mPrimClosePrice;
			SetEvent(PRIM_CLOSED);
		}// ��ƽ�����ҵĳ��۴��ڵ����̿�����ʱ�ɽ�
	}// ��ƽ����

	tempStream.clear();
	tempStream.str("");
	tempStream<<"[INFO]: estimate profit:	"<<EstimateProfit();
	logger.LogThisFast(tempStream.str());
#ifdef BACK_TEST
	mProfitLog.LogThisFast(tempStream.str());
	double lTempProfit = EstimateProfit()-stgArg.cost;// ��������һ������
	if(lTempProfit > 0)
	{
		mWin++;
	}
	else
	{
		mLose++;
	}
	mTotalProfit += lTempProfit;
#endif
}

void PrimeryAndSecondary::AsyncEventPoster(void)
{
	int lPostTime=0;
	const BasicMarketData &lPrim = primDataBuf[primBufIndex];
	const BasicMarketData &lScnd = scndDataBuf[scndBufIndex];
	switch(mStateMachine.GetState())
	{
	case IDLE_STATE:
		break;
	case OPENING_SCND_STATE:
		OpeningScndStateAsyncEventGenerator();
		break;
	case CHECKING_SCND_STATE:
		SetEvent(SCND_OPENED);
		break;
	case OPENING_PRIM_STATE:
		OpeningPrimStateAsyncEventGenerator();
		break;
	case PENDING_STATE:
		break;
	case CLOSING_BOTH_STATE:
		ClosingBothStateAsyncEventGenerator();
		break;
	case CANCELLING_SCND_STATE:
		break;
	case CLOSING_SCND_STATE:
		ClosingScndStateAsyncEventGenerator();
		break;
	case CANCELLING_PRIM_STATE:
		break;
	case WAITING_SCND_CLOSE_STATE:
		break;
	case WAITING_PRIM_CLOSE_STATE:
		break;
	default:
		break;
	}
}
void PrimeryAndSecondary::AsyncEventPoster(TRADE_EVENT aEvent)
{
	boost::this_thread::sleep(boost::posix_time::seconds(1));
	SetEvent(aEvent);
}
#endif