from jugaad_data.nse import stock_df
import pandas as pd
import numpy as np
from datetime import date,timedelta,datetime

sym=input()
n=int(input())
startdate=input()
enddate= input()

format_str = '%d/%m/%Y' # The format
startdate = datetime.datetime.strptime(startdate,format_str)
enddate = datetime.datetime.strptime(enddate,format_str)
startdate_new = startdate.replace(year=startdate.year-3)

df = stock_df(symbol=sym,from_date=startdate_new,to_date=enddate,series="EQ")
df = df[['DATE', 'CLOSE']]

df_copy = df
ind = df_copy.index[df['DATE']==startdate]
while len(ind)==0:
  startdate = startdate + timedelta(days=1)
  ind = df_copy.index[df['DATE']==startdate]
df= df[:ind[0]+n+1]  

df.to_csv("data.csv",index=False)