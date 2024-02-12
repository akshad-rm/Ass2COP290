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
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#define db double


using namespace std;


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
        string date = convert_to_y(raw_data[i][0]);
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
    while(last_day<data_1.size() && is_date_earlier_or_today(data_1[last_day].first, end_date)){
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
        
        if(z_score>=threshold && hold_quantity_1>(-1*x) && hold_quantity_2<(x)){
            cash_in_hand+=data_1[curr_day].second;
            
            order_stats_1.push_back({data_1[curr_day].first,"SELL","1",to_string(data_1[curr_day].second)});
            hold_quantity_1--;
            
            cash_in_hand-=data_2[curr_day].second;
            
            order_stats_2.push_back({data_2[curr_day].first,"BUY","1",to_string(data_2[curr_day].second)});
            hold_quantity_2++;
            
            daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
        }
        
        else if(z_score<=(-1*threshold) && hold_quantity_2>(-1*x) && hold_quantity_1<(x)){
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
    for(auto &x:daily_cashflow){
    	x.first = convert_to_d(x.first);
    }
    for(auto &x:order_stats_2){
    	x[0] = convert_to_d(x[0]);
    }
    for(auto &x:order_stats_1){
    	x[0] = convert_to_d(x[0]);
    }
    ofstream dc_file("daily_cashflow.csv");
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




int main(int argc,const char* argv[]) {
    // insert code here...
    string symbol_1 = argv[1];
    string symbol_2 = argv[2];
    int n = stoi(argv[4]);
    int x = stoi(argv[3]);
    db threshold  = stod(argv[5]);
    string start_date = argv[6];
    string end_date = argv[7];
    start_date = convert_to_y(start_date);
    end_date = convert_to_y(end_date);
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


