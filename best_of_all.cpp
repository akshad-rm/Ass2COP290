#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <map>
#include <cmath>
#define db double

using namespace std;


void get_data_basic(vector<pair<string,db>>&data){
    ifstream datafile("data_basic.csv");
    string line ;
    
    if(!datafile.is_open()){
        cerr << "could not open the file : data_basic.csv" << endl;
    }
    vector<vector<string>>raw_data;
    int title_flag = 0;
    
    while(getline(datafile, line)) {
        if(title_flag==1){
            istringstream ss(line);
            string column;
            vector<string> row_data;

            while (std::getline(ss, column, ',')) {
                row_data.push_back(column);
            }

            raw_data.push_back(row_data);
        }
        else{
            title_flag = 1;
        }
        
    }
    datafile.close();
    
    
    
    int row_count = int(raw_data.size());
    for(int i = 0;i<row_count;i++){
        string date = raw_data[i][0];
        db close_price = stod(raw_data[i][1]);
        data.push_back({date,close_price});
    }
    reverse(data.begin(),data.end());
    
    
}


db solve_basic(int n,int x,vector<pair<string,db>>&data,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats){
    
    int rise_fall_count = 0;
    int hold_quantity = 0;
    int row_count = int(data.size());
    db cash_in_hand= 0;
    for(int i = 1;i<row_count;i++){
        if(data[i].second>data[i-1].second){
            if(rise_fall_count>0){
                rise_fall_count++;
            }
            else{
                rise_fall_count = 1;
            }
        }
        else if(data[i].second<data[i-1].second){
            if(rise_fall_count<0){
                rise_fall_count--;
            }
            else{
                rise_fall_count = -1;
            }
        }
        else{
            rise_fall_count = 0;
        }
        
        
        if(rise_fall_count>=n && hold_quantity<x){
            cash_in_hand-=data[i].second;
            daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
            order_stats.push_back({data[i].first,"BUY","1",to_string(data[i].second)});
            hold_quantity++;
        }
        
        else if(rise_fall_count<=(-1*n) && hold_quantity>(-1*x)){
            cash_in_hand+=data[i].second;
            daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
            order_stats.push_back({data[i].first,"SELL","1",to_string(data[i].second)});
            hold_quantity--;
        }
        else if(i>n-1){
            daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
        }
        
        
        
        
        
    }
    if(hold_quantity>0){
        cash_in_hand += data[row_count-1].second*hold_quantity;
        
        
        hold_quantity = 0;
        
    }
    else if(hold_quantity<0){
        cash_in_hand -= ((data[row_count-1].second)*abs(hold_quantity));
        
        
        hold_quantity = 0;
    }
    return cash_in_hand;
    
    
}

void get_data_dma(vector<pair<string,db>>&data){
    ifstream datafile("data_dma.csv");
    string line ;
    
    if(!datafile.is_open()){
        cerr << "could not open the file : data_dma.csv" << endl;
    }
    vector<vector<string>>raw_data;
    int title_flag = 0;
    
    while(getline(datafile, line)) {
        if(title_flag==1){
            istringstream ss(line);
            string column;
            vector<string> row_data;

            while (std::getline(ss, column, ',')) {
                row_data.push_back(column);
            }

            raw_data.push_back(row_data);
        }
        else{
            title_flag = 1;
        }
        
    }
    datafile.close();
    
    
    
    int row_count = int(raw_data.size());
    for(int i = 0;i<row_count;i++){
        string date = raw_data[i][0];
        db close_price = stod(raw_data[i][1]);
        data.push_back({date,close_price});
    }
    reverse(data.begin(),data.end());
    
    
}


