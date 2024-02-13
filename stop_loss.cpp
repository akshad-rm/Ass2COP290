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
#include <queue>
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
        string date = convert_to_y(raw_data[i][0]);
        db close_price = stod(raw_data[i][1]);
        data_2.push_back({date,close_price});
    }
    reverse(data_2.begin(),data_2.end());
    
    if(data_1[0].first<data_2[0].first){
    	while(data_1[0].first<data_2[0].first){
    		data_1.erase(data_1.begin());
    	}
    }
    if(data_2[0].first<data_1[0].first){
    	while(data_2[0].first<data_1[0].first){
    		data_2.erase(data_2.begin());
    	}
    }
    
    
}


struct stock{
    string position;
    db mean;
    db std;
};

bool is_date_earlier(string d1,string d2){
    return d1<d2;
    
}

bool is_date_earlier_or_today(string d1,string d2){
    return d1<=d2;
    
}




void solve(string symbol_1,string symbol_2,int x,int n,db threshold,db stop_loss_threshold,string start_date,string end_date,vector<pair<string,db>>&data_1,vector<pair<string,db>>&data_2,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats_1,vector<vector<string>>&order_stats_2){
    
    
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
    for(int i = first_day-n;i<=last_day;i++){
        spread[i] = data_1[i].second - data_2[i].second;
    }
    for(int i = first_day-n;i<=first_day-1;i++){
        rolling_mean_window+=spread[i];
        rolling_square_window+=(spread[i]*spread[i]);
        
    }
    
    for(int i = first_day;i<=last_day;i++){
        db add_window = spread[i];
        db remove_window = spread[i-n];
        rolling_mean_window += add_window - remove_window;
        rolling_mean[i] = rolling_mean_window/n;
        
        rolling_square_window += (add_window*add_window) - (remove_window*remove_window);
        rolling_square[i] = rolling_square_window/n;
    }
    db z_score = 0;
    db cash_in_hand = 0;
    int hold_quantity_1 = 0;
    int hold_quantity_2 = 0;
    vector<stock>positions;
    map<int,db>rolling_std_map;
    string curr_pos = "empty";
    for(int curr_day = first_day;curr_day<=last_day;curr_day++){
        db rolling_std = rolling_square[curr_day] - (rolling_mean[curr_day]*rolling_mean[curr_day]);
        rolling_std = sqrt(rolling_std);
        rolling_std_map[curr_day] = rolling_std;
        
        z_score = (spread[curr_day] - rolling_mean[curr_day])/rolling_std;
        //cout<<data_1[curr_day].first<<" today's "<<rolling_mean[curr_day]<<" "<<rolling_square[curr_day]<<" "<<rolling_std<<" "<<z_score<<endl;
        int quant_1 = 0;
        int quant_2 = 0;
        if(positions.size()==0){
        	curr_pos="empty";
        }
        else{
        	if(positions[0].position=="bought"){
        		curr_pos="bought";
        	}
        	else{
        		curr_pos="sold";
        	}
        }
        if(z_score>threshold && hold_quantity_1>(-1*x) && hold_quantity_2<(x)){
            quant_1--;
            quant_2++;
            
            if(curr_pos=="empty"){
                stock new_stock;
                new_stock.position = "sold";
                new_stock.mean = rolling_mean[curr_day];
                new_stock.std = rolling_std_map[curr_day];
                positions.push_back(new_stock);
                curr_pos = "sold";
            }
            else if(curr_pos=="sold"){
                stock new_stock;
                new_stock.position = "sold";
                new_stock.mean = rolling_mean[curr_day];
                new_stock.std = rolling_std_map[curr_day];
                positions.push_back(new_stock);
                curr_pos = "sold";
            }
            else{
                positions.erase(positions.begin());
                if(positions.size()==0){
                    curr_pos = "empty";
                }
            }
            
            
            
        }
        
        
        else if(z_score<(-1*threshold) && hold_quantity_2>(-1*x) && hold_quantity_1<(x)){
            quant_1++;
            quant_2--;
            
            if(curr_pos=="empty"){
            	
                stock new_stock;
                new_stock.position = "bought";
                new_stock.mean = rolling_mean[curr_day];
                new_stock.std = rolling_std_map[curr_day];
                positions.push_back(new_stock);
                curr_pos = "bought";
            }
            else if(curr_pos=="bought"){
                stock new_stock;
                
                new_stock.position = "bought";
                new_stock.mean = rolling_mean[curr_day];
                new_stock.std = rolling_std_map[curr_day];
                positions.push_back(new_stock);
                curr_pos = "bought";
            }
            else{
            	
                positions.erase(positions.begin());
                if(positions.size()==0){
                    curr_pos = "empty";
                }
                
            }
            
            
        }
        
        vector<bool>to_be_removed(positions.size(),false);
        if(curr_pos=="empty"){
        	
            if(quant_1!=0){
                if(quant_1>0){
                    cash_in_hand-= (data_1[curr_day].second*(quant_1));
                    
                    order_stats_1.push_back({data_1[curr_day].first,"BUY",to_string(quant_1),to_string(data_1[curr_day].second)});
                    cash_in_hand+=data_2[curr_day].second*(quant_1);
                    
                    order_stats_2.push_back({data_2[curr_day].first,"SELL",to_string(quant_1),to_string(data_2[curr_day].second)});
                    daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
                    
                }
                else{
                    cash_in_hand+=(data_1[curr_day].second*(-1*quant_1));
                    
                    order_stats_1.push_back({data_1[curr_day].first,"SELL",to_string(quant_2),to_string(data_1[curr_day].second)});
                    
                    cash_in_hand-=data_2[curr_day].second*(-1*quant_1);
                    
                    order_stats_2.push_back({data_2[curr_day].first,"BUY",to_string(quant_2),to_string(data_2[curr_day].second)});
                    daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
                }
            }
            else{
                daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
            }
            
        }
        
        else if(curr_pos=="bought"){
        	int ch = 0;
        	int f = 0;
        	
        	
            for(int i = 0;i<int(positions.size());i++){
                db temp_z_score = (spread[curr_day] - positions[i].mean)/positions[i].std;
                if(temp_z_score<(-1*stop_loss_threshold)){
                    to_be_removed[i] = true;
                    quant_1--;
                    ch--;
                    quant_2++;
                    f = 1;
                    
                }
                
                
                
                
            }
            
            
            for(int i = int(positions.size())-1;i>=0;i--){
            	
                if(to_be_removed[i]){
                    positions.erase(positions.begin()+i);
                   
                }
                
            }
            
            
            if(quant_1!=0){
                if(quant_1>0){
                    cash_in_hand-=(data_1[curr_day].second*(quant_1));
                    
                    order_stats_1.push_back({data_1[curr_day].first,"BUY",to_string(quant_1),to_string(data_1[curr_day].second)});
                    cash_in_hand+=data_2[curr_day].second*(quant_1);
                    
                    order_stats_2.push_back({data_2[curr_day].first,"SELL",to_string(quant_1),to_string(data_2[curr_day].second)});
                    daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
                    
                }
                else{
                    cash_in_hand+=data_1[curr_day].second*(-1*quant_1);
                    
                    order_stats_1.push_back({data_1[curr_day].first,"SELL",to_string(quant_2),to_string(data_1[curr_day].second)});
                    
                    cash_in_hand-=data_2[curr_day].second*(-1*quant_1);
                    
                    order_stats_2.push_back({data_2[curr_day].first,"BUY",to_string(quant_2),to_string(data_2[curr_day].second)});
                    daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
                }
            }
            else{
                daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
            }
            
        }
       
        else{
        	int ch = 0;
        	int f = 0;
        	
        	
            for(int i = 0;i<int(positions.size());i++){
            
                db temp_z_score = (spread[curr_day] - positions[i].mean)/positions[i].std;
                if(temp_z_score>(stop_loss_threshold)){
                    to_be_removed[i] = true;
                    quant_1++;
                    ch++;
                    f = 1;
                    quant_2--;
                    
                }
                
            }
            
            
            for(int i = int(positions.size())-1;i>=0;i--){
                if(to_be_removed[i]){
                    positions.erase(positions.begin()+i);
                    
                }
            }
            
            
            if(quant_1!=0){
                if(quant_1>0){
                    cash_in_hand-=data_1[curr_day].second *(quant_1);
                    
                    order_stats_1.push_back({data_1[curr_day].first,"BUY",to_string(quant_1),to_string(data_1[curr_day].second)});
                    cash_in_hand+=data_2[curr_day].second*(quant_1);
                    
                    order_stats_2.push_back({data_2[curr_day].first,"SELL",to_string(quant_1),to_string(data_2[curr_day].second)});
                    daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
                    
                }
                else{
                    cash_in_hand+=data_1[curr_day].second*(-1*quant_1);
                    
                    order_stats_1.push_back({data_1[curr_day].first,"SELL",to_string(quant_2),to_string(data_1[curr_day].second)});
                    
                    cash_in_hand-=data_2[curr_day].second*(-1*quant_1);
                    
                    order_stats_2.push_back({data_2[curr_day].first,"BUY",to_string(quant_2),to_string(data_2[curr_day].second)});
                    daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
                }
            }
            else{
                daily_cashflow.push_back({data_1[curr_day].first,to_string(cash_in_hand)});
            }
        }
        hold_quantity_1+=quant_1;
        hold_quantity_2+=quant_2;
        
        
        
        
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
    res_file<<to_string(cash_in_hand)<<endl;
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
    db stop_loss_threshold  = stod(argv[6]);
    string start_date = argv[7];
    string end_date = argv[8];
    start_date = convert_to_y(start_date);
    end_date = convert_to_y(end_date);
    vector<pair<string,db>>data_1;
    vector<pair<string,db>>data_2;
    vector<pair<string,string>>daily_cashflow;
    vector<vector<string>>order_stats_1;
    vector<vector<string>>order_stats_2;
    get_data(data_1,data_2,symbol_1,symbol_2);
    solve(symbol_1,symbol_2,x,n,threshold,stop_loss_threshold,start_date,end_date, data_1,data_2,daily_cashflow,order_stats_1,order_stats_2);
    write_data(daily_cashflow, order_stats_1, order_stats_2);
    
    
    
    return 0;
}



