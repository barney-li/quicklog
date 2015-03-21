using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using WAPIWrapperCSharp;
using MathNet.Numerics.Statistics;
namespace ConsoleApplication1
{
    class DataFetch
    {
        // for profiling


        private WindAPI wAPI = new WindAPI();

        public DataFetch()
        {
            Connect();
        }
        public bool Connect()
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
            Console.WriteLine("ok");

            return stockTickers;
        }

        public List<string> GetTradingDays(string ticker, string from, string to)
        {
            List<string> tradingDays = new List<string>();

            WindData wsdResult = wAPI.wsd(ticker, "close", from, to, "Fill=Previous;PriceAdj=F");
            List<string> wsdData = new List<string>();
            foreach (DateTime dt in wsdResult.timeList)
            {
                wsdData.Add(string.Format("{0:yyyy-MM-dd}", dt));
                //Console.WriteLine(string.Format("{0:yyyy-MM-dd}",dt));
            }
            foreach (string s in wsdData)
            {
                tradingDays.Add(s);
            }
            return tradingDays;
        }

        public List<string> GetLastTradingDaySet(string ticker, string day)
        {
            List<string> lastTradingDays = new List<string>();
            WindData wsdResult = wAPI.wsd(ticker, "last_trade_day", day, day, "");
            object[] wsdData = (object[])wsdResult.data;
            foreach (object s in wsdData)
            {
                lastTradingDays.Add(string.Format("{0:yyyy-MM-dd}", (DateTime)s));
            }

            return lastTradingDays;
        }

        public List<double> GetClosePrices(string ticker, string from, string to)
        {
            List<double> closePrices = new List<double>();

            WindData wsdResult = wAPI.wsd(ticker, "close", from, to, "Fill=Previous;PriceAdj=F");
            double[] wsdData = (double[])wsdResult.data;
            foreach (double s in wsdData)
            {
                if (!double.IsNaN(s))
                {
                    closePrices.Add(Math.Round(s, 2));
                }
            }
            Console.WriteLine("ok");
            return closePrices;
        }

        public List<double> GetHighPrices(string ticker, string from, string to)
        {
            List<double> highPrices = new List<double>();

            WindData wsdResult = wAPI.wsd(ticker, "high", from, to, "Fill=Previous;PriceAdj=F");
            double[] wsdData = (double[])wsdResult.data;
            foreach (double s in wsdData)
            {
                if (!double.IsNaN(s))
                {
                    highPrices.Add(Math.Round(s, 2));
                }
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
                if (!double.IsNaN(s))
                {
                    lowPrices.Add(Math.Round(s, 2));
                }
            }
            return lowPrices;
        }

        public List<double> GetCloseChanges(string ticker, string from, string to)
        {
            List<double> closeChanges = new List<double>();

            WindData wsdResult = wAPI.wsd(ticker, "pct_chg", from, to, "Fill=Previous;PriceAdj=F");
            double[] wsdData = (double[])wsdResult.data;
            foreach (double s in wsdData)
            {
                closeChanges.Add(s);
            }
            return closeChanges;
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
