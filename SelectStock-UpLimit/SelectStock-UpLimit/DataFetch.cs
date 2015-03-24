using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using System.IO;
using WAPIWrapperCSharp;
using MathNet.Numerics.Statistics;
namespace MarketDataUtilities
{
    class DataFetch
    {
        private WindAPI wAPI = new WindAPI();
        public const string tickerCSI300 = "000300.SH";
		const int daysAfterIPO = 30;
		public const string ClosePriceType = "close";
		public const string OpenPriceType = "open";
		public const string HighPriceType = "high";
		public const string LowPriceType = "low";
		public const string CloseChangeType = "pct_chg";
		public const string VolumeType = "volume";
		public const string FreeFloatSharesType = "free_float_shares";
		public const string InflowRatioType = "mf_vol_ratio";

        public DataFetch()
        {
            Connect();
        }

        public bool Connect()
        {
            if (wAPI == null)
            {
                wAPI = new WindAPI();
            }

            if (wAPI.isconnected())
            {
                Console.WriteLine("Wind API already connected");
                return true;
            }
            else
            {
                int returnCode = wAPI.start();
                if (returnCode == 0)
                {
                    Console.WriteLine("Wind API connected");
                    return true;
                }
                else
                {
                    Console.WriteLine("Wind API cannot connected");
                    return false;
                }
            }
        }

