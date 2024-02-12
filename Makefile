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
