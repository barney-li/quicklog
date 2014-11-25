function [stock_matrix,stock_size,close_price_matrix,time_list,trading_day_count] = GetSh50IndexData(start_date,end_date)
    if nargin<2
        start_date='2014-01-02';
        end_date='2014-10-01';
    end
    w=windmatlab;
    inst_list_code=1;
    %inst_list_name=2;
    %inst_list_weight=3;
    set='IndexConstituent';%指数成分
    disp('getting stock IDs');
    [stock_matrix,~,~,~,~,~]=w.wset(set,'date=20140101;windcode=000016.SH;field=wind_code,sec_name,i_weight');
    disp('complete');
    stock_size=length(stock_matrix(:,inst_list_code));
    fields = 'close';
    %[trading_day_count,~,~,~,~,~] = w.tdayscount(start_date, end_date, 'Days=Trading');
    disp('getting last price matrix');
    [close_price_matrix,~,~,time_list,~,~]=w.wsd(stock_matrix(:,inst_list_code),fields,start_date,end_date,'PriceAdj=F');
    disp('complete');
    trading_day_count = length(close_price_matrix(:,1));
    w.close();
end