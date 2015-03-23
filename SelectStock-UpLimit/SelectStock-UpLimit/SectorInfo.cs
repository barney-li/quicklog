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
			closePrice = new List<double>();
        }
        public SectorInfo(string aSectorName, int aSectorACount, List<string> aStocksUpLmt, List<string> aStocks, List<double> aInflowRatio, List<double> aFreeFloatShares, List<double> aClosePrice)
        {
            SectorName = aSectorName;
            SectorACount = aSectorACount;
            stocksUpLmt = aStocksUpLmt;
            stocks = aStocks;
			inflowRatio = aInflowRatio;
			freeFloatShares = aFreeFloatShares;
			closePrice = aClosePrice;
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
		public List<double> closePrice;
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
		public double WeightAverageInflowRatio
		{
			get
			{
				double avgInflowRatio = 0;
				try
				{
					if (inflowRatio.Count != freeFloatShares.Count || freeFloatShares.Count != closePrice.Count)
					{
						Console.WriteLine("error, inflow ratio, free float shares and close price doesn't match");
					}
					List<double> weight = new List<double>();
					double valueOfSector = 0;
					for (int i = 0; i < closePrice.Count; i++)
					{
						if ((!double.IsNaN(inflowRatio[i])) &&
							(!double.IsNaN(freeFloatShares[i])) &&
							(!double.IsNaN(closePrice[i])))
						{
							valueOfSector += freeFloatShares[i] * closePrice[i];
						}
					}
					for (int i = 0; i < closePrice.Count; i++)
					{
						if ((!double.IsNaN(inflowRatio[i])) &&
							(!double.IsNaN(freeFloatShares[i])) &&
							(!double.IsNaN(closePrice[i])))
						{
							avgInflowRatio += inflowRatio[i] * freeFloatShares[i] * closePrice[i] / valueOfSector;
						}
					}
				}
				catch(Exception e)
				{
					Console.WriteLine(e.Message);
					Console.WriteLine(e.Source);
					Console.WriteLine(e.StackTrace);
				}
				return avgInflowRatio;
			}
		}
    }
}
