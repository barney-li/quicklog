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
		const int FetchDataCalenderDay = 200;
		const int FetchDataBussinessDay = 120;
		const int CutOffBussinessDay = 120;
		const int InsertToDataInfoBussinessDay = 120;
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
				string initialDay = string.Format("{0:yyyy-MM-dd}", Convert.ToDateTime(today).AddDays(-1*FetchDataCalenderDay));
				tradingDayList = data.GetTradingDays(DataFetch.TickerCSI300, initialDay, today);
				from = tradingDayList[tradingDayList.Count-1-1*FetchDataBussinessDay];
				to = tradingDayList[tradingDayList.Count-1];
				cutOff = tradingDayList[tradingDayList.Count-1*CutOffBussinessDay];
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
			StreamWriter writer = new StreamWriter("./" + to + " " +sectorName + ".csv", false, Encoding.UTF8);
			StreamWriter timeSerialWriter = new StreamWriter("./"+to+" "+sectorName+" TimeSerialAnalyze.csv",false,Encoding.UTF8);
			List<SectorInfo> sectorInfoList = new List<SectorInfo>();
			try
			{
				//sector list
				List<string> sectorList = data.GetSectorList("./" + sectorName + ".txt");
				int upLmtInAllArea = 0;
				foreach (string sector in sectorList)
				{
					Console.WriteLine("当前板块： " + sector);
					//get stock ticker 的截止时间选择为最新一天，保证能够拿到最新板块的股票代码
					List<string> sectorStockList = data.GetStockTickers(to, sector);
					
					List<string> limitedStocks = GetStringMarketData(sectorStockList, 0, LimitedStock);
					//将板块股票代码中不在A股市场交易的代码剔除
					sectorStockList = GetStringMarketData(sectorStockList, 0, MarketAStock);
					if (sectorStockList.Count == 0)
					{
						// if this sector is empty then ignore it
						continue;
					}
					
					List<List<double>> closeMatrix = new List<List<double>>();
					for (int i = 0; i < InsertToDataInfoBussinessDay; i++)
					{
						List<double> closePrices = GetDecimalMarketData(sectorStockList, InsertToDataInfoBussinessDay-1-i, Close);
						closeMatrix.Add(closePrices);
					}

					List<List<double>> inflowRatioMatrix = new List<List<double>>();
					for (int i = 0; i < InsertToDataInfoBussinessDay; i++)
					{
						List<double> inflowRatioForOneDay = GetDecimalMarketData(sectorStockList, InsertToDataInfoBussinessDay - 1 - i, InflowRatio);
						inflowRatioMatrix.Add(inflowRatioForOneDay);
					}

					List<List<double>> freeFloatSharesMatrix = new List<List<double>>();
					for (int i = 0; i < InsertToDataInfoBussinessDay; i++)
					{
						List<double> freeFloatSharesForOneDay = GetDecimalMarketData(sectorStockList, InsertToDataInfoBussinessDay - 1 - i, FreeFloatShares);
						freeFloatSharesMatrix.Add(freeFloatSharesForOneDay);
					}

					List<List<double>> volumeMatrix = new List<List<double>>();
					for (int i = 0; i < InsertToDataInfoBussinessDay; i++)
					{
						List<double> volumeForOneDay = GetDecimalMarketData(sectorStockList, InsertToDataInfoBussinessDay - 1 - i, Volume);
						volumeMatrix.Add(volumeForOneDay);
					}

					SectorInfo tempSector = new SectorInfo(sector, tickerListOfAll.Count, limitedStocks, sectorStockList, inflowRatioMatrix, freeFloatSharesMatrix, closeMatrix, volumeMatrix);

					sectorInfoList.Add(tempSector);

					foreach (string stock in limitedStocks)
					{
						upLmtInAllArea++;
						Console.WriteLine("+" + stock);
					}

				}
				//record sector information
				writer.WriteLine("板块名称,涨停个股数量,板块个股总数,涨停个股数量/板块个股总数,单一板块涨停个股/所有板块涨停个股,算术平均资金流,加权平均资金流,加权平均资金流增幅,板块成交量增幅,板块市值5日增幅,板块市值10日增幅,5日涨幅前10的个股,A股个股总数");
				foreach (SectorInfo sector in sectorInfoList)
				{
					writer.WriteLine(sector.SectorName + "," + sector.SectorUpLimitCount + "," + sector.SectorCount + ","
										+ Math.Round(100 * (double)sector.SectorUpLimitCount / (double)sector.SectorCount, 2) + "%,"
										+ Math.Round(100 * (double)sector.SectorUpLimitCount / (double)upLmtInAllArea, 2) + "%,"
										+ Math.Round(100 * sector.AverageInflowRatio, 2) + "%,"
										+ Math.Round(100 * sector.WeightAverageInflowRatio, 2) + "%," 
										+ Math.Round(100 * sector.DeltaWeightAverageInflowRatio, 2) + "%,"
										+ Math.Round(100 * sector.GetDeltaVolume(), 2) + "%,"
										+ Math.Round(100 * sector.GetDeltaSectorValue(5,0), 2) + "%,"
										+ Math.Round(100 * sector.GetDeltaSectorValue(10, 0), 2) + "%,"
										+ sector.GetDeltaOrderStockList(5,10) + ","
										+ tickerListOfAll.Count);
				}

				
				foreach(SectorInfo sector in sectorInfoList)
				{
					string inflowRatioList = "";
					string valueList = "";
					string tradingDay = "";
					inflowRatioList += "inflow ratio,";
					valueList+="value,";
					tradingDay += "trading day,";
					for (int i = InsertToDataInfoBussinessDay - 1; i >= 0; i-- )
					{
						inflowRatioList += Math.Round(sector.GetWeightAverageInflowRatio(i),4)+",";
						valueList+=Math.Round(sector.GetSectorValue(i),4)+",";
						tradingDay+=tradingDayList[tradingDayList.Count - 1 - i] + ",";
					}
					timeSerialWriter.WriteLine(sector.SectorName);
					timeSerialWriter.WriteLine(tradingDay);
					timeSerialWriter.WriteLine(inflowRatioList);
					timeSerialWriter.WriteLine(valueList);
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
				timeSerialWriter.Flush();
				timeSerialWriter.Close();
			}
			return sectorInfoList;
		}
    }
}
