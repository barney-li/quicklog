function [rsi_matrix,order_matrix,order_count_list]=StrategyCaptureBurst(raw_data,instrument_size,trading_day_count,enter_rsi,exit_rsi,rsi_period)
    if nargin<3
        enter_rsi = 15;
        exit_rsi = 85;
        rsi_period = 6;%RSI��������
    end
    
    rsi_matrix = zeros(trading_day_count,instrument_size);
    order_matrix = zeros(trading_day_count,instrument_size);%��¼���к�Լ�Ŀ�����ƽ�ּ۸񣬿��ּ۸���и���
    enter_price = 0;%��¼��ǰ��Լ�Ŀ��ּ۸�û����Ϊ0
    order_count_list = zeros(instrument_size);%��¼ÿһ����Լ�Ŀ��ִ���
    
    for j=1:1:instrument_size
        rsi_matrix(:,j) = rsindex(close_price_matrix(:,j), rsi_period);%�������к�Լ��rsi
        for i=1:1:trading_day_count
            if enter_price==0 && rsi_matrix(i,j)<enter_rsi
                enter_price = close_price_matrix(i,j);
                order_matrix(i,j) = -1*close_price_matrix(i,j);
                order_count_list(j) = order_count_list(j)+1;
            end %�������̼������볡����
            if enter_price~=0 && rsi_matrix(i,j)>exit_rsi
                order_matrix(i,j) = close_price_matrix(i,j);
                enter_price = 0;
            end
        end
    end 
end