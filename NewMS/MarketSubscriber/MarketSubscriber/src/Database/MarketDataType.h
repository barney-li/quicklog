#ifndef MARKETDATATYPE_ORACLE
# define MARKETDATATYPE_ORACLE

#ifndef OCCI_ORACLE
# include <occi.h>
#endif

class MarketDataType;

/************************************************************/
//  generated declarations for the MARKET_DATA_TYPE object type.
/************************************************************/

class MarketDataType : public oracle::occi::PObject {

private:

   oracle::occi::Number data_type_version;
   oracle::occi::Timestamp time_stamp;
   OCCI_STD_NAMESPACE::string trading_day;
   OCCI_STD_NAMESPACE::string instrument_id;
   OCCI_STD_NAMESPACE::string exchange_id;
   OCCI_STD_NAMESPACE::string exchange_instrument_id;
   oracle::occi::Number last_price;
   oracle::occi::Number pre_settlement_price;
   oracle::occi::Number pre_close_price;
   oracle::occi::Number pre_open_interest;
   oracle::occi::Number open_price;
   oracle::occi::Number highest_price;
   oracle::occi::Number lowest_price;
   oracle::occi::Number volume;
   oracle::occi::Number turnover;
   oracle::occi::Number open_interest;
   oracle::occi::Number close_price;
   oracle::occi::Number settlement_price;
   oracle::occi::Number upper_limit_price;
   oracle::occi::Number lower_limit_price;
   oracle::occi::Number pre_delta;
   oracle::occi::Number curr_delta;
   OCCI_STD_NAMESPACE::string update_time;
   oracle::occi::Number update_millisec;
   oracle::occi::Number bid_price_1;
   oracle::occi::Number bid_volume_1;
   oracle::occi::Number ask_price_1;
   oracle::occi::Number ask_volume_1;
   oracle::occi::Number bid_price_2;
   oracle::occi::Number bid_volume_2;
   oracle::occi::Number ask_price_2;
   oracle::occi::Number ask_volume_2;
   oracle::occi::Number bid_price_3;
   oracle::occi::Number bid_volume_3;
   oracle::occi::Number ask_price_3;
   oracle::occi::Number ask_volume_3;
   oracle::occi::Number bid_price_4;
   oracle::occi::Number bid_volume_4;
   oracle::occi::Number ask_price_4;
   oracle::occi::Number ask_volume_4;
   oracle::occi::Number bid_price_5;
   oracle::occi::Number bid_volume_5;
   oracle::occi::Number ask_price_5;
   oracle::occi::Number ask_volume_5;
   oracle::occi::Number average_price;
   OCCI_STD_NAMESPACE::string action_day;

public:

   oracle::occi::Number getdata_type_version() const;

   void setdata_type_version(const oracle::occi::Number &value);

   oracle::occi::Timestamp gettime_stamp() const;

   void settime_stamp(const oracle::occi::Timestamp &value);

   OCCI_STD_NAMESPACE::string gettrading_day() const;

   void settrading_day(const OCCI_STD_NAMESPACE::string &value);

   OCCI_STD_NAMESPACE::string getinstrument_id() const;

   void setinstrument_id(const OCCI_STD_NAMESPACE::string &value);

   OCCI_STD_NAMESPACE::string getexchange_id() const;

   void setexchange_id(const OCCI_STD_NAMESPACE::string &value);

   OCCI_STD_NAMESPACE::string getexchange_instrument_id() const;

   void setexchange_instrument_id(const OCCI_STD_NAMESPACE::string &value);

   oracle::occi::Number getlast_price() const;

   void setlast_price(const oracle::occi::Number &value);

   oracle::occi::Number getpre_settlement_price() const;

   void setpre_settlement_price(const oracle::occi::Number &value);

   oracle::occi::Number getpre_close_price() const;

   void setpre_close_price(const oracle::occi::Number &value);

   oracle::occi::Number getpre_open_interest() const;

   void setpre_open_interest(const oracle::occi::Number &value);

   oracle::occi::Number getopen_price() const;

   void setopen_price(const oracle::occi::Number &value);

   oracle::occi::Number gethighest_price() const;

   void sethighest_price(const oracle::occi::Number &value);

   oracle::occi::Number getlowest_price() const;

