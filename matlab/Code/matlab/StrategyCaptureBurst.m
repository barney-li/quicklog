function [rsi_matrix,order_matrix,order_count_list]=StrategyCaptureBurst(raw_data,instrument_size,trading_day_count,enter_rsi,exit_rsi,rsi_period)
    if nargin<3
        enter_rsi = 15;
        exit_rsi = 85;
        rsi_period = 6;%RSI计算周期
    end
    
    rsi_matrix = zeros(trading_day_count,instrument_size);
    order_matrix = zeros(trading_day_count,instrument_size);%记录所有合约的开仓与平仓价格，开仓价格带有负号
    enter_price = 0;%记录当前合约的开仓价格，没开仓为0
    order_count_list = zeros(instrument_size);%记录每一个合约的开仓次数
    
    for j=1:1:instrument_size
        rsi_matrix(:,j) = rsindex(close_price_matrix(:,j), rsi_period);%计算所有合约的rsi
        for i=1:1:trading_day_count
            if enter_price==0 && rsi_matrix(i,j)<enter_rsi
                enter_price = close_price_matrix(i,j);
                order_matrix(i,j) = -1*close_price_matrix(i,j);
                order_count_list(j) = order_count_list(j)+1;
            end %当天收盘价满足入场条件
            if enter_price~=0 && rsi_matrix(i,j)>exit_rsi
                order_matrix(i,j) = close_price_matrix(i,j);
                enter_price = 0;
            end
        end
    end 
end