db solve_dma(int n,int x,int p,vector<pair<string,db>>&data,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats){
    
    int hold_quantity = 0;
    int row_count = int(data.size());
    db cash_in_hand= 0;
    db dma  = 0;
    db sqmean = 0;
    db sd = 0;
    for(int i=0;i<n;i++){
        dma += data[i].second;
    }
    dma = dma/n;
    for(int i=0;i<n;i++){
        sqmean += (data[i].second)*(data[i].second);
    }
    sqmean = sqmean/n;
    for(int i = n-1;i<row_count;i++){
        if(i==n-1){
            sd = sqrt(sqmean - (dma*dma));
        }
        else{
            dma -= (data[i-n].second)/n;
            dma += (data[i].second)/n;
            sqmean -= (data[i-n].second)*(data[i-n].second)/n;
            sqmean += (data[i].second)*(data[i].second)/n;
            sd = sqrt(sqmean-(dma*dma));
        }
        if(data[i].second-dma>= p*sd && hold_quantity<x){
            cash_in_hand-=data[i].second;
            daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
            order_stats.push_back({data[i].first,"BUY","1",to_string(data[i].second)});
            hold_quantity++;
        }
        
        else if(dma-data[i].second>=p*sd && hold_quantity>(-1*x)){
            cash_in_hand+=data[i].second;
            daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
            order_stats.push_back({data[i].first,"SELL","1",to_string(data[i].second)});
            hold_quantity--;
        }
        else{
            daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
        }
        
        
        
        
        
    }
    if(hold_quantity>0){
        cash_in_hand += data[row_count-1].second*hold_quantity;
        hold_quantity = 0;
        
    }
    else if(hold_quantity<0){
        cash_in_hand -= ((data[row_count-1].second)*abs(hold_quantity));
        hold_quantity = 0;
    }
    return cash_in_hand;
    
    
    
}

void get_data_dma_plus(vector<pair<string,db>>&data){
    ifstream datafile("data_dma++.csv");
    string line ;
    
    if(!datafile.is_open()){
        cerr << "could not open the file : data_dma++.csv" << endl;
    }
    vector<vector<string>>raw_data;
    int title_flag = 0;
    
    while(getline(datafile, line)) {
        if(title_flag==1){
            istringstream ss(line);
            string column;
            vector<string> row_data;

            while (std::getline(ss, column, ',')) {
                row_data.push_back(column);
            }

            raw_data.push_back(row_data);
        }
        else{
            title_flag = 1;
        }
        
    }
    datafile.close();
    
    
    
    int row_count = int(raw_data.size());
    for(int i = 0;i<row_count;i++){
        string date = raw_data[i][0];
        db close_price = stod(raw_data[i][1]);
        data.push_back({date,close_price});
    }
    reverse(data.begin(),data.end());
    
    
}


