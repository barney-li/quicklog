#include "stdafx.h"
#ifndef TESTMARKETDATATYPE_ORACLE
# include "TestMarketDataType.h"
#endif


/*****************************************************************/
//  generated method implementations for the TESTMARKETDATATYPE object type.
/*****************************************************************/

oracle::occi::Timestamp TestMarketDataType::gettime_stamp() const
{
  return time_stamp;
}

void TestMarketDataType::settime_stamp(const oracle::occi::Timestamp &value)
{
  time_stamp = value;
}

OCCI_STD_NAMESPACE::string TestMarketDataType::gettrading_day() const
{
  return trading_day;
}

void TestMarketDataType::settrading_day(const OCCI_STD_NAMESPACE::string &value)
{
  trading_day = value;
}

oracle::occi::Number TestMarketDataType::getaverage_price() const
{
  return average_price;
}

void TestMarketDataType::setaverage_price(const oracle::occi::Number &value)
{
  average_price = value;
}

void *TestMarketDataType::operator new(size_t size)
{
  return oracle::occi::PObject::operator new(size);
}

void *TestMarketDataType::operator new(size_t size, const oracle::occi::Connection * sess,
  const OCCI_STD_NAMESPACE::string& table)
{
  return oracle::occi::PObject::operator new(size, sess, table, 
            (char *) "C##BARNEY.TESTMARKETDATATYPE");
}

void *TestMarketDataType::operator new(size_t size, void *ctxOCCI_)
{
 return oracle::occi::PObject::operator new(size, ctxOCCI_);
}

void *TestMarketDataType::operator new(size_t size,
    const oracle::occi::Connection *sess,
    const OCCI_STD_NAMESPACE::string &tableName, 
    const OCCI_STD_NAMESPACE::string &typeName,
    const OCCI_STD_NAMESPACE::string &tableSchema, 
    const OCCI_STD_NAMESPACE::string &typeSchema)
{
  return oracle::occi::PObject::operator new(size, sess, tableName,
        typeName, tableSchema, typeSchema);
}

OCCI_STD_NAMESPACE::string TestMarketDataType::getSQLTypeName() const
{
  return OCCI_STD_NAMESPACE::string("C##BARNEY.TESTMARKETDATATYPE");
}

void TestMarketDataType::getSQLTypeName(oracle::occi::Environment *env, void **schemaName,
    unsigned int &schemaNameLen, void **typeName, unsigned int &typeNameLen) const
{
  PObject::getSQLTypeName(env, &TestMarketDataType::readSQL, schemaName,
        schemaNameLen, typeName, typeNameLen);
}

TestMarketDataType::TestMarketDataType()
{
}

TestMarketDataType::TestMarketDataType(void *ctxOCCI_) : oracle::occi::PObject (ctxOCCI_)
{
}

void *TestMarketDataType::readSQL(void *ctxOCCI_)
{
  TestMarketDataType *objOCCI_ = new(ctxOCCI_) TestMarketDataType(ctxOCCI_);
  oracle::occi::AnyData streamOCCI_(ctxOCCI_);

  try
  {
    if (streamOCCI_.isNull())
      objOCCI_->setNull();
    else
      objOCCI_->readSQL(streamOCCI_);
  }
  catch (oracle::occi::SQLException& excep)
  {
    delete objOCCI_;
    excep.setErrorCtx(ctxOCCI_);
    return (void *)NULL;
  }
  return (void *)objOCCI_;
}

void TestMarketDataType::readSQL(oracle::occi::AnyData& streamOCCI_)
{
   time_stamp = streamOCCI_.getTimestamp();
   trading_day = streamOCCI_.getString();
   average_price = streamOCCI_.getNumber();
}

void TestMarketDataType::writeSQL(void *objectOCCI_, void *ctxOCCI_)
{
  TestMarketDataType *objOCCI_ = (TestMarketDataType *) objectOCCI_;
  oracle::occi::AnyData streamOCCI_(ctxOCCI_);

  try
  {
    if (objOCCI_->isNull())
      streamOCCI_.setNull();
    else
      objOCCI_->writeSQL(streamOCCI_);
  }
  catch (oracle::occi::SQLException& excep)
  {
    excep.setErrorCtx(ctxOCCI_);
  }
  return;
}

void TestMarketDataType::writeSQL(oracle::occi::AnyData& streamOCCI_)
{
   streamOCCI_.setTimestamp(time_stamp);
   streamOCCI_.setString(trading_day);
   streamOCCI_.setNumber(average_price);
}

TestMarketDataType::~TestMarketDataType()
{
  int i;
}
