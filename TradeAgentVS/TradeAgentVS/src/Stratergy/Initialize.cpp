#include "PrimeryAndSecondary.h"

InitErrorType PrimeryAndSecondary::InitOtherCrap()
{
	initStatus = ALL_GOOD;
	primDataBuf.resize(STRATEGY_BUFFER_SIZE);
	scndDataBuf.resize(STRATEGY_BUFFER_SIZE);
	primBufIndex = 0;
	scndBufIndex = 0;
	memset(&stgArg, 0, sizeof(stgArg));
	mPrimTodayLongPosition = 0;
	mPrimYdLongPosition = 0;
	mScndTodayLongPosition = 0;
	mScndYdLongPosition = 0;
	mPrimTodayShortPosition = 0;
	mPrimYdShortPosition = 0;
	mScndTodayShortPosition = 0;
	mScndYdShortPosition = 0;
	mCancelPrimCD = true;
	mCancelScndCD = true;
	mClosePrimCD = true;
	mCloseScndCD = true;
	mQueryCD = true;
	mOpenPrimId = 0;
	mOpenScndId = 0;
	mLastState = IDLE_STATE;
#ifdef BACK_TEST
	mCloseScndOnly = false;
#endif
	// read strategy arguments from configuration file
	if(config.ReadString(stgArg.primaryInst, "PrimaryInstrument") !=0 )
	{
		cout<<"[ERROR]: Can't find symble \"PrimaryInstrument\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"PrimaryInstrument\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.secondaryInst, "SecondaryInstrument") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"SecondaryInstrument\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"SecondaryInstrument\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.tradePeriod1Start, "TradePeriod1Start") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"TradePeriod1Start\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"TradePeriod1Start\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.tradePeriod1End, "TradePeriod1End") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"TradePeriod1End\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"TradePeriod1End\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.tradePeriod2Start, "TradePeriod2Start") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"TradePeriod2Start\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"TradePeriod2Start\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.tradePeriod2End, "TradePeriod2End") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"TradePeriod2End\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"TradePeriod2End\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.tradePeriod3Start, "TradePeriod3Start") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"TradePeriod3Start\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"TradePeriod3Start\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.tradePeriod3End, "TradePeriod3End") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"TradePeriod3End\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"TradePeriod3End\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.tradePeriod4Start, "TradePeriod4Start") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"TradePeriod4Start\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"TradePeriod4Start\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.tradePeriod4End, "TradePeriod4End") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"TradePeriod4End\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"TradePeriod4End\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.dataPeriod1Start, "DataPeriod1Start") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"DataPeriod1Start\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"DataPeriod1Start\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.dataPeriod1End, "DataPeriod1End") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"DataPeriod1End\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"DataPeriod1End\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.dataPeriod2Start, "DataPeriod2Start") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"DataPeriod2Start\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"DataPeriod2Start\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.dataPeriod2End, "DataPeriod2End") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"DataPeriod2End\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"DataPeriod2End\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.dataPeriod3Start, "DataPeriod3Start") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"DataPeriod3Start\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"DataPeriod3Start\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.dataPeriod3End, "DataPeriod3End") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"DataPeriod3End\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"DataPeriod3End\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.dataPeriod4Start, "DataPeriod4Start") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"DataPeriod4Start\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"DataPeriod4Start\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.dataPeriod4End, "DataPeriod4End") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"DataPeriod4End\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"DataPeriod4End\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadString(stgArg.stopOpenBeforeEnd, "StopOpenBeforeEnd") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"StopOpenBeforeEnd\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"StopOpenBeforeEnd\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadInteger(stgArg.bollPeriod, "BollPeriod") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"BollPeriod\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"BollPeriod\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadDouble(stgArg.outterBollAmp, "OutterBollAmp") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"OutterBollAmp\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"OutterBollAmp\" in config file");
		initStatus = CONFIG_ERROR;
	}
	else
	{
		stgArg.outterBollAmp = stgArg.outterBollAmp/100;
	}
	if(config.ReadDouble(stgArg.innerBollAmp, "InnerBollAmp") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"InnerBollAmp\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"InnerBollAmp\" in config file");
		initStatus = CONFIG_ERROR;
	}
	else
	{
		stgArg.innerBollAmp = stgArg.innerBollAmp/100;
	}
	if(config.ReadDouble(stgArg.stopBollAmp, "StopBollAmp") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"StopBollAmp\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"StopBollAmp\" in config file");
		initStatus = CONFIG_ERROR;
	}
	else
	{
		stgArg.stopBollAmp = stgArg.stopBollAmp/100;
	}
	if(config.ReadDouble(stgArg.winBollAmp, "WinBollAmp") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"WinBollAmp\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"WinBollAmp\" in config file");
		initStatus = CONFIG_ERROR;
	}
	else
	{
		stgArg.winBollAmp = stgArg.winBollAmp/100;
	}
	if(config.ReadInteger(stgArg.bollAmpLimit, "BollAmpLimit") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"BollAmpLimit\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"BollAmpLimit\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadDouble(stgArg.stopLossPrice, "StopLossPrice") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"StopLossPrice\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"StopLossPrice\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadInteger(stgArg.openShares, "OpenShares") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"OpenShares\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"OpenShares\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadInteger(stgArg.primOpenTime, "PrimOpenTime") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"PrimOpenTime\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"PrimOpenTime\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadInteger(stgArg.scndOpenTime, "ScndOpenTime") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"ScndOpenTime\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"ScndOpenTime\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadInteger(stgArg.primCloseTime, "PrimCloseTime") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"PrimCloseTime\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"PrimCloseTime\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadInteger(stgArg.scndCloseTime, "ScndCloseTime") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"ScndCloseTime\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"ScndCloseTime\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadInteger(stgArg.primCancelTime, "PrimCancelTime") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"PrimCancelTime\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"PrimCancelTime\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadInteger(stgArg.scndCancelTime, "ScndCancelTime") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"ScndCancelTime\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"ScndCancelTime\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadInteger(stgArg.scndOrderJump, "ScndOrderJump") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"ScndOrderJump\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"ScndOrderJump\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadDouble(stgArg.floatToleration, "FloatToleration") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"FloatToleration\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"FloatToleration\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadDouble(stgArg.ceilingPrice, "CeilingPrice") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"CeilingPrice\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"CeilingPrice\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadDouble(stgArg.floorPrice, "FloorPrice") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"FloorPrice\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"FloorPrice\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadDouble(stgArg.minMove, "MinMove") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"MinMove\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"MinMove\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadDouble(stgArg.askBidGapLimit, "AskBidGapLimit") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"AskBidGapLimit\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"AskBidGapLimit\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadDouble(stgArg.cost, "Cost") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"Cost\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"Cost\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadDouble(stgArg.stopWinPoint, "StopWinPoint") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"StopWinPoint\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"StopWinPoint\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadDouble(stgArg.durationStep, "DurationStep") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"DurationStep\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"DurationStep\" in config file");
		initStatus = CONFIG_ERROR;
	}
	if(config.ReadDouble(stgArg.winBollAmpAdjust, "WinBollAmpAdjust") != 0)
	{
		cout<<"[ERROR]: Can't find symble \"WinBollAmpAdjust\" in config file"<<endl;
		logger.LogThisFast("[ERROR]: Can't find symble \"WinBollAmpAdjust\" in config file");
		initStatus = CONFIG_ERROR;
	}
	else
	{
		stgArg.winBollAmpAdjust = stgArg.winBollAmpAdjust/100;
	}
	stgArg.askBidGapLimit = stgArg.askBidGapLimit*stgArg.minMove+stgArg.floatToleration;//这里一定要在min move配置读取之后
	stgArg.stopWinPoint = stgArg.stopWinPoint*stgArg.minMove+stgArg.floatToleration;
	stgArg.cost = stgArg.cost*stgArg.minMove;
	stgArg.stopLossPrice = stgArg.stopLossPrice*stgArg.minMove-stgArg.floatToleration;
	stgArg.bollAmpLimit = stgArg.bollAmpLimit*stgArg.minMove + stgArg.floatToleration;
	if(ALL_GOOD == initStatus)
	{
		cout<<"all arguments ready"<<endl;
		mPeriodicCheckPositionThread = new boost::thread(boost::bind(&PrimeryAndSecondary::PeriodicCheckPosition, this));
		mTradePeriod1 = new time_period(time_from_string("2000-01-01 "+stgArg.tradePeriod1Start), time_from_string("2000-01-01 "+stgArg.tradePeriod1End));
		mTradePeriod2 = new time_period(time_from_string("2000-01-01 "+stgArg.tradePeriod2Start), time_from_string("2000-01-01 "+stgArg.tradePeriod2End));
		mTradePeriod3 = new time_period(time_from_string("2000-01-01 "+stgArg.tradePeriod3Start), time_from_string("2000-01-01 "+stgArg.tradePeriod3End));
		mTradePeriod4 = new time_period(time_from_string("2000-01-01 "+stgArg.tradePeriod4Start), time_from_string("2000-01-01 "+stgArg.tradePeriod4End));
		mDataPeriod1 = new time_period(time_from_string("2000-01-01 "+stgArg.dataPeriod1Start), time_from_string("2000-01-01 "+stgArg.dataPeriod1End));
		mDataPeriod2 = new time_period(time_from_string("2000-01-01 "+stgArg.dataPeriod2Start), time_from_string("2000-01-01 "+stgArg.dataPeriod2End));
		mDataPeriod3 = new time_period(time_from_string("2000-01-01 "+stgArg.dataPeriod3Start), time_from_string("2000-01-01 "+stgArg.dataPeriod3End));
		mDataPeriod4 = new time_period(time_from_string("2000-01-01 "+stgArg.dataPeriod4Start), time_from_string("2000-01-01 "+stgArg.dataPeriod4End));
		
		time_duration lStopOpenBeforeEnd = time_from_string("2000-01-01 " + stgArg.stopOpenBeforeEnd) - time_from_string("2000-01-01 00:00:00");

		mOpenPeriod1 = new time_period(time_from_string("2000-01-01 "+stgArg.tradePeriod1Start), time_from_string("2000-01-01 "+stgArg.tradePeriod1End)-lStopOpenBeforeEnd);
		mOpenPeriod2 = new time_period(time_from_string("2000-01-01 "+stgArg.tradePeriod2Start), time_from_string("2000-01-01 "+stgArg.tradePeriod2End)-lStopOpenBeforeEnd);
		mOpenPeriod3 = new time_period(time_from_string("2000-01-01 "+stgArg.tradePeriod3Start), time_from_string("2000-01-01 "+stgArg.tradePeriod3End));
		mOpenPeriod4 = new time_period(time_from_string("2000-01-01 "+stgArg.tradePeriod4Start), time_from_string("2000-01-01 "+stgArg.tradePeriod4End)-lStopOpenBeforeEnd);
	}
	else
	{
		cout<<"invalid argument(s), strategy will not be loaded"<<endl;
	}
	return initStatus;

}
void PrimeryAndSecondary::InitTradeProcess()
{
	char* tempBroker = new char[20];
	char* tempInvestor = new char[20];
	char* tempPassword = new char[20];
	config.ReadList(&tempBroker, "BrokerID", ";");
	strcpy(basicTradeProcessData.brokerId, tempBroker);
	config.ReadList(&tempInvestor, "InvestorID", ";");
	strcpy(basicTradeProcessData.investorId, tempInvestor);
	config.ReadList(&tempPassword, "Password", ";");
	strcpy(basicTradeProcessData.investorPassword, tempPassword);
	basicTradeProcessData.numFrontAddress = config.ReadTradeFrontAddr(basicTradeProcessData.frontAddress);
	InitializeProcess();
	while(InitializeFinished() != true)
	{
		boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
	}
	delete tempBroker;
	delete tempInvestor;
	delete tempPassword;
}
void PrimeryAndSecondary::InitMarketProcess()
{
	char *tempConfig = new char[20];
	config.ReadList(&tempConfig, "BrokerID", ";");
	strcpy(marketObj.broker, tempConfig);
	config.ReadList(&tempConfig, "InvestorID", ";");
	strcpy(marketObj.investor, tempConfig);
	config.ReadList(&tempConfig, "Password", ";");
	strcpy(marketObj.pwd, tempConfig);
	marketObj.numFrontAddress = config.ReadMarketFrontAddr(marketObj.frontAddress);
	marketObj.numInstrument = config.ReadInstrumentID(marketObj.instrumentList);
	// only register this callback when initStatus == ALL_GOOD
	if(ALL_GOOD == initStatus)
	{
		marketObj.SetHook(this); //Register this obj for market data call back
	}
	marketObj.StartMarketProcess();
	delete tempConfig;
}