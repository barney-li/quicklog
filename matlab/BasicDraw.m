function BasicDraw(fileName, sheetName, range, scale)
%range = '20000';
%fileName = 'D:\Futures\Data\LongTermData\rb1312.xlsx';
%sheetName = '1hour';
indexWidth = 5;
factor1 = 5;%for HPI
factor2 = 100;%for HPI
dataRange = 'a2:a';
timeRange = 'b2:b';
volumeRange = 'h2:h';
opePriceRange = 'c2:c';
highestPriceRange = 'd2:d';
lowestPriceRange = 'e2:e';
closePriceRange = 'f2:f';
openInterestRange = 'i2:i';
data = xlsread(fileName, sheetName, strcat(dataRange,range));
time = xlsread(fileName, sheetName, strcat(timeRange,range));
volume = xlsread(fileName, sheetName, strcat(volumeRange,range));
openPrice = xlsread(fileName, sheetName, strcat(opePriceRange,range));
highestPrice = xlsread(fileName, sheetName, strcat(highestPriceRange,range));
lowestPrice = xlsread(fileName, sheetName, strcat(lowestPriceRange,range));
closePrice = xlsread(fileName, sheetName, strcat(closePriceRange,range));
openInterest = xlsread(fileName, sheetName, strcat(openInterestRange,range));
[closePriceMacd, signalLine] = macd(closePrice);
xAxis=1:length(openPrice);
if strcmp(scale, 'large') == true
    ax(1) = subplot(2,2,1);
    candle(highestPrice, lowestPrice, closePrice, openPrice);
    title('Candle');
    grid ON;
    ax(2) = subplot(2,2,3);
    bar(xAxis, closePriceMacd - signalLine);
    title('MACD');
    grid ON;
    ax(3) = subplot(2,2,2);
    plot(xAxis, volume, 'blue');
    title('Volume');
    grid ON;
else
    ax(1) = subplot(2,1,1);
    candle(highestPrice, lowestPrice, closePrice, openPrice);
    title('Candle');
    grid ON;
    ax(2) = subplot(2,1,2);
%     kdjK(1:length(openPrice)) = NaN;
%     for i=1:length(openPrice)
%         if(i>indexWidth)
%             Ln = min(lowestPrice(i-indexWidth:i));
%             Hn = max(highestPrice(i-indexWidth:i));
%             kdjK(i) = (closePrice(i) - Ln)/(Hn-Ln);
%         end
%     end
%     kdjD = tsmovavg(kdjK, 's', 3);
%     plot(xAxis, kdjK, xAxis, kdjD, '--');
%     title('KDJ');
    hpiValue=HPI(highestPrice, lowestPrice, volume, openInterest, 0.01, 1);
    plot(xAxis, hpiValue, '-');
    title('HPI');
    grid ON;
    
end
linkaxes(ax,'x');

