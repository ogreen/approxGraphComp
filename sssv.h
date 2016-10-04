#ifdef __cplusplus
extern "C" {
#endif




typedef struct
{

    // mystack S;
    int global_index;
    int* vind;
    int* vlowlink;
    int* vonstack;

} ts_t; // tarzen structure

void alloc_ts (int nv, ts_t *ts);
void init_ts(int nv, ts_t *ts);

void free_ts(ts_t *ts);

int strongConnected(int ii, graph_t *graph, lp_state_t  *lp_state,
                    ts_t *ts);

int shortcut_LP(graph_t *graph, lp_state_t*lp_state, ts_t *ts);

int selfStab_LP(graph_t *graph, lp_state_t*lp_state, ts_t *ts);

#ifdef __cplusplus
}
#endif