   void setlowest_price(const oracle::occi::Number &value);

   oracle::occi::Number getvolume() const;

   void setvolume(const oracle::occi::Number &value);

   oracle::occi::Number getturnover() const;

   void setturnover(const oracle::occi::Number &value);

   oracle::occi::Number getopen_interest() const;

   void setopen_interest(const oracle::occi::Number &value);

   oracle::occi::Number getclose_price() const;

   void setclose_price(const oracle::occi::Number &value);

   oracle::occi::Number getsettlement_price() const;

   void setsettlement_price(const oracle::occi::Number &value);

   oracle::occi::Number getupper_limit_price() const;

   void setupper_limit_price(const oracle::occi::Number &value);

   oracle::occi::Number getlower_limit_price() const;

   void setlower_limit_price(const oracle::occi::Number &value);

   oracle::occi::Number getpre_delta() const;

   void setpre_delta(const oracle::occi::Number &value);

   oracle::occi::Number getcurr_delta() const;

   void setcurr_delta(const oracle::occi::Number &value);

   OCCI_STD_NAMESPACE::string getupdate_time() const;

   void setupdate_time(const OCCI_STD_NAMESPACE::string &value);

   oracle::occi::Number getupdate_millisec() const;

   void setupdate_millisec(const oracle::occi::Number &value);

   oracle::occi::Number getbid_price_1() const;

   void setbid_price_1(const oracle::occi::Number &value);

   oracle::occi::Number getbid_volume_1() const;

   void setbid_volume_1(const oracle::occi::Number &value);

   oracle::occi::Number getask_price_1() const;

   void setask_price_1(const oracle::occi::Number &value);

   oracle::occi::Number getask_volume_1() const;

   void setask_volume_1(const oracle::occi::Number &value);

   oracle::occi::Number getbid_price_2() const;

   void setbid_price_2(const oracle::occi::Number &value);

   oracle::occi::Number getbid_volume_2() const;

   void setbid_volume_2(const oracle::occi::Number &value);

   oracle::occi::Number getask_price_2() const;

   void setask_price_2(const oracle::occi::Number &value);

   oracle::occi::Number getask_volume_2() const;

   void setask_volume_2(const oracle::occi::Number &value);

   oracle::occi::Number getbid_price_3() const;

   void setbid_price_3(const oracle::occi::Number &value);

   oracle::occi::Number getbid_volume_3() const;

   void setbid_volume_3(const oracle::occi::Number &value);

   oracle::occi::Number getask_price_3() const;

   void setask_price_3(const oracle::occi::Number &value);

   oracle::occi::Number getask_volume_3() const;

   void setask_volume_3(const oracle::occi::Number &value);

   oracle::occi::Number getbid_price_4() const;

   void setbid_price_4(const oracle::occi::Number &value);

   oracle::occi::Number getbid_volume_4() const;

   void setbid_volume_4(const oracle::occi::Number &value);

   oracle::occi::Number getask_price_4() const;

   void setask_price_4(const oracle::occi::Number &value);

   oracle::occi::Number getask_volume_4() const;

   void setask_volume_4(const oracle::occi::Number &value);

   oracle::occi::Number getbid_price_5() const;

   void setbid_price_5(const oracle::occi::Number &value);

   oracle::occi::Number getbid_volume_5() const;

   void setbid_volume_5(const oracle::occi::Number &value);

   oracle::occi::Number getask_price_5() const;

   void setask_price_5(const oracle::occi::Number &value);

   oracle::occi::Number getask_volume_5() const;

   void setask_volume_5(const oracle::occi::Number &value);

   oracle::occi::Number getaverage_price() const;

   void setaverage_price(const oracle::occi::Number &value);

   OCCI_STD_NAMESPACE::string getaction_day() const;

   void setaction_day(const OCCI_STD_NAMESPACE::string &value);

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

   MarketDataType();

   MarketDataType(void *ctxOCCI_);

   static void *readSQL(void *ctxOCCI_);

   virtual void readSQL(oracle::occi::AnyData& streamOCCI_);

   static void writeSQL(void *objOCCI_, void *ctxOCCI_);

   virtual void writeSQL(oracle::occi::AnyData& streamOCCI_);

   ~MarketDataType();

};

#endif