		public List<List<double>> GetDecimalMatrix(List<string> tickerList, string dataType, string from, string to)
		{
			List<List<double>> dataMatrix = new List<List<double>>();
			try
			{
				string ticker = "";
				foreach (string oneTicker in tickerList)
				{
					ticker += oneTicker + ",";
				}
				WindData wsdResult = wAPI.wsd(ticker, dataType, from, to, "Fill=Previous;PriceAdj=F");
				if (wsdResult.data is double[])
				{
					int tickerCount = tickerList.Count;
					for (int i = 0; i < tickerCount; i++)
					{
						dataMatrix.Add(new List<double>());
					}
					int index = 0;
					foreach (double data in (double[])wsdResult.data)
					{
						dataMatrix[index % tickerCount].Add(data);
						index++;
					}
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				Console.WriteLine(e.Source);
				Console.WriteLine(e.StackTrace);
			}
			return dataMatrix;
		}

		public List<List<string>> GetStringMatrix(List<string> tickerList, string dataType, string from, string to)
		{
			List<List<string>> dataMatrix = new List<List<string>>();
			try
			{
				string ticker = "";
				foreach (string oneTicker in tickerList)
				{
					ticker += oneTicker + ",";
				}
				WindData wsdResult = wAPI.wsd(ticker, dataType, from, to, "Fill=Previous;PriceAdj=F");
				if (wsdResult.data is string[])
				{
					int tickerCount = tickerList.Count;
					for (int i = 0; i < tickerCount; i++)
					{
						dataMatrix.Add(new List<string>());
					}
					int index = 0;
					foreach (string data in (string[])wsdResult.data)
					{
						dataMatrix[index % tickerCount].Add(data);
						index++;
					}
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				Console.WriteLine(e.Source);
				Console.WriteLine(e.StackTrace);
			}
			return dataMatrix;
		}

		public List<double> GetFreeFloatShares(List<string> inputTicker, string date)
		{
			List<double> freeFloatShares = new List<double>();
			try
			{
				string ticker = "";
				foreach (string oneTicker in inputTicker)
				{
					ticker += oneTicker + ",";
				}
				WindData wsdResult = wAPI.wsd(ticker, "free_float_shares", date, date, "Fill=Previous;PriceAdj=F");
				if (wsdResult.data is object[])
				{
					return freeFloatShares;
				}
				else
				{
					double[] wsdData = (double[])wsdResult.data;
					foreach (double s in wsdData)
					{
						freeFloatShares.Add(Math.Round(s, 6));
					}
					return freeFloatShares;
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				Console.WriteLine(e.Source);
				Console.WriteLine(e.StackTrace);
			}
			return freeFloatShares;
		}

		public List<double> GetInflowRatio(List<string> inputTicker, string date)
		{
			List<double> inflowRatio = new List<double>();
			try
			{
				string ticker = "";
				foreach(string oneTicker in inputTicker)
				{
					ticker += oneTicker + ",";
				}
				WindData wsdResult = wAPI.wsd(ticker, "mf_vol_ratio", date, date, "Fill=Previous;PriceAdj=F");
				if (wsdResult.data is object[])
				{
					return inflowRatio;
				}
				else
				{
					double[] wsdData = (double[])wsdResult.data;
					foreach (double s in wsdData)
					{
						inflowRatio.Add(Math.Round(s, 6));
					}
					return inflowRatio;
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				Console.WriteLine(e.Source);
				Console.WriteLine(e.StackTrace);
			}
			return inflowRatio;
		}

        public List<string> ScreenOutNewIPO(List<string> stockList, string date, int withinDays)
        {
            List<string> afterScreenList = new List<string>();
            try
            {
                DateTime toDate = Convert.ToDateTime(date);
                DateTime fromDate = toDate.AddDays(-1 * withinDays);
                List<string> tradingDayList = GetTradingDays(tickerCSI300, string.Format("{0:yyyy-MM-dd}", fromDate), string.Format("{0:yyyy-MM-dd}", toDate));
                string screenNewIPODate = tradingDayList[0];
                string inputStockList = "";
                foreach (string stock in stockList)
                {
                    inputStockList += stock + ",";
                }
                List<double> closePriceList = GetClosePrices(inputStockList, screenNewIPODate, screenNewIPODate);

                for (int i = 0; i < closePriceList.Count; i++)
                {
                    if (!double.IsNaN(closePriceList[i]))
                    {
                        afterScreenList.Add(stockList[i]);
                    }
                    else
                    {
                        //Console.WriteLine("new IPO: " + stockList[i]);
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine(e.Source);
                Console.WriteLine(e.StackTrace);
            }
            return afterScreenList;
        }

		public List<string> GetStockTickersWithoutNewIPO(string date, string sector)
		{
			List<string> stockTickerList = new List<string>();
			try
			{
				stockTickerList = GetStockTickers(date, sector);
				if (stockTickerList.Count > 0)
				{
					stockTickerList = ScreenOutNewIPO(stockTickerList, date, daysAfterIPO);
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				Console.WriteLine(e.Source);
				Console.WriteLine(e.StackTrace);
			}
			return stockTickerList;
		}

        public string GetLastValidTradingDay(string date)
        {
            string lastTradingDay = "";
            try
            {
                List<string> lastTradingDayList = GetLastTradingDaySet(tickerCSI300, date);
                lastTradingDay = lastTradingDayList[0];
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine(e.Source);
                Console.WriteLine(e.StackTrace);
            }
            return lastTradingDay;
        }

        public List<string> GetLimitedStocksInSector(string sector, string date, bool upLmt)
        {
            List<string> limitedStocks = new List<string>();
            try
            {
                List<string> lastTradingDayList = GetLastTradingDaySet(tickerCSI300, date);
                string lastTradingDay = lastTradingDayList[0];
				List<string> stockList = GetStockTickers(lastTradingDay, sector);
                limitedStocks = GetLimitedStocks(stockList, lastTradingDay, upLmt);
                if (limitedStocks.Count > 0)
                {
					limitedStocks = ScreenOutNewIPO(limitedStocks, lastTradingDay, daysAfterIPO);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine(e.Source);
                Console.WriteLine(e.StackTrace);
            }
            return limitedStocks;
        }

        public List<string> GetLimitedStocks(List<string> stockList, string lastTradingDay, bool upLmt)
        {
            List<string> limitedStocks = new List<string>();
            try
            {
                List<double> lastPctChg = new List<double>();
                string inputStockList = "";
                foreach (string stock in stockList)
                {
                    inputStockList += stock + ",";
                }
                List<double> closeChanges = GetCloseChanges(inputStockList, lastTradingDay, lastTradingDay);
                if (closeChanges.Count != stockList.Count)
                {
                    Console.WriteLine("output close price count and input stock list doesn't match");
                    return limitedStocks;
                }
                else
                {
                    for (int i = 0; i < closeChanges.Count; i++ )
                    {
                        if (closeChanges[i] > 9.95 && upLmt)
                        {
                            limitedStocks.Add(stockList[i]);
                        }
                        else if (closeChanges[i] < -9.95 && !upLmt)
                        {
                            limitedStocks.Add(stockList[i]);
                        }
                    }
                }
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine(e.Source);
                Console.WriteLine(e.StackTrace);
            }
            return limitedStocks;
        }

        public List<string> GetSectorList(string sectorConfig)
        {
            List<string> sectorList = new List<string>();
            try
            {
                StreamReader readConfig = new StreamReader(sectorConfig);
                while (!readConfig.EndOfStream)
                {
                    sectorList.Add(readConfig.ReadLine());
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine(e.Source);
                Console.WriteLine(e.StackTrace);
            }
            return sectorList;
        }

        public List<string> GetStockTickers(string date, string sector)
        {
            List<string> stockTickers = new List<string>();

            string options = "date=" + date + ";sector=" + sector;
            WindData wsetResult = wAPI.wset("SectorConstituent", options);
            string message = wsetResult.GetErrorMsg();
            object[] wsetData = (object[])wsetResult.data;
            int counter = 0;
            foreach (object s in wsetData)
            {
                if (counter % 3 == 1)
                {
                    stockTickers.Add((string)s);
                }
                counter++;
            }

            return stockTickers;
        }

        public List<string> GetTradingDays(string ticker, string from, string to)
        {
            List<string> tradingDays = new List<string>();

            WindData wsdResult = wAPI.wsd(ticker, "volume", from, to, "Fill=Previous;PriceAdj=F");
            if (wsdResult.data is object[])
            {
                return tradingDays;
            }
            else
            {
                double[] wsdData = (double[])wsdResult.data;
                int len = wsdData.Count();
                for (int i = 0; i <= len - 1; i++)
                {
                    if (wsdData[i] > 0)
                    {
                        DateTime dt = wsdResult.timeList[i];
                        tradingDays.Add(string.Format("{0:yyyy-MM-dd}", dt));
                    }
                }
                return tradingDays;
            }
        }

        public List<string> GetLastTradingDaySet(string ticker, string day)
        {
            List<string> lastTradingDays = new List<string>();

            DateTime tillDay = Convert.ToDateTime(day);
            string[] tickers = ticker.Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries);

            for (int i = 0; i <= tickers.Count() - 1; i++)
            {
                string t = tickers[i];
                for (int m = 0; m <= 100; m++)
                {
                    DateTime dtTo = tillDay.AddMonths(-1 * m);
                    DateTime dtFrom = tillDay.AddMonths(-1 * m - 1);
                    string strTo = string.Format("{0:yyyy-MM-dd}", dtTo);
                    string strFrom = string.Format("{0:yyyy-MM-dd}", dtFrom);
                    List<string> tTradingDays = GetTradingDays(t, strFrom, strTo);
                    if (tTradingDays.Count > 0)
                    {
                        string tLastTradingDay = tTradingDays[tTradingDays.Count - 1];
                        lastTradingDays.Add(tLastTradingDay);
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            return lastTradingDays;
        }

		public List<double> GetClosePrices(List<string> tickerList, string from, string to)
		{
			string longTicker = "";
			try
			{
				foreach (string singleTicker in tickerList)
				{
					longTicker += singleTicker+",";
				}
				
			}
			catch(Exception e)
			{
				Console.WriteLine(e.Message);
				Console.WriteLine(e.Source);
				Console.WriteLine(e.StackTrace);
			}
			return GetClosePrices(longTicker, from, to);
		}

        public List<double> GetClosePrices(string ticker, string from, string to)
        {
            List<double> closePrices = new List<double>();

            WindData wsdResult = wAPI.wsd(ticker, "close", from, to, "Fill=Previous;PriceAdj=F");
            if (wsdResult.data is object[])
            {
                return closePrices;
            }
            else
            {
                double[] wsdData = (double[])wsdResult.data;
                foreach (double s in wsdData)
                {
                    closePrices.Add(Math.Round(s, 2));
                }
                return closePrices;
            }
        }

        public List<double> GetHighPrices(string ticker, string from, string to)
        {
            List<double> highPrices = new List<double>();

            WindData wsdResult = wAPI.wsd(ticker, "high", from, to, "Fill=Previous;PriceAdj=F");
            double[] wsdData = (double[])wsdResult.data;
            foreach (double s in wsdData)
            {
                highPrices.Add(Math.Round(s, 2));
            }
            return highPrices;
        }

        public List<double> GetLowPrices(string ticker, string from, string to)
        {
            List<double> lowPrices = new List<double>();

            WindData wsdResult = wAPI.wsd(ticker, "low", from, to, "Fill=Previous;PriceAdj=F");
            double[] wsdData = (double[])wsdResult.data;
            foreach (double s in wsdData)
            {
                lowPrices.Add(Math.Round(s, 2));
            }

            return lowPrices;
        }

        public List<double> GetCloseChanges(string ticker, string from, string to)
        {
            List<double> closeChanges = new List<double>();

            WindData wsdResult = wAPI.wsd(ticker, "pct_chg", from, to, "Fill=Previous;PriceAdj=F");
			if (wsdResult.data is object[])
			{
				return closeChanges;
			}
			else
			{
				double[] wsdData = (double[])wsdResult.data;
				foreach (double s in wsdData)
				{
					closeChanges.Add(s);
				}
				return closeChanges;
			}
        }

        public List<double> GetMAPrices(string ticker, string from, string to, int ma)
        {
            List<double> MAPrices = new List<double>();
            List<string> tradingDays = GetTradingDays(ticker, from, to);
            string options = "MA=" + ma + ";Fill=Previous;PriceAdj=F";
            foreach (string t in tradingDays)
            {
                WindData wsdResult = wAPI.wsd(ticker, "MA", t, t, options);
                double[] wsdData = (double[])wsdResult.data;
                MAPrices.Add(wsdData[0]);
            }
            return MAPrices;
        }

        public List<double> GetShortMAPrices(string ticker, string from, string to, int ma)
        {
            List<double> MAPrices = new List<double>();
            string options = "MA_N=" + ma + ";Fill=Previous;PriceAdj=F";
            WindData wsdResult = wAPI.wsd(ticker, "MA", from, to, options);
            double[] wsdData = (double[])wsdResult.data;
            foreach (double s in wsdData)
            {
                MAPrices.Add(s);
            }
            return MAPrices;
        }

        public List<double> GetVolumes(string ticker, string from, string to)
        {
            List<double> stockVolumes = new List<double>();
            WindData wsdResult = wAPI.wsd(ticker, "volume", from, to, "Fill=Previous;PriceAdj=F");
            double[] wsdData = (double[])wsdResult.data;
            foreach (double s in wsdData)
            {
                stockVolumes.Add(s);
            }
            return stockVolumes;
        }

        public List<double> GetVolatility(string ticker, string from, string to, int period)
        {
            List<double> stockVols = new List<double>();
            List<string> tradingDays = GetTradingDays(ticker, from, to);
            foreach (string s in tradingDays)
            {
                string begin = "ED-" + (period - 1).ToString() + "TD";
                string end = s;
                List<double> closeChanges = GetCloseChanges(ticker, begin, end);
                double var = closeChanges.Variance();
                double std = Math.Sqrt(var);
                double vol = std * Math.Sqrt(242 / period);
                stockVols.Add(vol);
            }
            return stockVols;
        }
    }
}
