function [ HPIValue ] = HPI( hi, lo, vol, opInt, oneCentMov, multiplyFactor )
%HPI This function calculates HPI value.
%   Detailed explanation goes here
len = length(hi);
m=zeros(len,1);
k=zeros(len,1);
k(1)=1;
m(1)=(hi(1)+lo(1))/2;
for index=2:len
    m(index) = (hi(index)+lo(index))/2;
    if(m(index)>m(index-1))
        pn=1;
    else
        pn=-1;
    end
    i=abs(opInt(index)-opInt(index-1));
    g=max(opInt(index),opInt(index-1));
    k1=(oneCentMov*vol(index)*(m(index)-m(index-1))*(1+pn*2*i/g));
    k(index)=(k(index-1)+(k1-k(index-1))*multiplyFactor);
end
HPIValue = k;
end

