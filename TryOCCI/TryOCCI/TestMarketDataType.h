#ifndef TESTMARKETDATATYPE_ORACLE
# define TESTMARKETDATATYPE_ORACLE

#ifndef OCCI_ORACLE
# include <occi.h>
#endif

class TestMarketDataType;

/************************************************************/
//  generated declarations for the TESTMARKETDATATYPE object type.
/************************************************************/

class TestMarketDataType : public oracle::occi::PObject {

private:

   oracle::occi::Timestamp time_stamp;
   OCCI_STD_NAMESPACE::string trading_day;
   oracle::occi::Number average_price;

public:

   oracle::occi::Timestamp gettime_stamp() const;

   void settime_stamp(const oracle::occi::Timestamp &value);

   OCCI_STD_NAMESPACE::string gettrading_day() const;

   void settrading_day(const OCCI_STD_NAMESPACE::string &value);

   oracle::occi::Number getaverage_price() const;

   void setaverage_price(const oracle::occi::Number &value);

   void *operator new(size_t size);

   void *operator new(size_t size, const oracle::occi::Connection * sess,
      const OCCI_STD_NAMESPACE::string& table);

   void *operator new(size_t, void *ctxOCCI_);

   void *operator new(size_t size, const oracle::occi::Connection *sess,
      const OCCI_STD_NAMESPACE::string &tableName, 
      const OCCI_STD_NAMESPACE::string &typeName,
      const OCCI_STD_NAMESPACE::string &tableSchema, 
      const OCCI_STD_NAMESPACE::string &typeSchema);

   OCCI_STD_NAMESPACE::string getSQLTypeName() const;

   void getSQLTypeName(oracle::occi::Environment *env, void **schemaName,
      unsigned int &schemaNameLen, void **typeName,
      unsigned int &typeNameLen) const;

   TestMarketDataType();

   TestMarketDataType(void *ctxOCCI_);

   static void *readSQL(void *ctxOCCI_);

   virtual void readSQL(oracle::occi::AnyData& streamOCCI_);

   static void writeSQL(void *objOCCI_, void *ctxOCCI_);

   virtual void writeSQL(oracle::occi::AnyData& streamOCCI_);

   ~TestMarketDataType();

};

#endif
