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

bool is_date_earlier(string d1,string d2){
    return d1<d2;
    
}

bool is_date_earlier_or_today(string d1,string d2){
    return d1<=d2;
    
}

string convert_to_d(string date){
    string res = "dd/mm/yyyy";
    res[0] = date[8];
    res[1] = date[9];
    res[3] = date[5];
    res[4] = date[6];
    res[6] = date[0];
    res[7] = date[1];
    res[8] = date[2];
    res[9] = date[3];
    return res;
}

string convert_to_y(string date){
    string res = "yyyy-mm-dd";
    res[0] = date[6];
    res[1] = date[7];
    res[2] = date[8];
    res[3] = date[9];
    res[5] = date[3];
    res[6] = date[4];
    res[8] = date[0];
    res[9] = date[1];
    return res;
}

void get_data_macd(vector<pair<string,db>>&data){
    ifstream datafile("data_macd.csv");
    string line ;
    
    if(!datafile.is_open()){
        cerr << "could not open the file : data_macd.csv" << endl;
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
        string date = convert_to_y(raw_data[i][0]);
        db close_price = stod(raw_data[i][1]);
        data.push_back({date,close_price});
    }
    reverse(data.begin(),data.end());
    
    
    
}





db solve_macd(int x,vector<pair<string,db>>&data,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats,string start_date,string end_date){
    
   
    int first_day = 0;
    while(is_date_earlier(data[first_day].first, start_date)){
        first_day++;
        
    }
    
    int last_day = first_day;
    while(last_day<data.size() && is_date_earlier_or_today(data[last_day].first, end_date)){
        last_day++;
        
    }
    last_day--;
    
    
    map<int,db>macd;
    map<int,db>short_ewm;
    db lookback_period = 12;
    db alpha = (2/(lookback_period+1));
    
    db ewm = data[first_day].second;
    short_ewm[first_day] = ewm;
    for(int curr = first_day+1;curr<=last_day;curr++){
        ewm = alpha*(data[curr].second-ewm)+ewm;
        short_ewm[curr] = ewm;
    }
    map<int,db>long_ewm;
    lookback_period = 26;
    alpha = (2/(lookback_period+1));
    
    ewm =data[first_day].second;
    long_ewm[first_day] = ewm;
    for(int curr = first_day+1;curr<=last_day;curr++){
        ewm = alpha*(data[curr].second-ewm)+ewm;
        long_ewm[curr] = ewm;
    }
    for(int curr = first_day;curr<=last_day;curr++){
        
        db curr_macd = short_ewm[curr] - long_ewm[curr];
        macd[curr] = curr_macd;
    }
    
    map<int,db>signal;
    lookback_period = 9;
    alpha = (2/(lookback_period+1));
    
    ewm = macd[first_day];
    signal[first_day] = ewm;
    for(int curr = first_day+1;curr<=last_day;curr++){
        ewm = alpha*(macd[curr]-ewm)+ewm;
        signal[curr] = ewm;
    }
    
    db cash_in_hand = 0;
    int hold_quantity = 0;
    for(int current_day = first_day;current_day<=last_day;current_day++){
        
        db curr_signal = signal[current_day];
        db curr_macd = macd[current_day];
        //cout<<curr_signal<<endl;
        if(curr_macd>curr_signal && hold_quantity<x){
            cash_in_hand-=data[current_day].second;
            daily_cashflow.push_back({convert_to_d(data[current_day].first),to_string(cash_in_hand)});
            order_stats.push_back({convert_to_d(data[current_day].first),"BUY","1",to_string(data[current_day].second)});
            hold_quantity++;
            
            
        }
        else if(curr_macd<curr_signal && hold_quantity>(-1*x)){
            cash_in_hand+=data[current_day].second;
            daily_cashflow.push_back({convert_to_d(data[current_day].first),to_string(cash_in_hand)});
            order_stats.push_back({convert_to_d(data[current_day].first),"SELL","1",to_string(data[current_day].second)});
            hold_quantity--;
            
        }
        else{
            daily_cashflow.push_back({convert_to_d(data[current_day].first),to_string(cash_in_hand)});
            
        }
    }
    
    if(hold_quantity>0){
        cash_in_hand += data[last_day].second*hold_quantity;
        hold_quantity = 0;
        
    }
    else if(hold_quantity<0){
        cash_in_hand -= ((data[last_day].second)*abs(hold_quantity));
        hold_quantity = 0;
    }
    return cash_in_hand;
    
}

