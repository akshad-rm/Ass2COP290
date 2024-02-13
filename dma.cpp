#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <algorithm>
#define db double

using namespace std;


void get_data(vector<pair<string,db>>&data){
    ifstream datafile("data_dma.csv");
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


void solve(int n,int x,int p,vector<pair<string,db>>&data,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats){
    
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
    ofstream res_file("final_pnl.txt");
    res_file<<to_string(cash_in_hand)<<endl;
    res_file.close();
    
    
    
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
    int n = stoi(argv[2]);
    int x = stoi(argv[3]);
    int p = stoi(argv[4]);
    vector<pair<string,db>>data;
    vector<pair<string,string>>daily_cashflow;
    vector<vector<string>>order_stats;
    get_data(data);
    solve(n,x,p,data,daily_cashflow,order_stats);
    write_data(daily_cashflow,order_stats);
    
    
    return 0;
}
