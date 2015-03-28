using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SelectStock_UpLimit
{
    class SectorInfo
    {
		List<List<double>> volumeMatrix = new List<List<double>>();
		public List<string> stocksUpLmt = new List<string>();
		public List<string> stocks = new List<string>();
		/* matrix的存储结构是： somethingMatrix[day N][stock M]*/
		public List<List<double>> inflowRatioMatrix = new List<List<double>>();
		public List<List<double>> freeFloatSharesMatrix = new List<List<double>>();
		public List<List<double>> closePriceMatrix = new List<List<double>>();
		public List<string> limitedStockList = new List<string>();
		//public List<string> stockList = new List<string>();

        public SectorInfo()
        {
            SectorName = "";
            SectorACount = 0;
        }
        public SectorInfo(string aSectorName, 
						int aSectorACount, 
						List<string> aStocksUpLmt, 
						List<string> aStocks, 
						List<List<double>> aInflowRatioMatrix, 
						List<List<double>> aFreeFloatSharesMatrix, 
						List<List<double>> aClosePriceMatrix,
						List<List<double>> aVolumeMatrix)
        {
            SectorName = aSectorName;
            SectorACount = aSectorACount;
            stocksUpLmt = aStocksUpLmt;
            stocks = aStocks;
			if (aInflowRatioMatrix.Count < 6 || aFreeFloatSharesMatrix.Count < 6 || aClosePriceMatrix.Count < 6)
			{
				Console.WriteLine("error, SectorInfor requires at least 5 days of data");
				return;
			}
			else
			{
				inflowRatioMatrix = aInflowRatioMatrix;
				freeFloatSharesMatrix = aFreeFloatSharesMatrix;
				closePriceMatrix = aClosePriceMatrix;
				volumeMatrix = aVolumeMatrix;
			}
        }
        public string SectorName { get; set; }
        public int SectorCount
        {
            get 
            {
                return stocks.Count;
            }
        }
        public int SectorUpLimitCount
        {
            get
            {
                return stocksUpLmt.Count;
            }
        }
        public int SectorACount{set;get;}
		public double AverageInflowRatio
		{
			get
			{
				double total = 0;
				int count = 0;
				foreach (double fr in inflowRatioMatrix[inflowRatioMatrix.Count-1])
				{
					if (!double.IsNaN(fr))
					{
						count++;
						total += fr;
					}
				}
				return total / count;
			}
		}

		public static double SolveWeightAverageInflowRatio(List<double> aInflowRatioList, List<double> aFreeFloatSharesList, List<double> aClosePriceList)
		{
			double avgInflowRatio = 0;
			try
			{
				if (aInflowRatioList.Count != aFreeFloatSharesList.Count || aFreeFloatSharesList.Count != aClosePriceList.Count)
				{
					Console.WriteLine("error, inflow ratio, free float shares and close price doesn't match");
				}
				List<double> weight = new List<double>();
				double valueOfSector = 0;
				for (int i = 0; i < aClosePriceList.Count; i++)
				{
					if ((!double.IsNaN(aInflowRatioList[i])) &&
						(!double.IsNaN(aFreeFloatSharesList[i])) &&
						(!double.IsNaN(aClosePriceList[i])))
					{
						valueOfSector += aFreeFloatSharesList[i] * aClosePriceList[i];
					}
				}
				for (int i = 0; i < aClosePriceList.Count; i++)
				{
					if ((!double.IsNaN(aInflowRatioList[i])) &&
						(!double.IsNaN(aFreeFloatSharesList[i])) &&
						(!double.IsNaN(aClosePriceList[i])))
					{
						avgInflowRatio += aInflowRatioList[i] * aFreeFloatSharesList[i] * aClosePriceList[i] / valueOfSector;
					}
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				Console.WriteLine(e.Source);
				Console.WriteLine(e.StackTrace);
			}
			return avgInflowRatio;
		}
		public double WeightAverageInflowRatio
		{
			get
			{
				return SolveWeightAverageInflowRatio(inflowRatioMatrix[inflowRatioMatrix.Count-1], freeFloatSharesMatrix[freeFloatSharesMatrix.Count-1], closePriceMatrix[closePriceMatrix.Count-1]);
			}
		}
		public double GetWeightAverageInflowRatio(int daysAgo)
		{
			return SolveWeightAverageInflowRatio(inflowRatioMatrix[inflowRatioMatrix.Count - 1 - daysAgo],
												freeFloatSharesMatrix[freeFloatSharesMatrix.Count - 1 - daysAgo],
												closePriceMatrix[closePriceMatrix.Count - 1 - daysAgo]);
		}
		public double DeltaWeightAverageInflowRatio
		{
			get
			{
				double sum = 0;
				int deltaDays = 5;
				for (int i = 1; i <= deltaDays; i++)
				{
					sum += GetWeightAverageInflowRatio(i);
				}
				return WeightAverageInflowRatio - (sum / deltaDays);
			}
		}
		public string LimitedStockList
		{
			get
			{
				string list = "";
				foreach (string ticker in limitedStockList)
				{
					list += ticker + ",";
				}
				return list;
			}
		}
		public string StockList
		{
			get
			{
				string list = "";
				foreach (string ticker in stocks)
				{
					list += ticker + ",";
				}
				return list;
			}
		}

		public double GetVolume(int daysAgo)
		{
			double sum = 0;
			List<double> volumeList = volumeMatrix[volumeMatrix.Count-1-daysAgo];
			foreach (double volume in volumeList)
			{
				if (!double.IsNaN(volume))
				{
					sum += volume;
				}
			}
			return sum;
		}
		public double GetDeltaVolume()
		{
			int deltaDays = 5;
			double todayVolume = GetVolume(0);
			double sumVolume = 0;
			for (int i = 1; i <= deltaDays; i++)
			{
				sumVolume += GetVolume(i);
			}
			if (sumVolume == 0)
			{
				return 0;
			}
			else
			{
				return (todayVolume - sumVolume / deltaDays) / (sumVolume / deltaDays);
			}
		}

		//板块市值
		public double GetSectorValue(int daysAgo)
		{
			double sum = 0;
			if (closePriceMatrix.Count < daysAgo)
			{
				Console.WriteLine("error: require more days of close price");
			}
			if (freeFloatSharesMatrix.Count < daysAgo)
			{
				Console.WriteLine("error: require more days of free float shares");
			}
			List<double> closeList = closePriceMatrix[closePriceMatrix.Count - 1 - daysAgo];
			List<double> freeFloatSharesList = freeFloatSharesMatrix[freeFloatSharesMatrix.Count - 1 - daysAgo];
			for (int i = 0; i < closeList.Count; i++)
			{
				sum += closeList[i] * freeFloatSharesList[i];
			}
			return sum;
		}
		//板块市值涨幅
		public double GetDeltaSectorValue(int fromDaysAgo, int toDaysAgo)
		{
			double delta = 0;
			double valueBefore = GetSectorValue(fromDaysAgo);
			double valueNow = GetSectorValue(toDaysAgo);
			delta = (valueNow - valueBefore) / valueBefore;
			return delta;
		}

		//板块个股n日涨幅排行
		public string GetDeltaOrderStockList(int daysAgo, int topN)
		{
			string[] tickerArray = new string[stocks.Count];
			double[] deltaPriceArray = new double[stocks.Count];
			for (int i = 0; i < stocks.Count; i++)
			{
				tickerArray[i] = stocks[i];
				double closeToday = closePriceMatrix[closePriceMatrix.Count - 1][i];
				double closeBefore = closePriceMatrix[closePriceMatrix.Count - 1 - daysAgo][i];
				deltaPriceArray[i] = (closeToday - closeBefore)/closeBefore;
			}
			Array.Sort(deltaPriceArray, tickerArray);
			int outputCount = (tickerArray.Count()<topN)?tickerArray.Count():topN;
			string outputInfo = "";
			for (int i = tickerArray.Count()-1; i > tickerArray.Count()-1-outputCount; i--)
			{
				outputInfo += tickerArray[i] + "(" + Math.Round(deltaPriceArray[i], 2) * 100 + "%);";
			}
			return outputInfo;
		}
    }
}
