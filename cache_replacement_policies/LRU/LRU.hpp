//LRU Implementation header file
//Implementation uses a vector of cachelines.
//Index 0 : MRU Line
//Index Size - 1 : LRU Line
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cstdint>

#define NUM_WAYS 8
#define MRU_POS 0
#define WRITE 'w'
#define READ 'r'
using namespace std;

struct lru_meta_t {
	uint64_t tag;
	bool dirty;
	bool valid;
};

struct lru_stats_t  {
	uint64_t hit_count;
	uint64_t miss_count;
	uint64_t wb_evict;
	uint64_t silent_evict;
	uint64_t error_count;
};

std::vector<lru_meta_t> lru_list;

uint64_t hit_line_index;

lru_stats_t lru_stats;

void init();

void update_lru(uint64_t input_tag, char op );

bool do_assoc_search(uint64_t input_tag);


// 0 =< target < lru_list.size()
void age_lines(int target);

void print_lru_array();

void print_lru_stats();

//This function checks to ensure that lines that 
//have the valid bit = 0, are only towards the end of the list
//0 0 0 0 0 0 0 sane
//1 0 0 0 0 0 0 sane
//1 1 0 0 0 0 0 sane
//1 1 1 0 0 0 0 sane
//1 0 1 1 0 1 0 Insane!!!!
void sanity_contigious_valid_check();