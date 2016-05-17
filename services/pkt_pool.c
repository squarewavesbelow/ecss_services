#include "pkt_pool.h"


#undef __FILE_ID__
#define __FILE_ID__ 10

struct _pkt_pool pkt_pool;

tc_tm_pkt * get_pkt() {

    for(uint8_t i = 0; i < POOL_PKT_SIZE; i++) {
        if(pkt_pool.free[i] == true) {
            pkt_pool.free[i] = false;
            pkt_pool.time[i] = HAL_sys_GetTick();
            pkt_pool.pkt[i].verification_state = SATR_PKT_INIT;
            return &pkt_pool.pkt[i];
        }
    }

    return NULL;
}

extern uint8_t uart_temp[];

SAT_returnState free_pkt(tc_tm_pkt *pkt) {

    if(!C_ASSERT(pkt != NULL) == true) { return SATR_ERROR; }

    for(uint8_t i = 0; i < POOL_PKT_SIZE; i++) {
        if(&pkt_pool.pkt[i] == pkt) {
            pkt_pool.free[i] = true;
            uint32_t t = HAL_sys_GetTick() - pkt_pool.time[i];
            snprintf(uart_temp, 400, "Time %d: %d, %d, %d\n", i, t, pkt->ser_type, pkt->ser_subtype);
            HAL_uart_tx(DBG_APP_ID, (uint8_t *)uart_temp, strlen(uart_temp));
            return SATR_OK;
        }
    }
    return SATR_ERROR;
}

SAT_returnState pkt_pool_INIT() {

    for(uint8_t i = 0; i < POOL_PKT_SIZE; i++) {
       pkt_pool.free[i] = true;
       pkt_pool.pkt[i].data = &pkt_pool.data[i][0];
    }
    return SATR_OK;
}

void pkt_pool_GC() {

    for(uint8_t i = 0; i < POOL_PKT_SIZE; i++) {
        if(pkt_pool.free[i] == false && pkt_pool.time[i] - time_now() > PKT_TIMEOUT) {
            //pkt_pool.free[i] = 0;
            // error
        }
    }
}
