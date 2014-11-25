clear;
%tick=1:length(close(:,1));

%[sysMid, sysUppr, sysLowr] = bollinger(close(:,1)-close(:,2), 1000, 1, 3);
%figure(1)
%plot(tick,close(:,1)-close(:,2),'green', tick, boll(:,1), 'blue', tick, boll(:,2), 'blue', tick, sysUppr, 'red', tick, sysLowr, 'red');
%sysTick=1:length(sysMid);
%figure(2)
%plot(sysTick, close(:,1)-close(:,2), sysTick, sysUppr, sysTick, sysLowr, sysTick, sysMid);
fileName='D:\BACKTESTS\DataSet4\data\Log\boll.xlsx';
sheetName='last';
data = xlsread(fileName, sheetName);
for i=1:1:length(data(:,1));
    if(abs(data(i,3)-data(i,6))<=15)
        %data(i,3) = nan;
        %data(i,6) = nan;
    end
    for j=1:1:length(data(1,:));
        if( data(i,j) == 0)
        data(i,j)=nan;
        end
    end
end
tick = 1:length(data(:,1));
figure(1);
plot(tick,data(:,1),'red', tick,data(:,2),'blue', tick,data(:,3),'red',tick,data(:,5),'green',tick,data(:,7),'blue',tick,data(:,10),'black*');
figure(2);
ax(1)=subplot(3,1,1);
plot(tick,data(:,1),'red', tick,data(:,2),'blue', tick,data(:,3),'red',tick,data(:,5),'green',tick,data(:,7),'blue',tick,data(:,10),'black*');
prim_vol(length(data(:,1)))=0;
scnd_vol(length(data(:,1)))=0;
prim_vol(1)=nan;
scnd_vol(1)=nan;
for i=2:1:length(data(:,1))
    prim_vol(i) = data(i,8)-data(i-1,8);
    if prim_vol(i)<0
        prim_vol(i) = 0;
    end
    scnd_vol(i) = data(i,9)-data(i-1,9);
    if scnd_vol(i)<0
        scnd_vol(i) = 0;
    end
end
prim_filted=tsmovavg(prim_vol, 's', 100);
scnd_filted=tsmovavg(scnd_vol, 's', 100);
ax(2)=subplot(3,1,2);
plot(tick,prim_filted,'red',tick,scnd_filted,'green');
dir_index(length(data(:,1)))=0;
for i=101:1:length(data(:,1))
    for j=i-99:1:i
        dir_index(i)=dir_index(i)+data(j,5)-data(j-1,5);
    end
end
ax(3)=subplot(3,1,3);
plot(dir_index);
linkaxes(ax,'x');
