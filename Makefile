run:
ifeq ($(strategy), LINEAR_REGRESSION)
	python3 train_data.py $(symbol) $(train_start_date) $(train_end_date)
	python3 run_data.py $(symbol) $(start_date) $(end_date)
	g++ linear_regression.cpp
	./a.out $(symbol) $(p) $(x)
endif
ifeq ($(strategy), BASIC)
	python3 data_basic.py $(symbol) $(n) $(start_date) $(end_date)
	g++ basic.cpp
	./a.out $(symbol) $(n) $(x)
endif
ifeq ($(strategy),DMA)
	python3 data_dma.py $(symbol) $(n) $(start_date) $(end_date)
	g++ dma.cpp
	./a.out $(symbol) $(n) $(x) $(p)
endif
ifeq ($(strategy),DMA++)
	python3 data_dma++.py $(symbol) $(n) $(start_date) $(end_date)
	g++ dma++.cpp
	./a.out $(symbol) $(n) $(x) $(p) $(max_hold_days) $(c1) $(c2)
endif
ifeq ($(strategy),MACD)
	python3 data_basic.py $(symbol) 10 $(start_date) $(end_date)
	g++ MACD.cpp
	./a.out $(symbol) $(x) $(start_date) $(end_date)
endif
ifeq ($(strategy),RSI)
	python3 data_basic.py $(symbol) $(n) $(start_date) $(end_date)
	g++ RSI.cpp
	./a.out $(symbol) $(x) $(n) $(oversold_threshold) $(overbought_threshold) $(start_date) $(end_date)
endif
ifeq ($(strategy),ADX)
	python3 data_basic_for_ADX.py $(symbol) $(n) $(start_date) $(end_date)
	g++ ADX.cpp
	./a.out $(symbol) $(x) $(n) $(adx_threshold) $(start_date) $(end_date)
endif

ifeq ($(strategy),PAIRS)
ifdef stop_loss_threshold
	python3 data_basic_for_PAIRS.py $(symbol1) $(symbol2)  $(n) $(start_date) $(end_date)
	g++ p2.cpp
	./a.out $(symbol1) $(symbol2) $(x) $(n) $(threshold) $(stop_loss_threshold) $(start_date) $(end_date)
else 	
	python3 data_basic_for_PAIRS.py $(symbol1) $(symbol2)  $(n) $(start_date) $(end_date)
	g++ PAIRS.cpp
	./a.out $(symbol1) $(symbol2) $(x) $(n) $(threshold) $(start_date) $(end_date)
endif
	
endif

