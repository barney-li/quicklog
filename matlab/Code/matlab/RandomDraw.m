for i=1:length(data(:,2))
    if(data(i,2)<=2)
        belowVal(i)=data(i,1);
    else
        belowVal(i)=NaN;
    end
end
xaxis=1:length(data(:,1));
ax(1)=subplot(2,1,1); 
plot(xaxis,data(:,1),xaxis,belowVal,'o');
ax(2)=subplot(2,1,2); 
plot(xaxis,data(:,2));
linkaxes(ax,'x');