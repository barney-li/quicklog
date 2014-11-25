clear;
column_primask=1;
column_primbid=2;
column_scndask=3;
column_scndbid=4;
column_delta1=5;
column_delta2=6;
column_outupper=7;
column_inupper=8;
column_mid=9;
column_inlower=10;
column_outlower=11;
column_primvol=12;
column_scndvol=13;
column_trade=14;
column_scndenter=15;
column_primenter=16;
column_scndclose=17;
column_primclose=18;
fileName='C:\Users\li\Desktop\backtest\trade-cu\data\Log\boll.xlsx';
sheetName='oppo';
data = xlsread(fileName, sheetName);
for i=1:1:length(data(:,1));
    for j=1:1:length(data(1,:));
        if( data(i,j) == 99999 || data(i,j) == 0)
        data(i,j)=nan;
        end
    end
end
tick = 1:length(data(:,1));
figure(1);
plotyy(tick,[data(:,column_primask),data(:,column_primbid),data(:,column_scndask),data(:,column_scndbid),data(:,column_scndenter)],tick,[data(:,column_delta1), data(:,column_delta2)]);
legend('primask','primbid','scndask','scndbid','scndenter','delta1','delta2');
figure(2);
ax(1) = subplot(2,1,1);
plot(tick,data(:,column_primask),'blue',tick,data(:,column_primbid),'black',tick,data(:,column_scndask),'blue',tick,data(:,column_scndbid),'black',tick,data(:,column_scndenter),'reddiamond');
ax(2) = subplot(2,1,2);
plot(tick,data(:,column_outupper),'blue', tick,data(:,column_outlower),'black', tick, data(:,column_delta1),'blue', tick,data(:,column_delta2),'black');
linkaxes(ax,'x');