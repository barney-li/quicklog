﻿using System;
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
        }
        public SectorInfo(string aSectorName, int aSectorACount, List<string> aStocksUpLmt, List<string> aStocks, List<double> aInflowRatio)
        {
            SectorName = aSectorName;
            SectorACount = aSectorACount;
            stocksUpLmt = aStocksUpLmt;
            stocks = aStocks;
			inflowRatio = aInflowRatio;
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
		public double AverageInflowRatio
		{
			get
			{
				double total = 0;
				int count = 0;
				foreach(double fr in inflowRatio)
				{
					if (!double.IsNaN(fr))
					{
						count++;
						total += fr;
					}					
				}
				return total / count;
			}
 			//complete this tomorrow;
		}
    }
}