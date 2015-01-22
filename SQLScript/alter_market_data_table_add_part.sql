-- add partition at the end of the range
alter table market_data_table add partition part201501 values less than (to_timestamp('2015-02-01','YYYY-MM-DD')) tablespace marketdata201501 storage (initial 1M next 64M minextents 1 maxextents unlimited);
alter table market_data_table add partition part201502 values less than (to_timestamp('2015-03-01','YYYY-MM-DD')) tablespace marketdata201502 storage (initial 1M next 64M minextents 1 maxextents unlimited);
alter table market_data_table add partition part201503 values less than (to_timestamp('2015-04-01','YYYY-MM-DD')) tablespace marketdata201503 storage (initial 1M next 64M minextents 1 maxextents unlimited);
alter table market_data_table add partition part201504 values less than (to_timestamp('2015-05-01','YYYY-MM-DD')) tablespace marketdata201504 storage (initial 1M next 64M minextents 1 maxextents unlimited);
alter table market_data_table add partition part201505 values less than (to_timestamp('2015-06-01','YYYY-MM-DD')) tablespace marketdata201505 storage (initial 1M next 64M minextents 1 maxextents unlimited);
alter table market_data_table add partition part201506 values less than (to_timestamp('2015-07-01','YYYY-MM-DD')) tablespace marketdata201506 storage (initial 1M next 64M minextents 1 maxextents unlimited);
alter table market_data_table add partition part201507 values less than (to_timestamp('2015-08-01','YYYY-MM-DD')) tablespace marketdata201507 storage (initial 1M next 64M minextents 1 maxextents unlimited);
alter table market_data_table add partition part201508 values less than (to_timestamp('2015-09-01','YYYY-MM-DD')) tablespace marketdata201508 storage (initial 1M next 64M minextents 1 maxextents unlimited);
alter table market_data_table add partition part201509 values less than (to_timestamp('2015-10-01','YYYY-MM-DD')) tablespace marketdata201509 storage (initial 1M next 64M minextents 1 maxextents unlimited);
alter table market_data_table add partition part201510 values less than (to_timestamp('2015-11-01','YYYY-MM-DD')) tablespace marketdata201510 storage (initial 1M next 64M minextents 1 maxextents unlimited);
alter table market_data_table add partition part201511 values less than (to_timestamp('2015-12-01','YYYY-MM-DD')) tablespace marketdata201511 storage (initial 1M next 64M minextents 1 maxextents unlimited);
alter table market_data_table add partition part201512 values less than (to_timestamp('2016-01-01','YYYY-MM-DD')) tablespace marketdata201512 storage (initial 1M next 64M minextents 1 maxextents unlimited);
-- add partition before or in the middle of current range
-- create table space for part201312
create tablespace marketdata201312 datafile 'C:\tablespaces\2013\marketdata201312.dbf' size 1M autoextend on next 64M;
-- split partition part201401
alter table market_data_table split partition part201401 at (to_timestamp('2014-01-01','YYYY-MM-DD')) into (partition part201312,partition part201401);
-- move partition part201312 to table space marketdata201312
alter table market_data_table move partition part201312 tablespace marketdata201312 storage (initial 1M next 64M minextents 1 maxextents unlimited);

-- add partition before or in the middle of current range
create tablespace marketdata201311 datafile 'C:\tablespaces\2013\marketdata201311.dbf' size 1M autoextend on next 64M;
alter table market_data_table split partition part201312 at (to_timestamp('2013-12-01','YYYY-MM-DD')) into (partition part201311,partition part201312);
alter table market_data_table move partition part201311 tablespace marketdata201311 storage (initial 1M next 64M minextents 1 maxextents unlimited);
