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
#include <algorithm>
#define db double

using namespace std;



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

void get_data(vector<pair<string,db>>&data){
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





void solve(int x,vector<pair<string,db>>&data,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats,string start_date,string end_date){
    
   
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
            daily_cashflow.push_back({data[current_day].first,to_string(cash_in_hand)});
            order_stats.push_back({data[current_day].first,"BUY","1",to_string(data[current_day].second)});
            hold_quantity++;
            
            
        }
        else if(curr_macd<curr_signal && hold_quantity>(-1*x)){
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
    
    
    for(auto &x:daily_cashflow){
    	x.first = convert_to_d(x.first);
    }
    for(auto &x:order_stats){
    	x[0] = convert_to_d(x[0]);
    }
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




int main(int argc, const char * argv[]){
    string symbol = argv[1];
    int x = stoi(argv[2]);
    string start_date = argv[3];
    string end_date = argv[4];
    
    start_date = convert_to_y(start_date);
    end_date = convert_to_y(end_date);
    
    vector<pair<string,string>>daily_cashflow;
    vector<vector<string>>order_stats;
    vector<pair<string,db>> data;
    get_data(data);
    solve(x,data,daily_cashflow,order_stats,start_date,end_date);
    write_data(daily_cashflow,order_stats);
}