void get_data_rsi(vector<pair<string,db>>&data){
    ifstream datafile("data_basic.csv");
    string line ;
    
    if(!datafile.is_open()){
        cerr << "could not open the file : data.csv" << endl;
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
        string date = convert_to_y(raw_data[i][0]);
        db close_price = stod(raw_data[i][1]);
        data.push_back({date,close_price});
    }
    reverse(data.begin(),data.end());
    
}

db solve_rsi(int x,int n,db oversold_threshold,db overbought_threshold,string start_date,string end_date,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats,vector<pair<string,db>>&data){
    
    int first_day = 0;
    while(is_date_earlier(data[first_day].first, start_date)){
        first_day++;
    }
    
    int last_day = first_day;
    while(last_day<data.size() && is_date_earlier_or_today(data[last_day].first, end_date)){
        last_day++;
    }
    last_day--;
    
    map<int,db>gain_loss;
    for(int curr = first_day - n +1;curr<=last_day;curr++){
        db diff = data[curr].second - data[curr-1].second;
        gain_loss[curr] = diff;
    }
    db gain_window = 0;
    db loss_window = 0;
    for(int curr = first_day-n+1;curr<=first_day;curr++){
        if(gain_loss[curr]>0){
            gain_window+=gain_loss[curr];
        }
        else{
            loss_window+=abs(gain_loss[curr]);
        }
    }
    db cash_in_hand = 0;
    db hold_quantity = 0;
    db avg_gain = gain_window/n;
    db avg_loss = loss_window/n;
    db RS;
    db RSI;
    if(avg_loss==0){
        RSI = 100;
    }
    else{
        RS = avg_gain/avg_loss;
        RSI = 100 - (100/(1+RS));
    }
    if(RSI<=oversold_threshold && hold_quantity<x){
        cash_in_hand-=data[first_day].second;
        daily_cashflow.push_back({convert_to_d(data[first_day].first),to_string(cash_in_hand)});
        order_stats.push_back({convert_to_d(data[first_day].first),"BUY","1",to_string(data[first_day].second)});
        hold_quantity++;
    }
    else if(RSI>=overbought_threshold && hold_quantity>(-1*x)){
        cash_in_hand+=data[first_day].second;
        daily_cashflow.push_back({convert_to_d(data[first_day].first),to_string(cash_in_hand)});
        order_stats.push_back({convert_to_d(data[first_day].first),"SELL","1",to_string(data[first_day].second)});
        hold_quantity--;
    }
    else{
        daily_cashflow.push_back({convert_to_d(data[first_day].first),to_string(cash_in_hand)});
    }
    
    for(int current_day = first_day+1;current_day<=last_day;current_day++){
        db window_add = gain_loss[current_day];
        db window_remove = gain_loss[current_day-n];
        
        if(window_add>0){
            gain_window+=window_add;
        }
        else if(window_add<0){
            loss_window += abs(window_add);
        }
        
        if(window_remove>0){
            gain_window-=window_remove;
        }
        else if(window_remove<0){
            loss_window-=abs(window_remove);
        }
        
        db avg_gain = gain_window/n;
        db avg_loss = loss_window/n;
        db RS;
        db RSI;
        if(avg_loss==0){
            RSI = 100;
        }
        else{
            RS = avg_gain/avg_loss;
            RSI = 100 - (100/(1+RS));
        }
        if(RSI<oversold_threshold && hold_quantity<x){
            cash_in_hand-=data[current_day].second;
            daily_cashflow.push_back({convert_to_d(data[current_day].first),to_string(cash_in_hand)});
            order_stats.push_back({convert_to_d(data[current_day].first),"BUY","1",to_string(data[current_day].second)});
            hold_quantity++;
        }
        else if(RSI>overbought_threshold && hold_quantity>(-1*x)){
            cash_in_hand+=data[current_day].second;
            daily_cashflow.push_back({convert_to_d(data[current_day].first),to_string(cash_in_hand)});
            order_stats.push_back({convert_to_d(data[current_day].first),"SELL","1",to_string(data[current_day].second)});
            hold_quantity--;
        }
        else{
            daily_cashflow.push_back({convert_to_d(data[current_day].first),to_string(cash_in_hand)});
        }
        
    }
    
    if(hold_quantity>0){
        cash_in_hand += data[last_day].second*hold_quantity;
        hold_quantity = 0;
        
    }
    else if(hold_quantity<0){
        cash_in_hand -= ((data[last_day].second)*abs(hold_quantity));
        hold_quantity = 0;
    }
    return cash_in_hand;
    
}

