from jugaad_data.nse import stock_df
import pandas as pd
import numpy as np
from datetime import date,timedelta,datetime

sym=input()
startdate=input()
enddate= input()

format_str = '%d/%m/%Y' # The format
startdate = datetime.strptime(startdate,format_str)
enddate = datetime.strptime(enddate,format_str)
startdate = startdate - timedelta(days=1)
df = stock_df(symbol=sym,from_date=startdate,to_date=enddate,series="EQ")
df=df[['DATE','CLOSE','PREV. CLOSE','OPEN','VWAP','LOW','HIGH','NO OF TRADES']]
df.to_csv("data_lr.csv",index=False)
