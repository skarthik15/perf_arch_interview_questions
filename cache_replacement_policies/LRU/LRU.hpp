//LRU Implementation header file
//
using namespace std;

struct lru_meta_t {
	uint64_t tag,
	uint64_t dirty,
	uint64_t valid,
};

std::vector<lru_meta_t> lru_list;


int update_lru(uint64_t input_tag, char op );