db solve_dma_plus(int n,int x,int p,db c1,db c2,int max_hold_days,vector<pair<string,db>>&data,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats){
    
    int hold_quantity = 0;
    int row_count = int(data.size());
    db cash_in_hand= 0;
    db er_num  = 0;
    db er_den = 0;
    db eratio =0;
    db sf = 0.5;
    db ama = 0;
    vector<pair<int,int>> trades = {}; 
    for(int i=1;i<n+1;i++){
        er_den += abs(data[i].second-data[i-1].second);
    }
    for(int i = n;i<row_count;i++){
    	
    	bool den_zero=false;
        if(i==n){
           	er_num = abs(data[i].second - data[i-n].second);
           	if(er_den!=0){
           		eratio = er_num/er_den;
           	}
           	else{
           		den_zero =true;
           	}
           	ama  = data[i].second;
        }
        else{
            er_den -= abs(data[i-n].second-data[i-n-1].second);
            er_den += abs(data[i].second-data[i-1].second);
            er_num = abs(data[i].second - data[i-n].second);
            if(er_den!=0){
            	eratio = er_num/er_den;
            	db temp = (2*eratio)/(1+c2);
            	sf = ((1-c1)*sf) + (c1*((temp-1)/(temp+1)));
            	ama  = ((1-sf)*ama) + (sf*data[i].second); 
            }	
            else{
            	den_zero = true;
            }
        }
        //cout<<data[i].first<<" "<<data[i].second<<" "<<eratio<<" "<<sf<<" "<<ama<<endl;
        if((100*(data[i].second))>= (100+p)*ama && hold_quantity<x && den_zero==false){
            if(trades.size()>0 && i-trades.back().first>=max_hold_days){
                if(trades.back().second==1){
                    trades.pop_back();
                    daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
                    trades.insert(trades.begin(),pair<int,int>(i,1));
                }
                else{
                    cash_in_hand-=data[i].second;
                    daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
                    order_stats.push_back({data[i].first,"BUY","1",to_string(data[i].second)});
                    hold_quantity++;
                    trades.pop_back();
                    //trades.insert(trades.begin(),{i,1});
                }
            }
            else{
                int j = trades.size()-1;
                while(trades.size()>0 && trades[j].second==1 && j>=0){
                    j-=1;
                }
                if(j==-1){
                    cash_in_hand-=data[i].second;
                    daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
                    order_stats.push_back({data[i].first,"BUY","1",to_string(data[i].second)});
                    hold_quantity++;
                    trades.insert(trades.begin(),pair<int,int>(i,1));
                }
                else{
                    cash_in_hand-=data[i].second;
                    daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
                    order_stats.push_back({data[i].first,"BUY","1",to_string(data[i].second)});
                    hold_quantity++;
                    trades.erase(trades.begin()+j);
                    //trades.insert(trades.begin(),{i,1});
                }
            }    
        }
        
        else if((100*(data[i].second))<= (100-p)*ama && hold_quantity>(-1*x) && den_zero==false){
            if(trades.size()>0 && i-trades.back().first>=max_hold_days){
                if(trades.back().second==-1){
                    trades.pop_back();
                    daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
                    trades.insert(trades.begin(),pair<int,int>(i,-1));
                }
                else{
                    cash_in_hand+=data[i].second;
                    daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
                    order_stats.push_back({data[i].first,"SELL","1",to_string(data[i].second)});
                    hold_quantity--;
                    trades.pop_back();
                    //trades.insert(trades.begin(),{i,-1});
                }
            }
            else{
                int j = trades.size()-1;
                while(trades.size()>0 && trades[j].second==-1 && j>=0){
                    j-=1;
                }
                if(j==-1){
                    cash_in_hand+=data[i].second;
                    daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
                    order_stats.push_back({data[i].first,"SELL","1",to_string(data[i].second)});
                    hold_quantity--;
                    trades.insert(trades.begin(),pair<int,int>(i,-1));
                }
                else{
                    cash_in_hand+=data[i].second;
                    daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
                    order_stats.push_back({data[i].first,"SELL","1",to_string(data[i].second)});
                    hold_quantity--;
                    trades.erase(trades.begin()+j);
                    //trades.insert(trades.begin(),{i,-1});
                }
            }    
        }
        else{
            if(trades.size()>0 && i-trades.back().first>=max_hold_days){
                if(trades.back().second==1 && hold_quantity>(-1*x)){
                    cash_in_hand+=data[i].second;
                    daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
                    order_stats.push_back({data[i].first,"SELL","1",to_string(data[i].second)});
                    hold_quantity--;
                    trades.pop_back();
                    //trades.insert(trades.begin(),{i,-1});
                }
                else if(trades.back().second==-1 && hold_quantity<x){
                    cash_in_hand-=data[i].second;
                    daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
                    order_stats.push_back({data[i].first,"BUY","1",to_string(data[i].second)});
                    hold_quantity++;
                    trades.pop_back();
                    //trades.insert(trades.begin(),{i,1});
                }
                else{
                    daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
                }
            }
            else{
                daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
            }
        }
        
        
        
        
        
    }
    if(hold_quantity>0){
        cash_in_hand += data[row_count-1].second*hold_quantity;
        hold_quantity = 0;
        
    }
    else if(hold_quantity<0){
        cash_in_hand -= ((data[row_count-1].second)*abs(hold_quantity));
        hold_quantity = 0;
    }
    return cash_in_hand;
    
    
}

void get_training_data(vector<pair<string,vector<db>>>&data){
    ifstream datafile("data_lr.csv");
    string line ;
    
    if(!datafile.is_open()){
        cerr << "could not open the file : data_lr.csv" << endl;
    }
    vector<vector<string>>raw_data;
    int title_flag = 0;
    
    while(getline(datafile, line)) {
        if(title_flag==1){
            istringstream ss(line);
            string column;
            vector<string> row_data;

            while (std::getline(ss, column, ',')) {
                row_data.push_back(column);
            }

            raw_data.push_back(row_data);
        }
        else{
            title_flag = 1;
        }
        
    }
    datafile.close();
    
    
    
    int row_count = int(raw_data.size());
    for(int i = 0;i<row_count;i++){
        string date = raw_data[i][0];
        vector<db> cols ;
        for(int j=1;j<raw_data[0].size();j++){
        	db item = stod(raw_data[i][j]);
        	cols.push_back(item);
        }
        data.push_back({date,cols});
    }
    reverse(data.begin(),data.end());
    
    
}

