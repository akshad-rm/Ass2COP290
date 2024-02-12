from jugaad_data.nse import stock_df
import pandas as pd
import numpy as np
import sys
from datetime import date,timedelta,datetime

sym=sys.argv[1]
startdate=sys.argv[2]
enddate= sys.argv[3]

format_str = '%d/%m/%Y' # The format
startdate = datetime.strptime(startdate,format_str)
enddate = datetime.strptime(enddate,format_str)
startdate_new = startdate - timedelta(days=20)

df = stock_df(symbol=sym,from_date=startdate_new,to_date=enddate,series="EQ")
df=df[['DATE','CLOSE','PREV. CLOSE','OPEN','VWAP','LOW','HIGH','NO OF TRADES']]

df_copy = df
ind = df_copy.index[df['DATE']==startdate]
while len(ind)==0:
  startdate = startdate + timedelta(days=1)
  ind = df_copy.index[df['DATE']==startdate]
df= df[:ind[0]+2]  
df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
df.to_csv("data_run.csv",index=False)

