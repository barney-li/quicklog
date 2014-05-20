#pragma once
#include <string>
#include <vector>
#include <cmath>
using namespace std;
namespace Finicial
{
//
//typedef struct BollingerBandData
//{
//	double mMidLine;
//	double mStdDev;
//	double mOutterUpperLine;
//	double mInnerUpperLine;
//	double mInnerLowerLine;
//	double mOutterLowerLine;
//};
//typedef struct BollingerBandDataInt
//{
//		long long mMidLine;
//		double mStdDev;
//		long long  mOutterUpperLine;
//		long long  mInnerUpperLine;
//		long long  mInnerLowerLine;
//		long long  mOutterLowerLine;
//}; 
//class BollingerBands
//{
//private:
//	vector<BollingerBandDataInt> mIntBollData;
//	vector<long long> mPrice;
//	long long mIndex;
//	long long mSizeIncrease;
//	long long mBollPeriod;
//public:
//	// constructor
//	BollingerBands()
//	{
//		mSizeIncrease = 4096;
//		mPrice.resize(mSizeIncrease, 0);
//		mIntBollData.resize(mSizeIncrease);
//		mIndex = 0;
//		mBollPeriod = 0;
//	}
//public:
//	bool IsBollReady()
//	{
//		return (mIndex>=mBollPeriod);
//	}
//	// calculate Bollinger Band, must be invoked for every coming data
//	BollingerBandData CalcBoll(double aLastPrice, int aPeriod, int aOutterAmp, int aInnerAmp)
//	{
//		mBollPeriod = aPeriod;
//		// allocate another mSizeIncrease every time the buffer is almost full
//		mIndex++;
//		if(mPrice.size() - mIndex < 100)
//		{
//			mPrice.resize(mPrice.size()+mSizeIncrease, 0);
//		}
//		if(mIntBollData.size() - mIndex < 100)
//		{
//			mIntBollData.resize(mIntBollData.size()+mSizeIncrease);
//		}
//		long long lLastPrice = (long long)aLastPrice;
//		mPrice[mIndex] = lLastPrice;
//		mIntBollData[mIndex].mMidLine = Average(aPeriod);
//		mIntBollData[mIndex].mStdDev = StandardDev(aPeriod, mIntBollData[mIndex].mMidLine);
//		mIntBollData[mIndex].mOutterUpperLine = mIntBollData[mIndex].mMidLine + (aOutterAmp * (long long)(mIntBollData[mIndex].mStdDev + 0.5));
//		mIntBollData[mIndex].mOutterLowerLine = mIntBollData[mIndex].mMidLine - (aOutterAmp * (long long)(mIntBollData[mIndex].mStdDev + 0.5));
//		mIntBollData[mIndex].mInnerUpperLine = mIntBollData[mIndex].mMidLine + (aInnerAmp * (long long)(mIntBollData[mIndex].mStdDev + 0.5));
//		mIntBollData[mIndex].mInnerLowerLine = mIntBollData[mIndex].mMidLine - (aInnerAmp * (long long)(mIntBollData[mIndex].mStdDev + 0.5));
//		
//		return GetBoll(0);
//	}
//	// calculate Bollinger Band, must be invoked for every coming data
//	BollingerBandDataInt CalcBollInt(double aLastPrice, int aPeriod, int aOutterAmp, int aInnerAmp)
//	{
//		BollingerBandDataInt lReturn;
//		// allocate another mSizeIncrease every time the buffer is almost full
//		mIndex++;
//		if(mPrice.size() - mIndex < 100)
//		{
//			mPrice.resize(mPrice.size()+mSizeIncrease, 0);
//		}
//		if(mIntBollData.size() - mIndex < 100)
//		{
//			mIntBollData.resize(mIntBollData.size()+mSizeIncrease);
//		}
//
//		long long lLastPrice = (long long)aLastPrice;
//		mPrice[mIndex] = lLastPrice;
//		mIntBollData[mIndex].mMidLine = Average(aPeriod);
//		mIntBollData[mIndex].mStdDev = StandardDev(aPeriod, mIntBollData[mIndex].mMidLine);
//		mIntBollData[mIndex].mOutterUpperLine = mIntBollData[mIndex].mMidLine + (aOutterAmp * (long long)(mIntBollData[mIndex].mStdDev + 0.5));
//		mIntBollData[mIndex].mOutterLowerLine = mIntBollData[mIndex].mMidLine - (aOutterAmp * (long long)(mIntBollData[mIndex].mStdDev + 0.5));
//		mIntBollData[mIndex].mInnerUpperLine = mIntBollData[mIndex].mMidLine + (aInnerAmp * (long long)(mIntBollData[mIndex].mStdDev + 0.5));
//		mIntBollData[mIndex].mInnerLowerLine = mIntBollData[mIndex].mMidLine - (aInnerAmp * (long long)(mIntBollData[mIndex].mStdDev + 0.5));
//		
//		memcpy(&lReturn, &mIntBollData[mIndex], sizeof(lReturn));
//
//		
//		return lReturn;
//	}
//private:
//	// calculate moving average
//	long long Average(int aPeriod)
//	{
//		long long lSum = 0;
//		long long lAvg = 0;
//		int lStartIndex=mIndex-aPeriod+1;
//		if(lStartIndex>=0)
//		{
//			for(int i=lStartIndex; i<=mIndex; i++)
//			{
//				lSum += mPrice[i];
//			}
//			lAvg = lSum/aPeriod;
//		}
//		return lAvg;
//	}
//private:
//	// calculate standard deviation
//	double StandardDev(int aPeriod, long long aAvg)
//	{
//		long long lSum = 0;
//		double lAvg = 0;
//		int lStartIndex=mIndex-aPeriod+1;
//		if(lStartIndex>=0)
//		{
//			for(int i=lStartIndex; i<=mIndex; i++)
//			{
//				/* this loop should be the biggest time consumer*/
//				lSum += (mPrice[i]-aAvg)*(mPrice[i]-aAvg);
//			}
//			lAvg = sqrt((double)lSum/(double)aPeriod);
//		}
//		return lAvg;
//	}
//public:
//	// get history Bollinger Band data by index
//	BollingerBandData GetBoll(long long back)
//	{
//		long long index = mIndex - back;
//		BollingerBandData lReturn;
//		if(index>=0)
//		{
//			lReturn.mMidLine = (double)mIntBollData[index].mMidLine;
//			lReturn.mStdDev = (double)mIntBollData[index].mStdDev;
//			lReturn.mOutterUpperLine = (double)mIntBollData[index].mOutterUpperLine;
//			lReturn.mOutterLowerLine = (double)mIntBollData[index].mOutterLowerLine;
//			lReturn.mInnerUpperLine = (double)mIntBollData[index].mInnerUpperLine;
//			lReturn.mInnerLowerLine = (double)mIntBollData[index].mInnerLowerLine;
//		}
//		else
//		{
//			lReturn.mMidLine = 0.0;
//			lReturn.mStdDev = 0.0;
//			lReturn.mOutterUpperLine = 0.0;
//			lReturn.mOutterLowerLine = 0.0;
//			lReturn.mInnerUpperLine = 0.0;
//			lReturn.mInnerLowerLine = 0.0;
//		}
//		return lReturn;
//	}
//	// get history Bollinger Band data by index
//	BollingerBandDataInt GetBollInt(long long back)
//	{
//		long long index = mIndex - back;
//		BollingerBandDataInt lReturn;
//		if(index>=0)
//		{
//			memcpy(&lReturn, &mIntBollData[mIndex], sizeof(lReturn));
//		}
//		else
//		{
//			memset(&lReturn, 0, sizeof(lReturn));
//		}
//		return lReturn;
//	}
//};
}