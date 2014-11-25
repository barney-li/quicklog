stock_size = raw_data(:,1,1);
pos_open = 1;
pos_high = 2;
pos_low = 3;
pos_close = 4;
pos_volume = 5;
pos_pct_chg = 6;
pos_oi = 7;
for i=11:10:11
    open = raw_data(i,:,pos_open)';
    high = raw_data(i,:,pos_high)';
    low = raw_data(i,:,pos_low)';
    close = raw_data(i,:,pos_close)';
    pct_chg = raw_data(i,:,pos_pct_chg);
    data(:,1) = high;
    data(:,2) = low;
    data(:,3) = close;
    data(:,4) = open;
    data(:,5) = pct_chg;
    series_data = fints(w_wsi_times, data);
    date = datestr(series_data.dates, 'yyyy-mm-dd');
    time = datestr(series_data.times, 'HH:MM:SS');
    last_step1_date='';
    last_step2_date='';
    %for j=1:1;length(open)
    %    if strcmp(date(j),last_step1_date) == false
    %        if pct_chg>
    %    end
    %end
end
figure(1)
plot(pct_chg);
%candle(array_high, array_low, array_close, array_open);