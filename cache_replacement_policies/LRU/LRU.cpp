//LRU Implementation source file


#include "LRU.hpp"

using namespace std;

void init(){

    lru_list.resize(NUM_WAYS);
    
    lru_stats.hit_count = 0;
    lru_stats.miss_count = 0;
    lru_stats.wb_evict = 0;
    lru_stats.silent_evict = 0;
    lru_stats.error_count = 0;
}

void age_lines(int target){
    if(target >= 0 && target < lru_list.size()){
        for(int i = target ; i> 0;i-- ){
            lru_list[i] = lru_list[i-1];  
        }
    }
    else{
        lru_stats.error_count++;
    }
}

bool do_assoc_search(uint64_t input_tag){
    bool result = false;
    for(int i=0; i< lru_list.size(); i++){
        if(lru_list[i].valid == 1 && lru_list[i].tag == input_tag){
            hit_line_index = i;
            result = true;
            break;
        }
    }
    return result;
}

int get_num_valid_lines(){
    int valid_lines=0;

    for (int i=0; i<lru_list.size();i++){
        if(lru_list[i].valid == 1){
            valid_lines++;
        }
    }
    return valid_lines;
}

int get_first_empty_slot(){
    int empty_slot;
    int i;
    for(i =0; i<lru_list.size();i++){
        if(lru_list[i].valid == 0){
            break;
        }
    }
    empty_slot = i;
    return empty_slot;
}

void print_lru_array(){
    cout<<"==========================================="<<endl;
    cout<<"WAY#\tTag\tValid\tDirty"<<endl;
    for(int i = 0; i< NUM_WAYS; i++){
        cout<<i<<"\t"<<lru_list[i].tag<<"\t"<<lru_list[i].valid<<"\t"<<lru_list[i].dirty<<endl;
    }
    cout<<"==========================================="<<endl;
}

void print_lru_stats(){

    cout<<"Printing LRU Stats"<<endl;
    cout<<"Hits = "<<lru_stats.hit_count<<endl;
    cout<<"Misses = "<<lru_stats.miss_count<<endl;
    cout<<"WB_Evicts = "<<lru_stats.wb_evict<<endl;
    cout<<"Silent_Evicts = "<<lru_stats.silent_evict<<endl;
}

void update_lru(uint64_t input_tag, char op){
	
    lru_meta_t temp;
    temp.tag = input_tag;
    temp.valid = true;
    if(op == WRITE){
        temp.dirty = true;
    }
    else{
        temp.dirty = false;
    }
    //Hit
	if(do_assoc_search(input_tag)){
        lru_stats.hit_count++;
        //Age the other lines
        age_lines(hit_line_index);
        lru_list[0] = temp;
    }
    //Miss
    else{
        lru_stats.miss_count++;
        if(get_num_valid_lines() < NUM_WAYS){
            //Age all the lines 
            age_lines(get_first_empty_slot());
            lru_list[0] = temp;
        }
        else if(get_num_valid_lines() == NUM_WAYS){
            //Check if LRU line is dirty
            if(lru_list[NUM_WAYS-1].dirty == true){
                lru_stats.wb_evict++;
            }
            else{
                lru_stats.silent_evict++;
            }
            age_lines(NUM_WAYS-1);
            lru_list[0] = temp;
        }
    }
    print_lru_array();
}

int main(){
    init();
    for (int addr = 0; addr < 16 ; addr++){
        update_lru(addr, WRITE);
    }
    print_lru_stats();

    return 0;
}





