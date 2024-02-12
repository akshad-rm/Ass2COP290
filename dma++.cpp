#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#define db double

using namespace std;


void get_data(vector<pair<string,db>>&data){
    ifstream datafile("data_dma++.csv");
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
        string date = raw_data[i][0];
        db close_price = stod(raw_data[i][1]);
        data.push_back({date,close_price});
    }
    reverse(data.begin(),data.end());
    
    
}


void solve(int n,int x,int p,db c1,db c2,int max_hold_days,vector<pair<string,db>>&data,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats){
    
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
    ofstream res_file("final_pnl.txt");
    res_file<<cash_in_hand<<endl;
    res_file.close();
    
    
    
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
    int n = stoi(argv[2]);
    int x = stoi(argv[3]);
    int p = stoi(argv[4]);
    int max_hold_days = stoi(argv[5]);
    db c1= stod(argv[6]);
    db c2 = stod(argv[7]);
    vector<pair<string,db>>data;
    vector<pair<string,string>>daily_cashflow;
    vector<vector<string>>order_stats;
    get_data(data);
    solve(n,x,p,c1,c2,max_hold_days,data,daily_cashflow,order_stats);
    write_data(daily_cashflow,order_stats);
    
    
    return 0;
}
