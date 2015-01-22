--create table testtable1 of market_data_type 
--partition by range(time_stamp)
--(
--partition part201301 values less than(to_timestamp('2013-02-01','YYYY-MM-DD')) tablespace testmarketdata201301,
--partition part201302 values less than(to_timestamp('2013-03-01','YYYY-MM-DD')) tablespace testmarketdata201302
--)
select t.*, t.rowid from AG1506 t where time_stamp between to_timestamp('2014-12-26 09:00:00', 'YYYY-MM-DD HH24:MI:SS') and to_timestamp('2014-12-26 09:30:00', 'YYYY-MM-DD HH24:MI:SS')
