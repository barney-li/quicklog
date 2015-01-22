
#ifndef MARKETDATATYPEMAP_ORACLE
# include "MarketDataTypeMap.h"
#endif

void MarketDataTypeMap(oracle::occi::Environment* envOCCI_)
{
  oracle::occi::Map *mapOCCI_ = envOCCI_->getMap();
  mapOCCI_->put("C##BARNEY.MARKET_DATA_TYPE", &MarketDataType::readSQL, &MarketDataType::writeSQL);
}
