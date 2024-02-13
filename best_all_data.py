from jugaad_data.nse import stock_df
import pandas as pd
import numpy as np
import sys
import threading
from datetime import date,timedelta,datetime

sym=sys.argv[1]
startdate=sys.argv[2]
enddate= sys.argv[3]

format_str = '%d/%m/%Y' # The format
startdate = datetime.strptime(startdate,format_str)
enddate = datetime.strptime(enddate,format_str)

def basic(startdate,enddate):
	startdate_basic = startdate - timedelta(days=30)
	df = stock_df(symbol=sym,from_date=startdate_basic,to_date=enddate,series="EQ")
	df = df[['DATE', 'CLOSE']]
	df = df.drop_duplicates()
	df_copy = df
	ind = df_copy.index[df['DATE']==startdate]
	while len(ind)==0:
	  startdate = startdate + timedelta(days=1)
	  ind = df_copy.index[df['DATE']==startdate]
	df= df[:ind[0]+7+1]  
	ind =[]
	df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
	df.to_csv("data_basic.csv",index=False)

def dma(startdate,enddate):
	startdate_dma = startdate- timedelta(days=200)
	df = stock_df(symbol=sym,from_date=startdate_dma,to_date=enddate,series="EQ")
	df = df[['DATE', 'CLOSE']]
	df = df.drop_duplicates()
	df_copy = df
	ind = df_copy.index[df['DATE']==startdate]
	while len(ind)==0:
	  startdate = startdate + timedelta(days=1)
	  ind = df_copy.index[df['DATE']==startdate]
	df= df[:ind[0]+49+1]  
	ind=[]
	df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
	df.to_csv("data_dma.csv",index=False)

def dma_plus(startdate,enddate):
	startdate_dma_plus = startdate - timedelta(days=50)
	df = stock_df(symbol=sym,from_date=startdate_dma_plus,to_date=enddate,series="EQ")
	df = df[['DATE', 'CLOSE']]
	df = df.drop_duplicates()
	df_copy = df
	ind = df_copy.index[df['DATE']==startdate]
	while len(ind)==0:
	  startdate = startdate + timedelta(days=1)
	  ind = df_copy.index[df['DATE']==startdate]
	df= df[:ind[0]+14+1]  
	ind=[]
	df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
	df.to_csv("data_dma++.csv",index=False)

def lr(startdate,enddate):
	startdate_lr = startdate - timedelta(days=20)
	df = stock_df(symbol=sym,from_date=startdate_lr,to_date=enddate,series="EQ")
	df=df[['DATE','CLOSE','PREV. CLOSE','OPEN','VWAP','LOW','HIGH','NO OF TRADES']]
	df = df.drop_duplicates()
	df_copy = df
	ind = df_copy.index[df['DATE']==startdate]
	while len(ind)==0:
	  startdate = startdate + timedelta(days=1)
	  ind = df_copy.index[df['DATE']==startdate]
	df= df[:ind[0]+2]  
	ind=[]
	df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
	df.to_csv("data_run.csv",index=False)

def macd(startdate,enddate):
	startdate_macd = startdate - timedelta(days=10)
	df = stock_df(symbol=sym,from_date=startdate_macd,to_date=enddate,series="EQ")
	df = df[['DATE', 'CLOSE']]
	df = df.drop_duplicates()
	df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
	df.to_csv("data_macd.csv",index=False)

def rsi(startdate,enddate):
	startdate_rsi = startdate - timedelta(days=10)
	df = stock_df(symbol=sym,from_date=startdate_rsi,to_date=enddate,series="EQ")
	df = df[['DATE', 'CLOSE']]
	df = df.drop_duplicates()
	df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
	df.to_csv("data_rsi.csv",index=False)

def adx(startdate,enddate):
	startdate_adx = startdate - timedelta(50)
	df = stock_df(symbol=sym,from_date=startdate_adx,to_date=enddate,series="EQ")
	df = df[['DATE', 'CLOSE','HIGH','LOW','PREV. CLOSE']]
	df = df.drop_duplicates()
	df_copy = df
	ind = df_copy.index[df['DATE']==startdate]
	while len(ind)==0:
	  startdate = startdate + timedelta(days=1)
	  ind = df_copy.index[df['DATE']==startdate]
	df= df[:ind[0]+14+1] 
	ind=[] 
	df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
	df.to_csv("data_adx.csv",index=False)

def lr_train(startdate,enddate):	
	startdate_train = startdate.replace(year=startdate.year-1)
	enddate_train = enddate.replace(year=enddate.year-1)
	startdate_train_new = startdate_train - timedelta(days=20)
	df = stock_df(symbol=sym,from_date=startdate_train_new,to_date=enddate_train,series="EQ")
	df=df[['DATE','CLOSE','PREV. CLOSE','OPEN','VWAP','LOW','HIGH','NO OF TRADES']]
	df = df.drop_duplicates()
	df_copy = df
	ind = df_copy.index[df['DATE']==startdate_train]
	while len(ind)==0:
	  startdate_train = startdate_train + timedelta(days=1)
	  ind = df_copy.index[df['DATE']==startdate_train]
	df= df[:ind[0]+2]  
	ind=[]
	df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
	df.to_csv("data_lr.csv",index=False)
	
t1 = threading.Thread(target=basic,args=(startdate,enddate))	
t2 = threading.Thread(target=dma,args=(startdate,enddate))	
t3 = threading.Thread(target=dma_plus,args=(startdate,enddate))	
t4 = threading.Thread(target=lr,args=(startdate,enddate))	
t5 = threading.Thread(target=macd,args=(startdate,enddate))	
t6 = threading.Thread(target=rsi,args=(startdate,enddate))	
t7 = threading.Thread(target=adx,args=(startdate,enddate))	
t8 = threading.Thread(target=lr_train,args=(startdate,enddate))	

t1.start()
t2.start()
t3.start()
t4.start()
t5.start()
t6.start()
t7.start()
t8.start()

t1.join()
t2.join()
t3.join()
t4.join()
t5.join()
t6.join()
t7.join()
t8.join()
