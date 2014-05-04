#pragma once
#include <string>
#include <vector>
#include <cmath>
using namespace std;
namespace Finicial
{

typedef struct BollingerBandData
{
	double mUpperLine;
	double mLowerLine;
	double mMidLine;
};

class BollingerBands
{
private:
	typedef struct BollingerBandDataInt
	{
		long long mUpperLine;
		long long mLowerLine;
		long long mMidLine;
	}; 
	vector<BollingerBandDataInt> mIntBollData;
	vector<long long> mPrice;
	long long mIndex;
	long long mSizeIncrease;
public:
	// constructor
	BollingerBands()
	{
		mSizeIncrease = 4096;
		mPrice.resize(mSizeIncrease, 0);
		mIntBollData.resize(mSizeIncrease);
		mIndex = 0;
	}
public:
	// calculate Bollinger Band, must be invoked for every coming data
	BollingerBandData CalcBoll(double aLastPrice, int aPeriod, int aAmp)
	{
		BollingerBandData lReturn;
		long long lLastPrice = (long long)aLastPrice;
		mPrice[mIndex] = lLastPrice;
		mIntBollData[mIndex].mMidLine = Average(aPeriod);
		long long lStdDev = StandardDev(aPeriod, mIntBollData[mIndex].mMidLine);
		mIntBollData[mIndex].mUpperLine = mIntBollData[mIndex].mMidLine + lStdDev;
		mIntBollData[mIndex].mLowerLine = mIntBollData[mIndex].mMidLine - lStdDev;

		lReturn.mUpperLine = (double)mIntBollData[mIndex].mUpperLine;
		lReturn.mMidLine = (double)mIntBollData[mIndex].mMidLine;
		lReturn.mLowerLine = (double)mIntBollData[mIndex].mLowerLine;

		// allocate another mSizeIncrease every time the buffer is almost full
		mIndex++;
		if(mPrice.size() - mIndex < 100)
		{
			mPrice.resize(mPrice.size()+mSizeIncrease, 0);
		}
		if(mIntBollData.size() - mIndex < 100)
		{
			mIntBollData.resize(mIntBollData.size()+mSizeIncrease);
		}
		return lReturn;
	}
private:
	// calculate moving average
	long long Average(int aPeriod)
	{
		long long lSum = 0;
		long long lAvg = 0;
		int lStartIndex=mIndex-aPeriod+1;
		if(lStartIndex>=0)
		{
			for(int i=lStartIndex; i<=mIndex; i++)
			{
				lSum += mPrice[i];
			}
			lAvg = lSum/aPeriod;
		}
		return lAvg;
	}
private:
	// calculate standard deviation
	long long StandardDev(int aPeriod, long long aAvg)
	{
		long long lSum = 0;
		double lAvg = 0;
		int lStartIndex=mIndex-aPeriod+1;
		if(lStartIndex>=0)
		{
			for(int i=lStartIndex; i<=mIndex; i++)
			{
				lSum += (mPrice[i]-aAvg)*(mPrice[i]-aAvg);
			}
			lAvg = sqrt((double)lSum/aPeriod)+0.5;// round up
		}
		return lAvg;
	}
public:
	// get history Bollinger Band data by index
	BollingerBandData GetBoll(long long back)
	{
		long long index = mIndex - back;
		BollingerBandData lReturn;
		lReturn.mUpperLine = (double)mIntBollData[index].mUpperLine;
		lReturn.mMidLine = (double)mIntBollData[index].mMidLine;
		lReturn.mLowerLine = (double)mIntBollData[index].mLowerLine;
		return lReturn;
	}
};
}