function MarketDataCallback(reqid,isfinished,errorid,datas,codes,fields,times,selfdata)
str = 'market data callback'
      if(length(codes)==0|| length(fields)==0||length(times)==0)
      else
          if(length(times)==1)
              datas = reshape(datas,length(fields),length(codes))';
          elseif (length(codes)==1)
              datas = reshape(datas,length(fields),length(times))';
          elseif(length(fields)==1)
              datas = reshape(datas,length(codes),length(times));
%               else
%                   data = mapdata{5};
          end
      end

    codestr = '';
    for( i=1:length(codes))
        codestr=strcat(codestr,codes{i},',');
    end
    
    fieldstr='';
    for i=1:length(fields)
        fieldstr=strcat(fieldstr,fields{i},',');
    end
    fprintf('%s\n%s\n',codestr,fieldstr);
    disp(datas);
    fprintf('reqid = %d\n\n',reqid);
    %disp(codes);
    %disp(fields);
end