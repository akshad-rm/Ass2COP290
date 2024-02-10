//
//  main.cpp
//  copass
//
//  Created by Akshad Mhaske on 08/02/24.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#define db double

using namespace std;


void get_data(vector<pair<string,db>>&data_1,vector<pair<string,db>>&data_2,string symbol_1,string symbol_2){
    ifstream datafile(symbol_1+".csv");
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
        data_1.push_back({date,close_price});
    }
    reverse(data_1.begin(),data_1.end());
    
    
    ifstream datafile_2(symbol_2+".csv");
    line = "" ;
    
    if(!datafile_2.is_open()){
        cerr << "could not open the file : data.csv" << endl;
    }
    raw_data.clear();
    title_flag = 0;
    
    while(getline(datafile_2, line)) {
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
    
    
    
    row_count = int(raw_data.size());
    for(int i = 0;i<row_count;i++){
        string date = raw_data[i][0];
        db close_price = stod(raw_data[i][1]);
        data_2.push_back({date,close_price});
    }
    reverse(data_2.begin(),data_1.end());
    
    
}


bool is_date_earlier(string d1,string d2){
    return d1<d2;
    
}

bool is_date_earlier_or_today(string d1,string d2){
    return d1<=d2;
    
}

void solve(string symbol_1,string symbol_2,int x,int n,db threshold,string start_date,string end_date,vector<pair<string,db>>&data_1,vector<pair<string,db>>&data_2,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats_1,vector<vector<string>>&order_stats_2){
    
    
    int first_day = 0;
    while(is_date_earlier(data_1[first_day].first, start_date)){
        first_day++;
    }
    
    int last_day = first_day;
    while(is_date_earlier_or_today(data_1[last_day].first, end_date)){
        last_day++;
    }
    last_day--;
    
    
    map<int,db>spread;
    map<int,db>rolling_mean;
    map<int,db>rolling_square;
    db rolling_mean_window = 0;
    db rolling_square_window = 0;
    for(int i = first_day-2*n +1;i<=last_day;i++){
        spread[i] = data_1[i].second - data_2[i].second;
    }
    for(int i = first_day-2*n+1;i<=first_day-n;i++){
        rolling_mean_window+=spread[i];
        rolling_square_window+=(spread[i]*spread[i]);
        
    }
    
    for(int i = first_day-n+1;i<=last_day;i++){
        db add_window = spread[i];
        db remove_window = spread[i-n];
        rolling_mean_window+=add_window - remove_window;
        rolling_mean[i] = rolling_mean_window/n;
        
        rolling_square_window += (add_window*add_window) - (remove_window*remove_window);
        rolling_square[i] = rolling_square_window/n;
    }
    db z_score = 0;
    db cash_in_hand = 0;
    int hold_quantity_1 = 0;
    int hold_quantity_2 = 0;
    for(int curr_day = first_day;curr_day<=last_day;curr_day++){
        db rolling_std = rolling_square[curr_day] - (rolling_mean[curr_day]*rolling_mean[curr_day]);
        rolling_std = sqrt(rolling_std);
        
        z_score = (spread[curr_day] - rolling_mean[curr_day])/rolling_std;
        
        if(z_score>threshold && hold_quantity_1>(-1*x) && hold_quantity_2<(x)){
            cash_in_hand+=data_1[curr_day].second;
            
            order_stats_1.push_back({data_1[curr_day].first,"SELL","1",to_string(data_1[curr_day].second)});
            hold_quantity_1--;
            
            cash_in_hand-=data_2[curr_day].second;
            
            order_stats_2.push_back({data_2[curr_day].first,"BUY","1",to_string(data_2[curr_day].second)});
            hold_quantity_2++;
            
            daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
        }
        
        else if(z_score<(-1*threshold) && hold_quantity_2>(-1*x) && hold_quantity_1<(x)){
            cash_in_hand+=data_2[curr_day].second;
            
            order_stats_2.push_back({data_2[curr_day].first,"SELL","1",to_string(data_2[curr_day].second)});
            hold_quantity_2--;
            
            cash_in_hand-=data_1[curr_day].second;
            
            order_stats_1.push_back({data_1[curr_day].first,"BUY","1",to_string(data_1[curr_day].second)});
            hold_quantity_1++;
            
            daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
        }
        else{
            daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
        }
        //cout<<z_score<<endl;
        
        
        
    }
    
    if(hold_quantity_1>0){
        cash_in_hand += data_1[last_day].second*hold_quantity_1;
        hold_quantity_1 = 0;
        
    }
    else if(hold_quantity_1<0){
        cash_in_hand -= ((data_1[last_day].second)*abs(hold_quantity_1));
        hold_quantity_1 = 0;
    }
    
    if(hold_quantity_2>0){
        cash_in_hand += data_2[last_day].second*hold_quantity_2;
        hold_quantity_2 = 0;
        
    }
    else if(hold_quantity_2<0){
        cash_in_hand -= ((data_2[last_day].second)*abs(hold_quantity_2));
        hold_quantity_2 = 0;
    }
    
    
    ofstream res_file("final_pnl.txt");
    res_file<<cash_in_hand<<endl;
    res_file.close();
    

    
    
    
}



void write_data(vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats_1,vector<vector<string>>&order_stats_2){
    
    ofstream dc_file("daily cashflow.csv");
    dc_file<<"Date,Cashflow"<<endl;
    for(auto x:daily_cashflow){
        dc_file<<x.first<<","<<x.second<<endl;
    }
    dc_file.close();
    
    ofstream os_file("order_statistics_1.csv");
    
    os_file<<"Date,Order_dir,Quantity,Price"<<endl;
    for(auto row:order_stats_1){
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
    
    
    ofstream os_file_2("order_statistics_2.csv");
    cout<<order_stats_2.size()<<" "<<order_stats_1.size()<<endl;
    os_file_2<<"Date,Order_dir,Quantity,Price"<<endl;
    for(auto row:order_stats_2){
        string temp;
        for(int i = 0;i<4;i++){
            temp+=row[i];
            if(i!=3){
                temp.push_back(',');
            }
        }
        os_file_2<<temp<<endl;
    }
    os_file_2.close();
    
}




int main(int argc, const char * argv[]) {
    // insert code here...
    string symbol_1 = "SBIN";
    string symbol_2 = "ADANIENT";
    int n = 5;
    int x = 3;
    db threshold  = 1.5;
    string start_date = "2023-05-05";
    string end_date = "2023-10-10";
    vector<pair<string,db>>data_1;
    vector<pair<string,db>>data_2;
    vector<pair<string,string>>daily_cashflow;
    vector<vector<string>>order_stats_1;
    vector<vector<string>>order_stats_2;
    get_data(data_1,data_2,symbol_1,symbol_2);
    solve(symbol_1,symbol_2,x,n,threshold,start_date,end_date, data_1,data_2,daily_cashflow,order_stats_1,order_stats_2);
    
    write_data(daily_cashflow, order_stats_1, order_stats_2);
    
    
    
    return 0;
}

