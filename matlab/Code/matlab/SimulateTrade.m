function [profit,cash_list,equalty_list,win_rate,win,lose] = SimulateTrade(price_list,order_list,initial_cash,commission_rate,stamp_tax_rate,transfer_fee_rate)
    if nargin<3
        return;
    end
    if nargin<6
        commission_rate = 6/10000;
        stamp_tax_rate = 1/1000;
        transfer_fee_rate = 6/1000;
    end
    if length(price_list)~=length(order_list)
        return;
    end
    data_len = length(price_list);
    profit=0;
    cash_list = zeros(data_len,1);
    cash_list(1) = initial_cash;
    equalty_list = zeros(data_len,1);
    equalty_list(1) = 0;
    win_rate=0;
    win=0;
    lose=0;
    shares=0;%开仓手数
    cash_in=0;
    cash_out=0;
    for i=2:1:data_len
        cash_list(i)=cash_list(i-1);
        if order_list(i)<0
            if -1*order_list(i)~=price_list(i)
                error('enter price is not compatible with market price');
            end
            shares = floor(cash_list(i)/price_list(i)/100);%计算最大开仓手数
            commission = shares*100*price_list(i)*commission_rate;%计算佣金
            cash_list(i) = cash_list(i)+order_list(i)*shares*100 - commission;%计算现金
            cash_in = cash_list(i-1) - cash_list(i);
        end
        if order_list(i)>0
            if order_list(i)~=price_list(i)
                error('exit price is not compatible with market price');
            end
            commission = shares*100*price_list(i)*commission_rate;%计算佣金
            stamp_tax = shares*100*price_list(i)*stamp_tax_rate;%计算印花税
            transfer_fee = shares*100*transfer_fee_rate;%计算过户费
            cash_list(i) = cash_list(i)+order_list(i)*shares*100 - commission - stamp_tax - transfer_fee;
            shares=0;
            cash_out = cash_list(i)-cash_list(i-1);
            if cash_out-cash_in>0
                win = win+1;
            else
                lose = lose+1;
            end
        end
        equalty_list(i) = price_list(i)*shares*100;%每天计算一次权益
    end
    win_rate = win/win+lose;
    profit = equalty_list(data_len)+cash_list(data_len)-equalty_list(1)-cash_list(1);
end