void get_data_lr(vector<pair<string,vector<db>>>&data){
    ifstream datafile("data_run.csv");
    string line ;
    
    if(!datafile.is_open()){
        cerr << "could not open the file : data_run.csv" << endl;
    }
    vector<vector<string>>raw_data;
    int title_flag = 0;
    
    while(getline(datafile, line)) {
        if(title_flag==1){
            istringstream ss(line);
            string column;
            vector<string> row_data;

            while (std::getline(ss, column, ',')) {
                row_data.push_back(column);
            }

            raw_data.push_back(row_data);
        }
        else{
            title_flag = 1;
        }
        
    }
    datafile.close();
    
    
    
    int row_count = int(raw_data.size());
    for(int i = 0;i<row_count;i++){
        string date = raw_data[i][0];
        vector<db> cols ;
        for(int j=1;j<raw_data[0].size();j++){
        	db item = stod(raw_data[i][j]);
        	cols.push_back(item);
        }
        data.push_back({date,cols});
    }
    reverse(data.begin(),data.end());
    
}

vector<db> gaussJordan(const vector<vector<db>>& coefficients, const vector<db>& constants) {
    int n = coefficients.size();
    vector<vector<db>> augmented(n, vector<db>(n + 1));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            augmented[i][j] = coefficients[i][j];
        }
        augmented[i][n] = constants[i];
    }
    
    for (int i = 0; i < n; ++i) {
        int pivot_row = i;
        for (int j = i + 1; j < n; ++j) {
            if (abs(augmented[j][i]) > abs(augmented[pivot_row][i])) {
                pivot_row = j;
            }
        }
        swap(augmented[i], augmented[pivot_row]);

        db divisor = augmented[i][i];
        for (int j = i; j <= n; ++j) {
            augmented[i][j] /= divisor;
        }

        for (int j = 0; j < n; ++j) {
            if (j != i) {
                db factor = augmented[j][i];
                for (int k = i; k <= n; ++k) {
                    augmented[j][k] -= factor * augmented[i][k];
                }
            }
        }
    }

    vector<db> solution(n);
    for (int i = 0; i < n; ++i) {
        solution[i] = augmented[i][n];
    }

    return solution;
}

