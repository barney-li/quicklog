using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MarketDataUtilities;
using System.IO;
namespace SelectStock_UpLimit
{
    class Program
    {
		static DataFetch data = new DataFetch();
		static SectorInfo sectorA = new SectorInfo();
        static void Main(string[] args)
        {
            try
            {
				
                //get last trading day
				Console.WriteLine("请输入计算日期，格式为YYYY-MM-DD，计算当天数据请直接按回车键");
				string toDate = Console.ReadLine();
				if (toDate == "")
				{
					toDate = string.Format("{0:yyyy-MM-dd}",DateTime.Today);
				}
				string fromDate = string.Format("{0:yyyy-MM-dd}", Convert.ToDateTime(toDate).AddDays(-30));
				List<string> dateList = data.GetTradingDays(DataFetch.tickerCSI300, fromDate, toDate);
				//get trading day list
				string lastTradingDay = dateList[dateList.Count-1];
				Console.WriteLine(lastTradingDay);			
	
				// test super fetch
				List<string> tickerList = new List<string>();
				tickerList.Add("600000.sh");
				tickerList.Add("600004.sh");
				tickerList.Add("600030.sh");
				tickerList.Add("600567.sh");
				List<List<string>> freeFloatSharesMatrix = data.GetStringMatrix(tickerList, "sec_name", fromDate, toDate);
				List<List<double>> closePriceMatrix = data.GetDecimalMatrix(tickerList, "close", fromDate, toDate);


                //get sector A info first
                List<string> sectorAStocks = data.GetStockTickers(lastTradingDay, "全部A股");
				List<double> sectorAInflowRatio = data.GetInflowRatio(sectorAStocks, lastTradingDay);
				List<double> sectorAFreeFloatShares = data.GetFreeFloatShares(sectorAStocks, lastTradingDay);
				List<double> sectorAClosePrices = data.GetClosePrices(sectorAStocks, lastTradingDay, lastTradingDay);
				sectorA = new SectorInfo("全部A股", sectorAStocks.Count, sectorAStocks, sectorAStocks, sectorAInflowRatio, sectorAFreeFloatShares, sectorAClosePrices);

				GetSectorInfo("test", lastTradingDay);
				//GetSectorInfo("area", lastTradingDay);
				//GetSectorInfo("concept", lastTradingDay);
				//GetSectorInfo("industry", lastTradingDay);
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

		static List<SectorInfo> GetSectorInfo(string sectorName, string lastTradingDay)
		{
			StreamWriter writer = new StreamWriter("./"+sectorName+".csv", false, Encoding.UTF8);
			List<SectorInfo> sectorInfoList = new List<SectorInfo>();
			try
			{
				//sector info list
				List<string> sectorList = data.GetSectorList("./" + sectorName + ".txt");
				int upLmtInAllArea = 0;
				foreach (string sector in sectorList)
				{
					Console.WriteLine("当前板块： " + sector);
					List<string> limitedStocks = data.GetLimitedStocksInSector(sector, lastTradingDay, true);
					List<string> sectorStocks = data.GetStockTickersWithoutNewIPO(lastTradingDay, sector);
					List<double> freeFloatShares = data.GetFreeFloatShares(sectorStocks, lastTradingDay);
					List<double> closePrices = data.GetClosePrices(sectorStocks, lastTradingDay, lastTradingDay);
					List<double> sectorInflowRatio = data.GetInflowRatio(sectorStocks, lastTradingDay);
					SectorInfo tempSector = new SectorInfo(sector, sectorA.SectorCount, limitedStocks, sectorStocks, sectorInflowRatio, freeFloatShares, closePrices);
					sectorInfoList.Add(tempSector);

					foreach (string stock in limitedStocks)
					{
						upLmtInAllArea++;
						Console.WriteLine("+" + stock);
					}
				}
				//record sector information
				writer.WriteLine("板块名称,涨停个股数量,板块个股总数,涨停个股数量/板块个股总数,单一板块涨停个股/所有板块涨停个股,算术平均资金流,加权平均资金流,A股个股总数");
				foreach (SectorInfo sector in sectorInfoList)
				{
					writer.WriteLine(sector.SectorName + "," + sector.SectorUpLimitCount + "," + sector.SectorCount + ","
										+ Math.Round(100 * (double)sector.SectorUpLimitCount / (double)sector.SectorCount, 2) + "%,"
										+ Math.Round(100 * (double)sector.SectorUpLimitCount / (double)upLmtInAllArea, 2) + "%,"
										+ Math.Round(100 * sector.AverageInflowRatio, 2) + "%,"
										+ Math.Round(100 * sector.WeightAverageInflowRatio, 2) + "%," + sectorA.SectorCount);
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
