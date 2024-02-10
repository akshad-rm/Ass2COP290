#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#define db double

using namespace std;


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

void get_data(vector<pair<string,vector<db>>>&data){
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

void solve(int p,int x,vector<pair<string,vector<db>>>&data,vector<pair<string,string>>&daily_cashflow,vector<vector<string>>&order_stats,vector<db> &betas){
    
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
    string symbol = "SBIN";
    int p = 2;
    int x = 3;
    vector<pair<string,vector<db>>>data;
    vector<pair<string,string>>daily_cashflow;
    vector<vector<string>>order_stats;
    vector<pair<string,vector<db>>>final_data;
    get_training_data(data);
    vector<db> betas = get_betas(data);
    get_data(final_data);
    solve(p,x,final_data,daily_cashflow,order_stats,betas);
    write_data(daily_cashflow,order_stats);
    return 0;
}
