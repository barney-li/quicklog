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
        static void Main(string[] args)
        {
			StreamWriter areaWriter = new StreamWriter("./report/area.txt");
			StreamWriter conceptWriter = new StreamWriter("./report/concept.txt");
			StreamWriter industryWriter = new StreamWriter("./report/industry.txt");
            try
            {
                DataFetch data = new DataFetch();

                //get last trading day
                //string lastTradingDay = data.GetLastValidTradingDay(string.Format("{0:yyyy-MM-dd}", DateTime.Today));
				string lastTradingDay = "2015-03-20";
                //get sector A info first
                List<string> SectorAStocks = data.GetStockTickers(lastTradingDay, "全部A股");
				List<double> SectorAInflowRatio = data.GetInflowRatio(SectorAStocks, lastTradingDay);
				SectorInfo sectorA = new SectorInfo("全部A股", SectorAStocks.Count, SectorAStocks, SectorAStocks, SectorAInflowRatio);

                //sector info list
                List<SectorInfo> sectorInfoList = new List<SectorInfo>();

                //stocks divided by area
                List<string> areaList = data.GetSectorList("./config/area.txt");
				int upLmtInAllArea = 0;
				data.GetInflowRatio(areaList, lastTradingDay);
                foreach (string sector in areaList)
                {
                    List<string> limitedStocks = data.GetLimitedStocksInSector(sector, lastTradingDay, true);
                    List<string> sectorStocks = data.GetStockTickersWithoutNewIPO(lastTradingDay, sector);
					List<double> sectorInflowRatio = data.GetInflowRatio(sectorStocks, lastTradingDay);
					SectorInfo tempSector = new SectorInfo(sector, sectorA.SectorCount, limitedStocks, sectorStocks, sectorInflowRatio);
                    sectorInfoList.Add(tempSector);

					Console.WriteLine(sector);
                    foreach (string stock in limitedStocks)
                    {
						upLmtInAllArea++;
                        Console.WriteLine("+" + stock);
                    }
                }
				//record sector information
				areaWriter.WriteLine("板块名称,涨停个股数量,板块个股总数,涨停个股数量/板块个股总数,单一板块涨停个股/所有板块涨停个股,资金流,A股个股总数");
				foreach (SectorInfo sector in sectorInfoList)
				{
					areaWriter.WriteLine(sector.SectorName + "," + sector.SectorUpLimitCount + "," + sector.SectorCount + ","
										+ (double)sector.SectorUpLimitCount/(double)sector.SectorCount+"," 
										+ (double)sector.SectorUpLimitCount/(double)upLmtInAllArea+","
										+ sector.AverageInflowRatio + "," + sectorA.SectorCount);
				}

				//clear sector info list
				sectorInfoList = new List<SectorInfo>();
				List<string> conceptList = data.GetSectorList("./config/concept.txt");
				int upLmtInAllConcept = 0;
				foreach (string sector in conceptList)
				{
					List<string> limitedStocks = data.GetLimitedStocksInSector(sector, lastTradingDay, true);
					List<string> sectorStocks = data.GetStockTickersWithoutNewIPO(lastTradingDay, sector);
					List<double> sectorInflowRatio = data.GetInflowRatio(sectorStocks, lastTradingDay);
					SectorInfo tempSector = new SectorInfo(sector, sectorA.SectorCount, limitedStocks, sectorStocks, sectorInflowRatio);
					sectorInfoList.Add(tempSector);

					Console.WriteLine(sector);
					foreach (string stock in limitedStocks)
					{
						upLmtInAllConcept++;
						Console.WriteLine("+" + stock);
					}
				}
				//record sector information
				conceptWriter.WriteLine("板块名称,涨停个股数量,板块个股总数,涨停个股数量/板块个股总数,单一板块涨停个股/所有板块涨停个股,资金流,A股个股总数");
				foreach (SectorInfo sector in sectorInfoList)
				{
					conceptWriter.WriteLine(sector.SectorName + "," + sector.SectorUpLimitCount + "," + sector.SectorCount + ","
										+ (double)sector.SectorUpLimitCount / (double)sector.SectorCount + ","
										+ (double)sector.SectorUpLimitCount / (double)upLmtInAllArea + ","
										+ sector.AverageInflowRatio + "," + sectorA.SectorCount);
				}

				//clear sector info list
				sectorInfoList = new List<SectorInfo>();
				List<string> industryList = data.GetSectorList("./config/industry.txt");
				int upLmtInAllIndustry = 0;
				foreach (string sector in industryList)
				{
					List<string> limitedStocks = data.GetLimitedStocksInSector(sector, lastTradingDay, true);
					List<string> sectorStocks = data.GetStockTickersWithoutNewIPO(lastTradingDay, sector);
					List<double> sectorInflowRatio = data.GetInflowRatio(sectorStocks, lastTradingDay);
					SectorInfo tempSector = new SectorInfo(sector, sectorA.SectorCount, limitedStocks, sectorStocks, sectorInflowRatio);
					sectorInfoList.Add(tempSector);

					Console.WriteLine(sector);
					foreach (string stock in limitedStocks)
					{
						upLmtInAllIndustry++;
						Console.WriteLine("+" + stock);
					}
				}
				//record sector information
				industryWriter.WriteLine("板块名称,涨停个股数量,板块个股总数,涨停个股数量/板块个股总数,单一板块涨停个股/所有板块涨停个股,资金流,A股个股总数");
				foreach (SectorInfo sector in sectorInfoList)
				{
					industryWriter.WriteLine(sector.SectorName + "," + sector.SectorUpLimitCount + "," + sector.SectorCount + ","
										+ (double)sector.SectorUpLimitCount / (double)sector.SectorCount + ","
										+ (double)sector.SectorUpLimitCount / (double)upLmtInAllArea + ","
										+ sector.AverageInflowRatio + "," + sectorA.SectorCount);
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
                areaWriter.Flush();
				areaWriter.Close();
				conceptWriter.Flush();
				conceptWriter.Close();
				industryWriter.Flush();
				industryWriter.Close();
            }
            DateTime today = DateTime.Today;

            

        }
    }
}
