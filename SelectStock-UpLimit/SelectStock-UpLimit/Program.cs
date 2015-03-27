using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MarketDataUtilities;
using System.IO;
using System.Collections;
namespace SelectStock_UpLimit
{
    class Program
    {
		static DataFetch data = new DataFetch();
		static List<string> tradingDayList = new List<string>();
		static Hashtable map = new Hashtable();
		static List<string> tickerListOfAll = new List<string>();
		static List<List<double>> closeMatrixOfAll = new List<List<double>>();
		static List<List<double>> freeFloatSharesMatrixOfAll = new List<List<double>>();
		static List<List<double>> closeChangeMatrixOfAll = new List<List<double>>();
		static List<List<string>> nameMatrixOfAll = new List<List<string>>();
		static List<List<double>> inflowRatioMatrixOfAll = new List<List<double>>();
		static List<List<double>> volumeMatrixOfAll = new List<List<double>>();
		const string LimitedStock = "LimitedStock";
		const string MarketAStock = "MarketAStock";
		const string Close = "Close";
		const string FreeFloatShares = "FreeFloatShares";
		const string CloseChangeMatrix = "CloseChangeMatrix";
		const string InflowRatio = "InflowRatio";
		const string Name = "Name";
		const string Volume = "Volume";
        static void Main(string[] args)
        {
            try
            {
				
                //get last trading day
				Console.WriteLine("请输入计算日期，格式为YYYY-MM-DD，计算当天数据请直接按回车键");
				string processDate = Console.ReadLine();
				if (processDate == "")
				{
					processDate = string.Format("{0:yyyy-MM-dd}",DateTime.Today);
				}
				string fromDate = "";
				string toDate = "";
				string cutOffDate = "";
				GetDate(processDate, out fromDate, out toDate, out cutOffDate);

				//get trading day list
				Console.WriteLine(toDate);

				tickerListOfAll = data.GetStockTickers(cutOffDate, "全部A股");
				closeMatrixOfAll = data.GetDecimalMatrix(tickerListOfAll, DataFetch.ClosePriceType, fromDate, toDate);
				freeFloatSharesMatrixOfAll = data.GetDecimalMatrix(tickerListOfAll, DataFetch.FreeFloatSharesType, fromDate, toDate);
				closeChangeMatrixOfAll = data.GetDecimalMatrix(tickerListOfAll, DataFetch.CloseChangeType, fromDate, toDate);
				inflowRatioMatrixOfAll = data.GetDecimalMatrix(tickerListOfAll, DataFetch.InflowRatioType, fromDate, toDate);
				nameMatrixOfAll = data.GetStringMatrix(tickerListOfAll, "sec_name", cutOffDate, cutOffDate);
				volumeMatrixOfAll = data.GetDecimalMatrix(tickerListOfAll, DataFetch.VolumeType, fromDate, toDate);

				int index = 0;
				foreach (string ticker in tickerListOfAll)
				{
					map.Add(ticker, index);
					index++;
				}
				Console.WriteLine("data fetch complete");
				GetSectorInfoMatrix("area", fromDate, toDate, cutOffDate);
				GetSectorInfoMatrix("concept", fromDate, toDate, cutOffDate);
				GetSectorInfoMatrix("industry", fromDate, toDate, cutOffDate);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine(e.Source);
                Console.WriteLine(e.StackTrace);
            }
            finally
            {

            }
        }
		static void GetDate(string today, out string from, out string to, out string cutOff)
		{
			try
			{
				string initialDay = string.Format("{0:yyyy-MM-dd}", Convert.ToDateTime(today).AddDays(-60));
				tradingDayList = data.GetTradingDays(DataFetch.TickerCSI300, initialDay, today);
				from = tradingDayList[tradingDayList.Count-11];
				to = tradingDayList[tradingDayList.Count-1];
				cutOff = tradingDayList[tradingDayList.Count-30];
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				Console.WriteLine(e.Source);
				Console.WriteLine(e.StackTrace);
				from = "";
				to = "";
				cutOff = "";
			}
		}
		static List<string> GetUpLimitStock(List<string> stockInSector, int backdate)
		{
			List<string> tickers = new List<string>();
			int dateIndex = tradingDayList.Count - 1 - backdate;
			foreach (string stock in stockInSector)
			{
				int stockIndex = (int)map[stock];
				if (closeChangeMatrixOfAll[stockIndex][dateIndex] > 9.95)
				{
					tickers.Add(stock);
				}
			}
			return tickers;
		}
		static List<double> GetDecimalMarketData(List<string> tickerList, int backdate, string dataType)
		{
			List<double> dataList = new List<double>();
			int dateIndex = closeChangeMatrixOfAll[0].Count - 1 - backdate;
			switch (dataType)
			{
				case FreeFloatShares:
					foreach (string stock in tickerList)
					{
						// 如果这个板块中的股票不在A股市场则略过
						int stockIndex = 0;
						if (map.ContainsKey(stock))
						{
							stockIndex = (int)map[stock];
						}
						dataList.Add(freeFloatSharesMatrixOfAll[stockIndex][dateIndex]);
					}
					break;
				case InflowRatio:
					foreach (string stock in tickerList)
					{
						// 如果这个板块中的股票不在A股市场则略过
						int stockIndex = 0;
						if (map.ContainsKey(stock))
						{
							stockIndex = (int)map[stock];
						}
						dataList.Add(inflowRatioMatrixOfAll[stockIndex][dateIndex]);
					}
					break;
				case Close:
					foreach (string stock in tickerList)
					{
						// 如果这个板块中的股票不在A股市场则略过
						int stockIndex = 0;
						if (map.ContainsKey(stock))
						{
							stockIndex = (int)map[stock];
						}
						dataList.Add(closeMatrixOfAll[stockIndex][dateIndex]);
					}
					break;
				case Volume:
					foreach (string stock in tickerList)
					{
						// 如果这个板块中的股票不在A股市场则略过
						int stockIndex = 0;
						if (map.ContainsKey(stock))
						{
							stockIndex = (int)map[stock];
						}
						dataList.Add(volumeMatrixOfAll[stockIndex][dateIndex]);
					}
					break;
			}
			return dataList;
		}
		static List<string> GetStringMarketData(List<string> tickerList, int backdate, string dataType)
		{
			List<string> dataList = new List<string>();
			int dateIndex = closeChangeMatrixOfAll[0].Count - 1 - backdate;
			switch (dataType)
			{
				case LimitedStock:
					foreach (string stock in tickerList)
					{
						int stockIndex = 0;
						try
						{
							// 如果这个板块中的股票不在A股市场则略过
							if (map.ContainsKey(stock))
							{
								stockIndex = (int)map[stock];
								if (closeChangeMatrixOfAll[stockIndex][dateIndex] > 9.95)
								{
									dataList.Add(stock);
								}
							}
						}
						catch (Exception e)
						{
							Console.WriteLine(e.Message);
						}
					}
					break;
				case MarketAStock:
					foreach (string stock in tickerList)
					{
						try
						{
							// 如果这个板块中的股票不在A股市场则略过
							if (map.ContainsKey(stock))
							{
								dataList.Add(stock);
							}
						}
						catch (Exception e)
						{
							Console.WriteLine(e.Message);
						}
					}
					break;
			}
			return dataList;
		}
		static List<SectorInfo> GetSectorInfoMatrix(string sectorName, string from, string to, string cutOff)
		{
			int daysInterval = 6;
			StreamWriter writer = new StreamWriter("./" + sectorName + ".csv", false, Encoding.UTF8);
			List<SectorInfo> sectorInfoList = new List<SectorInfo>();
			try
			{
				//sector list
				List<string> sectorList = data.GetSectorList("./" + sectorName + ".txt");
				int upLmtInAllArea = 0;
				foreach (string sector in sectorList)
				{
					Console.WriteLine("当前板块： " + sector);
					List<string> sectorStockList = data.GetStockTickers(cutOff, sector);
					

					List<string> limitedStocks = GetStringMarketData(sectorStockList, 0, LimitedStock);
					sectorStockList = GetStringMarketData(sectorStockList, 0, MarketAStock);
					if (sectorStockList.Count == 0)
					{
						// if this sector is empty then ignore it
						continue;
					}
					
					List<List<double>> closeMatrix = new List<List<double>>();
					for (int i = 0; i < daysInterval; i++)
					{
						List<double> closePrices = GetDecimalMarketData(sectorStockList, daysInterval-1-i, Close);
						closeMatrix.Add(closePrices);
					}

					List<List<double>> inflowRatioMatrix = new List<List<double>>();
					for (int i = 0; i < daysInterval; i++)
					{
						List<double> inflowRatioForOneDay = GetDecimalMarketData(sectorStockList, daysInterval - 1 - i, InflowRatio);
						inflowRatioMatrix.Add(inflowRatioForOneDay);
					}

					List<List<double>> freeFloatSharesMatrix = new List<List<double>>();
					for (int i = 0; i < daysInterval; i++)
					{
						List<double> freeFloatSharesForOneDay = GetDecimalMarketData(sectorStockList, daysInterval - 1 - i, FreeFloatShares);
						freeFloatSharesMatrix.Add(freeFloatSharesForOneDay);
					}

					List<List<double>> volumeMatrix = new List<List<double>>();
					for (int i = 0; i < daysInterval; i++)
					{
						List<double> volumeForOneDay = GetDecimalMarketData(sectorStockList, daysInterval - 1 - i, Volume);
						volumeMatrix.Add(volumeForOneDay);
					}

					SectorInfo tempSector = new SectorInfo(sector, tickerListOfAll.Count, limitedStocks, sectorStockList, inflowRatioMatrix, freeFloatSharesMatrix, closeMatrix, volumeMatrix);

					sectorInfoList.Add(tempSector);

					foreach (string stock in limitedStocks)
					{
						upLmtInAllArea++;
						Console.WriteLine("+" + stock);
					}
					// test only
					//Console.WriteLine(sectorInfoList[0].GetDeltaVolume());
				}
				//record sector information
				writer.WriteLine("板块名称,涨停个股数量,板块个股总数,涨停个股数量/板块个股总数,单一板块涨停个股/所有板块涨停个股,算术平均资金流,加权平均资金流,加权平均资金流增幅,板块成交量增幅,A股个股总数");
				foreach (SectorInfo sector in sectorInfoList)
				{
					writer.WriteLine(sector.SectorName + "," + sector.SectorUpLimitCount + "," + sector.SectorCount + ","
										+ Math.Round(100 * (double)sector.SectorUpLimitCount / (double)sector.SectorCount, 2) + "%,"
										+ Math.Round(100 * (double)sector.SectorUpLimitCount / (double)upLmtInAllArea, 2) + "%,"
										+ Math.Round(100 * sector.AverageInflowRatio, 2) + "%,"
										+ Math.Round(100 * sector.WeightAverageInflowRatio, 2) + "%," 
										+ Math.Round(100 * sector.DeltaWeightAverageInflowRatio, 2) + "%,"
										+ Math.Round(100 * sector.GetDeltaVolume(), 2) + "%," 
										+ tickerListOfAll.Count);
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				Console.WriteLine(e.Source);
				Console.WriteLine(e.StackTrace);
			}
			finally
			{
				writer.Flush();
				writer.Close();

			}
			return sectorInfoList;
		}
    }
}
