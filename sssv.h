#ifdef __cplusplus
extern "C" {
#endif




int SSstep_Async(graph_t *graph, lp_state_t*lp_state);
int SSstep_Sync(graph_t *graph, lp_state_t*lp_state_out, lp_state_t* lp_state_in );

int SSSVAlg_Async( lp_state_t *lp_state,  graph_t *graph,
                   stat_t* stat, int ssf // frequency of self stabilization
                   , int max_iter
                 );
int SSSVAlg_Sync( lp_state_t* lp_state_prev, graph_t *graph,
                  stat_t* stat, int ssf // frequency of self stabilization
                  , int max_iter
                 );
int FISVAlg_Sync( lp_state_t* lp_state_prev, graph_t *graph,
                  stat_t* stat, int ssf // frequency of self stabilization
                  , int max_iter
                 );

#ifdef __cplusplus
}
#endif