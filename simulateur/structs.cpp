#include "structs.h" 


volatile bool refresh = false;

ClockTicks clockticks_new() {
    ClockTicks ct;
    ct.click_constant = 0;
    ct.wmem_t = 0;
    ct.add_t = 0;
    ct.sub_t = 0;
    ct.snif_t = 0;
    ct.and_t = 0;
    ct.or_t = 0;
    ct.xor_t = 0;
    ct.lsl_t = 0;
    ct.lsr_t = 0;
    ct.asr_t = 0;
    ct.call_t = 0;
    ct.jump_t = 0;
    ct.return_t = 0;
    ct.letl_t = 0;
    ct.leth_t = 0;
    ct.copy_t = 0;
    ct.rmem_t = 0;
    return ct;
}
