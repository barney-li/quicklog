#include "stdafx.h"
#ifndef TESTMARKETDATATYPEMAP_ORACLE
# include "TestMarketDataTypeMap.h"
#endif

void TestMarketDataTypeMap(oracle::occi::Environment* envOCCI_)
{
  oracle::occi::Map *mapOCCI_ = envOCCI_->getMap();
  mapOCCI_->put("C##BARNEY.TESTMARKETDATATYPE", &TestMarketDataType::readSQL, &TestMarketDataType::writeSQL);
}
