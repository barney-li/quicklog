bidspread = marketdata(:,4)-marketdata(:,1);
askspread = marketdata(:,3)-marketdata(:,2);
avg1501 = 0.5*(marketdata(:,3)+marketdata(:,4));
avg1409 = 0.5*(marketdata(:,1)+marketdata(:,2));
avgSpread = 0.5*(bidspread+askspread);
logAvg1501 = log(avg1501);
logAvg1409 = log(avg1409);
logAvgSpread = log(avgSpread);
difLogAvg1501 = [0;diff(logAvg1501)];
difLogAvg1409 = [0;diff(logAvg1409)];
difAvgSpread = [0;diff(avgSpread)];

% p = 120;
% len = length(bidspread);
% cuscore(1:len,1) = 0;
% for i = p:len
%     periodMean = mean(avgSpread(i-p+1:i));
%     for j = 1:p
%        cuscore(i) = cuscore(i)+(avgSpread(i-p+j)-periodMean)*j;
%     end    
% end
% plotyy(1:len,avgSpread,1:len,cuscore);

len = length(bidspread);
% plot(1:len,difAvgSpread);
plotyy(1:len,avgSpread,1:len,difAvgSpread);