vector<db> get_betas(vector<pair<string,vector<db>>> &data){
	vector<db> sums(7,0); //sum of clos-1,open-1,VWAP-1,low-1,high-1,trades-1,clos
	vector<db> two_sums(21,0); 
	vector<db> sq_sums(7,0);//sq sum of clos-1,open-1,VWAP-1,low-1,high-1,trades-1,clos
	vector<db> const_sums(7,0);//sums with clos
	db close_sum = 0; //sum of y
	for(int i=1;i<data.size();i++){
		close_sum += data[i].second[0];
		sums[0]+= data[i].second[1];
		sums[1]+=data[i-1].second[2];
		sums[2]+=data[i-1].second[3];
		sums[3]+=data[i-1].second[4];
		sums[4]+=data[i-1].second[5];
		sums[5]+=data[i-1].second[6];
		sums[6]+=data[i].second[2];
		sq_sums[0]+= data[i].second[1]*data[i].second[1];
		sq_sums[1]+=data[i-1].second[2]*data[i-1].second[2];
		sq_sums[2]+=data[i-1].second[3]*data[i-1].second[3];
		sq_sums[3]+=data[i-1].second[4]*data[i-1].second[4];
		sq_sums[4]+=data[i-1].second[5]*data[i-1].second[5];
		sq_sums[5]+=data[i-1].second[6]*data[i-1].second[6];
		sq_sums[6]+=data[i].second[2]*data[i].second[2];
		//01,02,03,04,05,06,12,13,14,15,16,23,24,25,26,34,35,36,45,46,56
		two_sums[0] += data[i].second[1]*data[i-1].second[2];
		two_sums[1] += data[i].second[1]*data[i-1].second[3];
		two_sums[2] += data[i].second[1]*data[i-1].second[4];
		two_sums[3] += data[i].second[1]*data[i-1].second[5];
		two_sums[4] += data[i].second[1]*data[i-1].second[6];
		two_sums[5] += data[i].second[1]*data[i].second[2];
		two_sums[6] += data[i-1].second[2]*data[i-1].second[3];
		two_sums[7] += data[i-1].second[2]*data[i-1].second[4];
		two_sums[8] += data[i-1].second[2]*data[i-1].second[5];
		two_sums[9] += data[i-1].second[2]*data[i-1].second[6];
		two_sums[10] += data[i-1].second[2]*data[i].second[2];
		two_sums[11] += data[i-1].second[3]*data[i-1].second[4];
		two_sums[12] += data[i-1].second[3]*data[i-1].second[5];
		two_sums[13] += data[i-1].second[3]*data[i-1].second[6];
		two_sums[14] += data[i-1].second[3]*data[i].second[2];
		two_sums[15] += data[i-1].second[4]*data[i-1].second[5];
		two_sums[16] += data[i-1].second[4]*data[i-1].second[6];
		two_sums[17] += data[i-1].second[4]*data[i].second[2];
		two_sums[18] += data[i-1].second[5]*data[i-1].second[6];
		two_sums[19] += data[i-1].second[5]*data[i].second[2];
		two_sums[20] += data[i-1].second[6]*data[i].second[2];
		const_sums[0]+= data[i].second[1]*data[i].second[0];
		const_sums[1]+=data[i-1].second[2]*data[i].second[0];
		const_sums[2]+=data[i-1].second[3]*data[i].second[0];
		const_sums[3]+=data[i-1].second[4]*data[i].second[0];
		const_sums[4]+=data[i-1].second[5]*data[i].second[0];
		const_sums[5]+=data[i-1].second[6]*data[i].second[0];
		const_sums[6]+=data[i].second[2]*data[i].second[0];
	}
	vector<vector<db>> coefficients = {
	{db(data.size()-1),sums[0],sums[1],sums[2],sums[3],sums[4],sums[5],sums[6]},
	{sums[0],sq_sums[0],two_sums[0],two_sums[1],two_sums[2],two_sums[3],two_sums[4],two_sums[5]},
	{sums[1],two_sums[0],sq_sums[1],two_sums[6],two_sums[7],two_sums[8],two_sums[9],two_sums[10]},
	{sums[2],two_sums[1],two_sums[6],sq_sums[2],two_sums[11],two_sums[12],two_sums[13],two_sums[14]},
	{sums[3],two_sums[2],two_sums[7],two_sums[11],sq_sums[3],two_sums[15],two_sums[16],two_sums[17]},
	{sums[4],two_sums[3],two_sums[8],two_sums[12],two_sums[15],sq_sums[4],two_sums[18],two_sums[19]},
	{sums[5],two_sums[4],two_sums[9],two_sums[13],two_sums[16],two_sums[18],sq_sums[5],two_sums[20]},
	{sums[6],two_sums[5],two_sums[10],two_sums[14],two_sums[17],two_sums[19],two_sums[20],sq_sums[6]}
	};
	
	vector<db> constants = {close_sum,const_sums[0],const_sums[1],const_sums[2],const_sums[3],const_sums[4],const_sums[5],const_sums[6]};
	
	return gaussJordan(coefficients,constants);
	
}

