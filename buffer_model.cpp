#include <stdlib.h>
#include <vector>
#include <queue>
#include <iostream>
using namespace std;
#define SRC_ISSUANCE_RATE  0.8
#define SRC_MAX_OUTSTANDING 80

#define DEST_ACCEPTANCE_RATE 0.8
#define DEST_SINK_DEPTH 8


#define TOTAL_NUM_TXNS 1000
#define TOTAL_SIM_CYCLES 1000
//Global Variables

unsigned long global_clk = 0;
unsigned long global_txn_count = 0;
unsigned long global_txns_sunk = 0;

std::queue<unsigned long> buffer_model;
int max_num_txns = 0;
int num_txns_issued = 0;
int num_txns_sunk = 0;
int src_throttle = 0;
int dest_throttle = 0;
int max_idle_cycles;
int idle_cycles;
int max_dest_idle_cycles;
int dest_idle_cycles;
int burst_length;



void dump_buffer_model(){
	cout<<global_clk<<" DEBUG : Buffer size = "<<buffer_model.size()<<endl;
//	cout<<global_clk<<" DEBUG : Dumping Buffer Model "<<endl;
//	for(unsigned long i = 0; i< buffer_model.size(); i++){
//		cout<<global_clk<<" DEBUG  buffer_entry "<<i<<" : "<<buffer_model[i]<<endl;
//	}
}

void process_txn(){

	unsigned long long sink_txn_id;

	cout<<"---------------Begin Clock "<<global_clk<<" ----------------------"<<endl;


	//End of Phase 2, reset the 
	if(dest_idle_cycles == 0 && dest_throttle == 1){
		cout<<global_clk<<" : DEST : Dest Idle Cycles done, clearing Dest_throttle"<<endl;
		dest_idle_cycles = max_dest_idle_cycles;
		num_txns_sunk = 0;
		dest_throttle = 0;
	}

	if(dest_idle_cycles > 0 && dest_throttle == 1){
		dest_idle_cycles--;
		cout<<global_clk<<" : DEST : Throttled. dest_idle_cycles count = "<<dest_idle_cycles<<endl;

	}
	//Phase 2a: Pop an entry from the buffer mode if the buffer model has an entry 
	if(num_txns_sunk < DEST_SINK_DEPTH && buffer_model.size()>0 && dest_throttle == 0){
		cout<<global_clk<<" : DEST : Buffer size : "<<buffer_model.size()<<endl;
		sink_txn_id = buffer_model.front();
		buffer_model.pop();
		cout<<global_clk<<" : DEST : Popped Buffer. Sink Txn ID "<<sink_txn_id<<endl;
		num_txns_sunk++;
		global_txns_sunk ++;
		cout<<global_clk<<" : DEST : global_txns_sunk "<<global_txns_sunk<<endl;

	}
	//Phase 2b : If the sink depth has been reached, then idle
	if(num_txns_sunk == DEST_SINK_DEPTH){
		dest_throttle = 1;
		cout<<global_clk<<" : DEST : DET_SINK_DEPTH Reached, dest_throttle is "<<src_throttle<<endl;

	}






	//SRC ----------------------
	if(idle_cycles == 0 && src_throttle == 1){
		cout<<global_clk<<" : SRC : src Idle Cycles done, clearing src_throttle"<<endl;
		idle_cycles = max_idle_cycles;
		src_throttle = 0;
		num_txns_issued = 0;
	
	}

	//Phase 1b : check if max outstanding has been issued and throttle
	if( idle_cycles >0 && src_throttle == 1){
		idle_cycles--;
		cout<<global_clk<<" : SRC : Throttled. idle_cycle count = "<<idle_cycles<<endl;

	}
	//phase 1a : Issue transaction as long as it is less than the max_num_txns count
	if(global_txn_count < max_num_txns){
		if(num_txns_issued < SRC_MAX_OUTSTANDING && src_throttle == 0 ) {
			buffer_model.push(global_clk);
			dump_buffer_model();
			cout<<global_clk<<" : SRC : Pushing Txn ID "<<global_clk<<endl;
			num_txns_issued++;
			global_txn_count ++;
			cout<<global_clk<<" : SRC : global_txn_issued "<<global_txn_count<<endl;
	
		}
		if(num_txns_issued == SRC_MAX_OUTSTANDING ){
			//Reset num_txns_issued
			src_throttle = 1;
			cout<<global_clk<<" : SRC : SRC_MAX_OUTSTANDING Reached, Setting  Src_throttle  "<<src_throttle<<endl;
	
		}
	}	
	if(global_txn_count == max_num_txns){
		cout<<global_clk<<" : SRC : Max_num_txns have been issued. Now the system shall drain"<<endl;
	}



	cout<<"---------------End Clock "<<global_clk<<" ----------------------"<<endl;
	global_clk++;
	

}

int main(){

	burst_length = SRC_MAX_OUTSTANDING/SRC_ISSUANCE_RATE;
	max_idle_cycles = burst_length - SRC_MAX_OUTSTANDING;
        idle_cycles = max_idle_cycles;
	max_dest_idle_cycles = (DEST_SINK_DEPTH/DEST_ACCEPTANCE_RATE) - DEST_SINK_DEPTH; 
	dest_idle_cycles = max_dest_idle_cycles;

	max_num_txns = 100;
	int num_txns = 0;
	bool system_quiesced = false;
	while(!system_quiesced){
		cout<<"MAIN : calling process_txn() txn_number  "<<num_txns<<endl;
		process_txn();
		num_txns++;
		system_quiesced = (global_txn_count == max_num_txns) && (global_txns_sunk == max_num_txns) && (buffer_model.size() == 0); 
		if(system_quiesced){
			cout<<global_clk<<" : MAIN : System Quiesced. Ending Sim"<<endl;
		}	
	}

}
