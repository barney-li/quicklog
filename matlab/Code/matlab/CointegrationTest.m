fileName='D:\BACKTESTS\cointegration\data\Log\data.xlsx';
sheetName='Sheet1';
data = xlsread(fileName, sheetName);
tick = 1:length(data(:,1));
%figure(1);
%plot(tick,data(:,1),'red', tick,data(:,2),'blue', tick,data(:,3),'red',tick,data(:,6),'blue',tick,data(:,9),'black*');
