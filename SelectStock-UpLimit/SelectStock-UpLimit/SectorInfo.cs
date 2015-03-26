using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SelectStock_UpLimit
{
    class SectorInfo
    {
        public SectorInfo()
        {
            SectorName = "";
            SectorACount = 0;
			inflowRatio = new List<double>();
            stocksUpLmt = new List<string>();
            stocks = new List<string>();
			freeFloatShares = new List<double>();
			closePriceMatrix = new List<List<double>>();
        }
        public SectorInfo(string aSectorName, int aSectorACount, List<string> aStocksUpLmt, List<string> aStocks, List<double> aInflowRatio, List<double> aFreeFloatShares, List<List<double>> aClosePrice)
        {
            SectorName = aSectorName;
            SectorACount = aSectorACount;
            stocksUpLmt = aStocksUpLmt;
            stocks = aStocks;
			inflowRatio = aInflowRatio;
			freeFloatShares = aFreeFloatShares;
			closePriceMatrix = aClosePrice;
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
        public List<string> stocksUpLmt;
        public List<string> stocks;
        public int SectorACount{set;get;}
		public List<double> inflowRatio;
		public List<double> freeFloatShares;
		public List<List<double>> closePriceMatrix;
		public double AverageInflowRatio
		{
			get
			{
				double total = 0;
				int count = 0;
				foreach (double fr in inflowRatio)
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
				return SolveWeightAverageInflowRatio(inflowRatio, freeFloatShares, closePriceMatrix[closePriceMatrix.Count-1]);
			}
		}
		public List<string> limitedStockList = new List<string>();
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
		public List<string> stockList = new List<string>();
		public string StockList
		{
			get
			{
				string list = "";
				foreach (string ticker in stockList)
				{
					list += ticker + ",";
				}
				return list;
			}
		}
    }
}
