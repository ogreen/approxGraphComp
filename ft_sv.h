#ifdef __cplusplus
extern "C" {
#endif

int SCstep_Sync(size_t nv,
                uint32_t* cc_curr, uint32_t* cc_prev,
                uint32_t* m_curr, uint32_t* m_prev,
                uint32_t* off, uint32_t* ind);


int FISVSweep_Sync(size_t nv, uint32_t* cc_prev, uint32_t* cc_curr,
                           uint32_t* m_curr, uint32_t* off, uint32_t* ind,
                           uint32_t* FaultArrEdge,         /*probability of bit flip*/
                           uint32_t* FaultArrCC         /*probability of bit flip for type-2 faults*/
                          );

lp_state_t SCSVAlg_Sync( graph_t *graph,
                         stat_t* stat,       /*for counting stats of each iteration*/
                         int max_iter        /*contgrolling maximum number of iteration*/
                       );

int  TMSVSweep_Async(graph_t *graph, lp_state_t *lp_state,
                     uint32_t* FaultArrEdge,         /*probability of bit flip*/
                     uint32_t* FaultArrCC );        /*probability of bit flip for type-2 faults*/


int  TMSVSweep_Sync(graph_t *graph, 
                     lp_state_t* lp_state_in,      // State which is read in the iteration
                     lp_state_t* lp_state_out,     // State which is written to in the iteration     
                     uint32_t* FaultArrEdge,       /*probability of bit flip*/
                     uint32_t* FaultArrCC );        /*probability of bit flip for type-2 faults*/

int FISVModAlg_Sync( lp_state_t* lp_state_prev, graph_t *graph,
                     stat_t* stat, int max_iter );

int FISVModAlg_Async( lp_state_t* lp_state_prev, graph_t *graph,
                     stat_t* stat, int max_iter );

#ifdef __cplusplus
}
#endif