void get_data_adx(vector<pair<string,vector<db>>>&data){
    ifstream datafile("data_basic.csv");
    string line ;
    
    if(!datafile.is_open()){
        cerr << "could not open the file : data.csv" << endl;
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
        string date = convert_to_y(raw_data[i][0]);
        db close_price = stod(raw_data[i][1]);
        db high = stod(raw_data[i][2]);
        db low = stod(raw_data[i][3]);
        db prev_close = stod(raw_data[i][4]);
        data.push_back({date,{close_price,high,low,prev_close}});
    }
    reverse(data.begin(),data.end());
    
}

db solve_adx(int x,int n, db adx_threshold,string start_date,string end_date,vector<pair<string,vector<db>>> &data,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats){
    
    int first_day = 0;
    while(is_date_earlier(data[first_day].first, start_date)){
        first_day++;
    }
    
    int last_day = first_day;
    while( last_day<data.size() && is_date_earlier_or_today(data[last_day].first, end_date)){
        last_day++;
    }
    last_day--;
    
    map<int,db>true_range;
    for(int curr = first_day - n+1;curr<=last_day;curr++){
        db maximum = max(data[curr].second[1] - data[curr].second[2],max(data[curr].second[1] - data[curr].second[3],data[curr].second[2] - data[curr].second[3]));
        true_range[curr] = maximum;
    }
    map<int,db>ATR;
    map<int,db>DMp;
    map<int,db>DMm;
    for(int curr = first_day - n+1;curr<=last_day;curr++){
        if(data[curr].second[1] - data[curr-1].second[1]>0){
            DMp[curr] = data[curr].second[1] - data[curr-1].second[1];
        }
        else{
            DMp[curr] = 0;
        }
        
        if(data[curr].second[2] - data[curr-1].second[2]>0){
            DMm[curr] = data[curr].second[2] - data[curr-1].second[2];
        }
        else{
            DMm[curr] = 0;
        }
        
        
    }
   
    ATR[first_day] = true_range[first_day];
    db look_back_period = n;
    db alpha = 2/(look_back_period+1);
    
    
    db ewm = ATR[first_day];
    
    for(int curr = first_day+1;curr<=last_day;curr++){
        ewm = alpha*(true_range[curr] - ewm) + ewm;
        
        ATR[curr] = ewm;
        
    }
    
    map<int,db>DIp;
    ewm = DMp[first_day]/ATR[first_day];
    DIp[first_day] = ewm;
    
    for(int curr = first_day+1;curr<=last_day;curr++){
        ewm = alpha*((DMp[curr]/ATR[curr])-ewm)+ewm;
        DIp[curr]=  ewm;
        
        
    }
    
    map<int,db>DIm;
    ewm = DMm[first_day]/ATR[first_day];
    
    DIm[first_day] = ewm;
    for(int curr = first_day+1;curr<=last_day;curr++){
        ewm = alpha*((DMm[curr]/ATR[curr])-ewm)+ewm;
        DIm[curr]=  ewm;
        
    }
    map<int,db>DX;
    for(int curr = first_day;curr<=last_day;curr++){
        db dx;
        if(DIp[curr]+DIm[curr]==0){
            dx = 0;
        }
        else{
            dx = ((DIp[curr]-DIm[curr])/(DIp[curr]+DIm[curr]))*100;
        }
        DX[curr] = dx;
        
    }
    map<int,db>ADX;
    ewm = DX[first_day];
    ADX[first_day] = ewm;
    for(int curr = first_day+1;curr<=last_day;curr++){
        ewm = alpha*((DX[curr])-ewm)+ewm;
        ADX[curr] = ewm;
    }
    db cash_in_hand = 0;
    int hold_quantity = 0;
    for(int current_day = first_day;current_day<=last_day;current_day++){
        db adx = ADX[current_day];
        
        if(adx>=adx_threshold && hold_quantity<x && DX[current_day]!=0){
            cash_in_hand-=data[current_day].second[0];
            daily_cashflow.push_back({convert_to_d(data[current_day].first),to_string(cash_in_hand)});
            order_stats.push_back({convert_to_d(data[current_day].first),"BUY","1",to_string(data[current_day].second[0])});
            hold_quantity++;
        }
        else if(adx<=adx_threshold && hold_quantity>(-1*x) && DX[current_day]!=0){
            cash_in_hand+=data[current_day].second[0];
            daily_cashflow.push_back({convert_to_d(data[current_day].first),to_string(cash_in_hand)});
            order_stats.push_back({convert_to_d(data[current_day].first),"SELL","1",to_string(data[current_day].second[0])});
            hold_quantity--;
        }
        else{
            daily_cashflow.push_back({convert_to_d(data[current_day].first),to_string(cash_in_hand)});
        }
    }
    if(hold_quantity>0){
        cash_in_hand += data[last_day].second[1]*hold_quantity;
        hold_quantity = 0;
            
    }
    else if(hold_quantity<0){
        cash_in_hand -= ((data[last_day].second[1])*abs(hold_quantity));
        hold_quantity = 0;
    }
    return cash_in_hand;
}
    

