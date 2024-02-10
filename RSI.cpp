//
//  MACD.cpp
//  copass
//
//  Created by Akshad Mhaske on 09/02/24.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#define db double

using namespace std;

bool is_date_earlier(string d1,string d2){
    return d1<d2;
    
}

bool is_date_earlier_or_today(string d1,string d2){
    return d1<=d2;
    
}



void get_data(vector<pair<string,db>>&data){
    ifstream datafile("data.csv");
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

void solve(int x,int n,db oversold_threshold,db overbought_threshold,string start_date,string end_date,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats,vector<pair<string,db>>&data){
    
    int first_day = 0;
    while(is_date_earlier(data[first_day].first, start_date)){
        first_day++;
    }
    
    int last_day = first_day;
    while(is_date_earlier_or_today(data[last_day].first, end_date)){
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
    int hold_quantity = 0;
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
        cash_in_hand-=data[first_day].second;
        daily_cashflow.push_back({data[first_day].first,to_string(cash_in_hand)});
        order_stats.push_back({data[first_day].first,"BUY","1",to_string(data[first_day].second)});
        hold_quantity++;
    }
    else if(RSI>overbought_threshold && hold_quantity>(-1*x)){
        cash_in_hand+=data[first_day].second;
        daily_cashflow.push_back({data[first_day].first,to_string(cash_in_hand)});
        order_stats.push_back({data[first_day].first,"SELL","1",to_string(data[first_day].second)});
        hold_quantity--;
    }
    else{
        daily_cashflow.push_back({data[first_day].first,to_string(cash_in_hand)});
    }
    
    for(int current_day = first_day+1;current_day<=last_day;current_day++){
        db window_add = data[current_day].second;
        db window_remove = data[current_day-n].second;
        
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
            daily_cashflow.push_back({data[current_day].first,to_string(cash_in_hand)});
            order_stats.push_back({data[current_day].first,"BUY","1",to_string(data[current_day].second)});
            hold_quantity++;
        }
        else if(RSI>overbought_threshold && hold_quantity>(-1*x)){
            cash_in_hand+=data[current_day].second;
            daily_cashflow.push_back({data[current_day].first,to_string(cash_in_hand)});
            order_stats.push_back({data[current_day].first,"SELL","1",to_string(data[current_day].second)});
            hold_quantity--;
        }
        else{
            daily_cashflow.push_back({data[current_day].first,to_string(cash_in_hand)});
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



    







int main(){
    string symbol = "SBIN";
    int x = 100;
    int n = 13;
    db oversold_threshold = 30;
    db overbought_threshold = 70;
    string start_date = "2023-02-05";
    string end_date = "2023-12-10";
    vector<pair<string,string>>daily_cashflow;
    vector<vector<string>>order_stats;
    vector<pair<string,db>> data;
    get_data(data);
    solve(x,n,oversold_threshold,overbought_threshold,start_date,end_date,daily_cashflow,order_stats,data);
    write_data(daily_cashflow,order_stats);
    
}

