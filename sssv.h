#ifdef __cplusplus
extern "C" {
#endif




int SSstep_Async(graph_t *graph, lp_state_t*lp_state);
int SSstep_Sync(graph_t *graph, lp_state_t*lp_state_out, lp_state_t* lp_state_in );

#ifdef __cplusplus
}
#endif