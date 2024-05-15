#define MEM_SIZE (32768)
void mem_init(long mem[MEM_SIZE]);
long mem_get(long request);
void mem_free(long index);
double mem_internal();
void mem_available(long *empty, long *large, long *n_holes);
void mem_exit();
