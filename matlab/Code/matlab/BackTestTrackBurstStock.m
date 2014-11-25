%function BackTestTrackBurstStock
    %clear;
    %w=windmatlab;
    load 1MinData;
    initial_cash = 10000;
    commission_rate = 6/10000;
    stamp_tax_rate = 1/1000;
    transfer_fee_rate = 6/1000;
    set='IndexConstituent';%指数成分
    inst_list_code=1;
    %inst_list_name=2;
    %inst_list_weight=3;
    disp('getting stock IDs');
    %[stock_matrix,~,~,~,~,~]=w.wset(set,'date=20140101;windcode=000016.SH;field=wind_code,sec_name,i_weight');
    disp('complete');
    stock_size=length(stock_matrix(:,inst_list_code));
    fields = 'open,high,low,close,volume,pct_chg,oi';
    disp('getting last price matrix');
    %for stock_index=1:1:stock_size
    %    [w_wsi_data,w_wsi_codes,w_wsi_fields,w_wsi_times,w_wsi_errorid,w_wsi_reqid]=w.wsi(stock_matrix(stock_index,inst_list_code)',fields,'2014-10-05 09:00:00','2014-11-05 20:53:56','PriceAdj=F');
    %    raw_data(stock_index,:,:)=w_wsi_data;
    %end
    disp('complete');
    %w.close();
%end