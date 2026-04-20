#include "efload_app/startup_sequence.h"

bool startup_delay_elapsed(uint32_t now_us, uint32_t entered_us, uint32_t delay_us)
{
    return (uint32_t)(now_us - entered_us) >= delay_us;
}

