function BackTest()
    clear;
    %load Sh50Index2014;
    initial_cash = 10000;
    commission_rate = 6/10000;
    stamp_tax_rate = 1/1000;
    transfer_fee_rate = 6/1000;

    start_date = '2008-01-01';
    end_date = '2009-01-01';
    [~,~,close_price_matrix,~,~] = GetSh50IndexData(start_date,end_date);
    stock_size = length(close_price_matrix(1,:));
    trading_day_count = length(close_price_matrix(:,1));
    
    data_size = length(close_price_matrix(:,1));
   
    contract_order_matrix = zeros(data_size,stock_size);%生成对照组的交易记录
    contract_order_matrix(2,:) = -1.*close_price_matrix(2,:);
    contract_order_matrix(data_size,:) = close_price_matrix(data_size,:);
    contract_cash_matrix = zeros(data_size,stock_size);
    contract_equalty_matrix = zeros(data_size,stock_size);
    for i=1:1:stock_size
        [~,contract_cash_matrix(:,i),contract_equalty_matrix(:,i),~,win,lose] = SimulateTrade(close_price_matrix(:,i),contract_order_matrix(:,i),initial_cash,commission_rate,stamp_tax_rate,transfer_fee_rate);
    end
    
    contract_cash_list = zeros(data_size);
    contract_equalty_list = zeros(data_size);
    for i=1:1:data_size
        contract_cash_list(i) = sum(contract_cash_matrix(i,:));
        contract_equalty_list(i) = sum(contract_equalty_matrix(i,:));
    end
    figure(2);
    plot(contract_cash_list+contract_equalty_list);
    
    enter_rsi=10:5:50;
    exit_rsi=70:5:90;
    rsi_period=6:1:6;
    optimize_max = length(enter_rsi)*length(exit_rsi)*length(rsi_period);
    optimize_count = 0;
    cash_list = zeros(data_size,optimize_max);
    equalty_list = zeros(data_size,optimize_max);
    result_matrix = zeros(length(enter_rsi),length(exit_rsi));
    m=0;
    for enter_rsi=10:5:50
        m=m+1;
        n=0;
        for exit_rsi=70:5:90
            n=n+1;
            for rsi_period=6:1:6
                optimize_count = optimize_count+1;
                [~,order_matrix,~] = StrategyRSI(close_price_matrix,stock_size,trading_day_count,enter_rsi,exit_rsi,rsi_period);
                cash_matrix = zeros(data_size,stock_size);
                equalty_matrix = zeros(data_size,stock_size);
                for i=1:1:stock_size
                    [~,cash_matrix(:,i),equalty_matrix(:,i),~,win,lose] = SimulateTrade(close_price_matrix(:,i),order_matrix(:,i),initial_cash,commission_rate,stamp_tax_rate,transfer_fee_rate);
                end
                
                for i=1:1:data_size
                    cash_list(i,optimize_count) = sum(cash_matrix(i,:));
                    equalty_list(i,optimize_count) = sum(equalty_matrix(i,:));
                end
                result_matrix(m,n) = cash_list(data_size,optimize_count) + equalty_list(data_size,optimize_count);
            end
        end
    end
    
    
    figure(3);
    plot(cash_list+equalty_list);
    figure(4);
    enter_rsi=10:5:50;
    exit_rsi=70:5:90;
    mesh(exit_rsi,enter_rsi,result_matrix);
    hold on;
    contract_result_matrix = result_matrix./result_matrix * (contract_cash_list(data_size)+contract_equalty_list(data_size));
    mesh(exit_rsi,enter_rsi,contract_result_matrix);
    hold off;
    alpha(0.5);
end