db solve_lr(int p,int x,vector<pair<string,vector<db>>>&data,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats,vector<db> &betas){
    
    int hold_quantity = 0;
    int row_count = int(data.size());
    db cash_in_hand= 0;
    for(int i = 1;i<row_count;i++){
        db prediction = betas[0] + (betas[1]*data[i].second[1]) +(betas[2]*data[i-1].second[2]) +(betas[3]*data[i-1].second[3]) +(betas[4]*data[i-1].second[4]) +(betas[5]*data[i-1].second[5]) +(betas[6]*data[i-1].second[6]) +(betas[7]*data[i].second[2]) ;
        
        if((100*prediction)>=(100+p)*data[i].second[0] && hold_quantity<x){
            cash_in_hand-=data[i].second[0];
            daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
            order_stats.push_back({data[i].first,"BUY","1",to_string(data[i].second[0])});
            hold_quantity++;
        }
        
        else if((100*prediction)<=(100-p)*data[i].second[0] && hold_quantity>(-1*x)){
            cash_in_hand+=data[i].second[0];
            daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
            order_stats.push_back({data[i].first,"SELL","1",to_string(data[i].second[0])});
            hold_quantity--;
        }
        else{
            daily_cashflow.push_back({data[i].first,to_string(cash_in_hand)});
        }
         
    }
    if(hold_quantity>0){
        cash_in_hand += data[row_count-1].second[0]*hold_quantity;
        hold_quantity = 0;
        
    }
    else if(hold_quantity<0){
        cash_in_hand -= ((data[row_count-1].second[0])*abs(hold_quantity));
        hold_quantity = 0;
    }
    return cash_in_hand; 
}



void write_data(vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats){
    
    ofstream dc_file("daily cashflow.csv");
    dc_file<<"Date,Cashflow"<<endl;
    for(auto x:daily_cashflow){
        dc_file<<x.first<<","<<x.second<<endl;
    }
    dc_file.close();
    
    ofstream os_file("order_statistics.csv");
    
    os_file<<"Date,Order_dir,Quantity,Price"<<endl;
    for(auto row:order_stats){
        string temp;
        for(int i = 0;i<4;i++){
            temp+=row[i];
            if(i!=3){
                temp.push_back(',');
            }
        }
        os_file<<temp<<endl;
    }
    os_file.close();
    
}


int main(int argc, const char * argv[]) {
    // insert code here...
    string symbol = argv[1];
    map<string,vector<pair<string,db>>> data;
    map<string,vector<pair<string,string>>> daily_cashflow;
    map<string,vector<vector<string>>> order_stats;
    map<string,db> pnl;
    vector<pair<string,vector<db>>> train_data;
    vector<pair<string,vector<db>>> lr_data;
    omp_set_num_threads(4);
    #pragma omp parallel
    {
       int thread_num = omp_get_thread_num();
       
       if (thread_num==0){
		   	#pragma omp task
		   	{
		   		#pragma omp critical
		   		{
			   		get_data_basic(data["basic"]);
				    db temp1 = solve_basic(7,5,data["basic"],daily_cashflow["basic"],order_stats["basic"]);
				    pnl["basic"] = temp1;
				    cout<<"basic"<<temp1<<endl;
				}    
		   	}           
       }
       else if (thread_num==1){
		   	#pragma omp task
		   	{
		   		#pragma omp critical
		   		{
		   		get_data_dma(data["dma"]);
		        db temp2 = solve_dma(50,5,2,data["dma"],daily_cashflow["dma"],order_stats["dma"]);
		        pnl["dma"] = temp2;
		        cout<<"dma"<<temp2<<endl;
		        }
		   	}           
       }
       else if (thread_num==2){
		   	#pragma omp task
		   	{
		   		#pragma omp critical
		   		{
		   		get_data_dma_plus(data["dma++"]);
		        db temp3 = solve_dma_plus(14,5,5,2,0.2,28,data["dma++"],daily_cashflow["dma++"],order_stats["dma++"]);
		        pnl["dma++"] = temp3;
		        cout<<"dma++"<<temp3<<endl;
		        }
		   	}           
       }
       else if (thread_num==3){
		   	#pragma omp task
		   	{
		   		#pragma omp critical
		   		{
		   		get_training_data(train_data);
		        get_data_lr(lr_data);
		        vector<db> betas = get_betas(train_data);
		        db temp_lr = solve_lr(2,5,lr_data,daily_cashflow["lr"],order_stats["lr"], betas);
		        pnl["lr"] = temp_lr;
		        cout<<"lr"<<temp_lr<<endl;
		        }
		   	}           
       }
   		
    }
    
    db max = pnl["basic"];
    string max_pnl = "basic";
    for(auto x: pnl){
        if(x.second>max){
            max = x.second;
            max_pnl = x.first;
        }
    }
    write_data(daily_cashflow[max_pnl],order_stats[max_pnl]);
    ofstream res_file("final_pnl.txt");
    res_file<<max<<endl;
    res_file.close();
    
    return 0;
}