void write_data(vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats){
    
    ofstream dc_file("daily_cashflow.csv");
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
    string start_date = argv[2];
    string end_date = argv[3];
    map<string,vector<pair<string,db>>> data;
    map<string,vector<pair<string,string>>> daily_cashflow;
    map<string,vector<vector<string>>> order_stats;
    map<string,db> pnl;
    vector<pair<string,vector<db>>> train_data;
    vector<pair<string,vector<db>>> lr_data;
    vector<pair<string,vector<db>>> adx_data;
    omp_set_num_threads(7);
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
       else if (thread_num==4){
		   	#pragma omp task
		   	{
		   		#pragma omp critical
		   		{
		   		get_data_macd(data["macd"]);
                string sd = convert_to_y(start_date);
                string ed = convert_to_y(end_date);
		        db temp4 = solve_macd(5,data["macd"],daily_cashflow["macd"],order_stats["macd"],sd,ed);
		        pnl["macd"] = temp4;
		        cout<<"macd"<<temp4<<endl;
		        }
		   	}           
       }
       else if (thread_num==5){
		   	#pragma omp task
		   	{
		   		#pragma omp critical
		   		{
		   		get_data_rsi(data["rsi"]);
                string sd = convert_to_y(start_date);
                string ed = convert_to_y(end_date);
		        db temp5 = solve_rsi(5,14,30,70,sd,ed,daily_cashflow["rsi"],order_stats["rsi"],data["rsi"]);
		        pnl["rsi"] = temp5;
		        cout<<"rsi"<<temp5<<endl;
		        }
		   	}           
       }
       else if (thread_num==6){
		   	#pragma omp task
		   	{
		   		#pragma omp critical
		   		{
		   		get_data_adx(adx_data);
                string sd = convert_to_y(start_date);
                string ed = convert_to_y(end_date);
		        db temp6 = solve_adx(5,14,25,sd,ed,adx_data,daily_cashflow["adx"],order_stats["adx"]);
		        pnl["adx"] = temp6;
		        cout<<"adx"<<temp6<<endl;
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
