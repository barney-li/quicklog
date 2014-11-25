[w_wset_data,w_wset_codes,w_wset_fields,w_wset_times,w_wset_errorid,w_wset_reqid]=w.wset('IndexConstituent','date=20141104;windcode=000300.SH');
[w_wsq_data,w_wsq_codes,w_wsq_fields,w_wsq_times,w_wsq_errorid,w_wsq_reqid]=w.wsq('000001.SZ','rt_date,rt_time,rt_last,rt_latest,rt_last_vol,rt_vol,rt_high_limit,rt_low_limit,rt_pct_chg,rt_open,rt_high,rt_low',@MarketDataCallback);
