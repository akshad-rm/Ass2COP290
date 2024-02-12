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


startdate_basic = startdate - timedelta(days=30)
df = stock_df(symbol=sym,from_date=startdate_basic,to_date=enddate,series="EQ")
df = df[['DATE', 'CLOSE']]
df_copy = df
ind = df_copy.index[df['DATE']==startdate]
while len(ind)==0:
  startdate = startdate + timedelta(days=1)
  ind = df_copy.index[df['DATE']==startdate]
df= df[:ind[0]+7+1]  
ind =[]
df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
df.to_csv("data_basic.csv",index=False)

startdate_dma = startdate- timedelta(days=200)
df = stock_df(symbol=sym,from_date=startdate_dma,to_date=enddate,series="EQ")
df = df[['DATE', 'CLOSE']]
df_copy = df
ind = df_copy.index[df['DATE']==startdate]
while len(ind)==0:
  startdate = startdate + timedelta(days=1)
  ind = df_copy.index[df['DATE']==startdate]
df= df[:ind[0]+49+1]  
ind=[]
df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
df.to_csv("data_dma.csv",index=False)

startdate_dma_plus = startdate - timedelta(days=50)
df = stock_df(symbol=sym,from_date=startdate_dma_plus,to_date=enddate,series="EQ")
df = df[['DATE', 'CLOSE']]
df_copy = df
ind = df_copy.index[df['DATE']==startdate]
while len(ind)==0:
  startdate = startdate + timedelta(days=1)
  ind = df_copy.index[df['DATE']==startdate]
df= df[:ind[0]+14+1]  
ind=[]
df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
df.to_csv("data_dma++.csv",index=False)

startdate_lr = startdate - timedelta(days=20)
df = stock_df(symbol=sym,from_date=startdate_lr,to_date=enddate,series="EQ")
df=df[['DATE','CLOSE','PREV. CLOSE','OPEN','VWAP','LOW','HIGH','NO OF TRADES']]
df_copy = df
ind = df_copy.index[df['DATE']==startdate]
while len(ind)==0:
  startdate = startdate + timedelta(days=1)
  ind = df_copy.index[df['DATE']==startdate]
df= df[:ind[0]+2]  
ind=[]
df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
df.to_csv("data_run.csv",index=False)

startdate_train = startdate.replace(year=startdate.year-1)
enddate_train = enddate.replace(year=enddate.year-1)
startdate_train_new = startdate_train - timedelta(days=20)
df = stock_df(symbol=sym,from_date=startdate_train_new,to_date=enddate_train,series="EQ")
df=df[['DATE','CLOSE','PREV. CLOSE','OPEN','VWAP','LOW','HIGH','NO OF TRADES']]
df_copy = df
ind = df_copy.index[df['DATE']==startdate_train]
while len(ind)==0:
  startdate_train = startdate_train + timedelta(days=1)
  ind = df_copy.index[df['DATE']==startdate_train]
df= df[:ind[0]+2]  
ind=[]
df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
df.to_csv("data_lr.csv",index=False)
