#pragma once
#include <vector>
using namespace std;
namespace Finicial
{
typedef struct 
{
	double mMidLine;
	double mStdDev;
	double mOutterUpperLine;
	double mInnerUpperLine;
	double mInnerLowerLine;
	double mOutterLowerLine;
}BollingerBandData;
class BollingerBand
{
private:
	vector<BollingerBandData> mBollData;
	vector<long long> mPrice;
	long long mIndex;
	long long mSizeIncrease;
	long long mBollPeriod;
	double mReservedAccuracy;
	bool mDataValid;
public:
	// constructor
	BollingerBand()
	{
		InitAllData();
	}
	// initialize private data
	void InitAllData()
	{
		mSizeIncrease = 4096;
		mPrice.resize(mSizeIncrease, 0);
		mBollData.resize(mSizeIncrease);
		mIndex = 0;
		mBollPeriod = 0;
		mReservedAccuracy = 10000;
		mDataValid = false;
	}
	bool IsBollReady()
	{
		return (mIndex>mBollPeriod+10)&&mDataValid;
	}
	bool CalcBoll(double aLastPrice, int aPeriod, double aOutterAmp, double aInnerAmp)
	{
		BollingerBandData lTempBollData;
		mBollPeriod = aPeriod;
		// check input arguments
		if(abs(aLastPrice)>100000)
		{
			mDataValid = false;
			return false;
		}
		// allocate another mSizeIncrease every time the buffer is almost full
		mIndex++;
		if(mPrice.size() - mIndex < 100)
		{
			mPrice.resize(mPrice.size()+mSizeIncrease, 0);
		}
		if(mBollData.size() - mIndex < 100)
		{
			mBollData.resize(mBollData.size()+mSizeIncrease);
		}
		// 在从double转换为整型之前，先乘以精度保留系数，这样可以在充分保留计算精度的
		// 同时保证运算速度
		mPrice[mIndex] = (long long)(aLastPrice * mReservedAccuracy);
		
		lTempBollData.mMidLine = Average(aPeriod);
		lTempBollData.mStdDev = StandardDev(aPeriod, (long long)lTempBollData.mMidLine);
		lTempBollData.mOutterUpperLine = lTempBollData.mMidLine + (aOutterAmp * lTempBollData.mStdDev);
		lTempBollData.mOutterLowerLine = lTempBollData.mMidLine - (aOutterAmp * lTempBollData.mStdDev);
		lTempBollData.mInnerUpperLine = lTempBollData.mMidLine + (aInnerAmp * lTempBollData.mStdDev);
		lTempBollData.mInnerLowerLine = lTempBollData.mMidLine - (aInnerAmp * lTempBollData.mStdDev);
		
		// 存储数据之前除以精度保留系数
		mBollData[mIndex].mMidLine = lTempBollData.mMidLine/mReservedAccuracy;
		mBollData[mIndex].mStdDev = lTempBollData.mStdDev/mReservedAccuracy;
		mBollData[mIndex].mOutterUpperLine = lTempBollData.mOutterUpperLine/mReservedAccuracy;
		mBollData[mIndex].mOutterLowerLine = lTempBollData.mOutterLowerLine/mReservedAccuracy;
		mBollData[mIndex].mInnerUpperLine = lTempBollData.mInnerUpperLine/mReservedAccuracy;
		mBollData[mIndex].mInnerLowerLine = lTempBollData.mInnerLowerLine/mReservedAccuracy;
		
		mDataValid = true;
		return true;
	}
	double Average(int aPeriod)
	{
		long long lSum = 0;
		double lAvg = 0;
		int lStartIndex=mIndex-aPeriod+1;
		if(lStartIndex>=0)
		{
			for(int i=lStartIndex; i<=mIndex; i++)
			{
				lSum += mPrice[i];
			}
			lAvg = (double)lSum/aPeriod;
		}
		return lAvg;
	}
	double StandardDev(int aPeriod, long long aAvg)
	{
		long long lSum = 0;
		double lDev = 0;
		int lStartIndex=mIndex-aPeriod+1;
		if(lStartIndex>=0)
		{
			for(int i=lStartIndex; i<=mIndex; i++)
			{
				/* this loop should be the biggest time consumer*/
				lSum += (mPrice[i]-aAvg)*(mPrice[i]-aAvg);
			}
			lDev = sqrt((double)lSum/(double)aPeriod);
		}
		return lDev;
	}
	BollingerBandData GetBoll(long long aBack)
	{
		if(mIndex - aBack>=0)
		{
			return mBollData[mIndex - aBack];
		}
		else
		{
			BollingerBandData lReturn;
			memset(&lReturn, 0, sizeof(lReturn));
			return lReturn;
		}
	}
};
}