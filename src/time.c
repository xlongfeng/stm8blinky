#include "jiffies.h"

volatile uint32_t jiffies = 0;

void tick(void)
{
    jiffies++;
}

/*
 * Convert jiffies to milliseconds and back.
 *
 * Avoid unnecessary multiplications/divisions in the
 * two most common HZ cases:
 */
inline uint32_t jiffies_to_msecs(const uint32_t j)
{
#if HZ <= MSEC_PER_SEC && !(MSEC_PER_SEC % HZ)
    return (MSEC_PER_SEC / HZ) * j;
#elif HZ > MSEC_PER_SEC && !(HZ % MSEC_PER_SEC)
    return (j + (HZ / MSEC_PER_SEC) - 1)/(HZ / MSEC_PER_SEC);
#endif
}

inline uint32_t jiffies_to_usecs(const uint32_t j)
{
#if HZ <= USEC_PER_SEC && !(USEC_PER_SEC % HZ)
    return (USEC_PER_SEC / HZ) * j;
#elif HZ > USEC_PER_SEC && !(HZ % USEC_PER_SEC)
    return (j + (HZ / USEC_PER_SEC) - 1)/(HZ / USEC_PER_SEC);
#endif
}

uint32_t msecs_to_jiffies(const uint32_t m)
{
    /*
     * Negative value, means infinite timeout:
     */
    if ((int32_t)m < 0)
        return MAX_JIFFY_OFFSET;

    /*
     * HZ is equal to or smaller than 1000, and 1000 is a nice
     * round multiple of HZ, divide with the factor between them,
     * but round upwards:
     */
    return (m + (MSEC_PER_SEC / HZ) - 1) / (MSEC_PER_SEC / HZ);
}


uint32_t usecs_to_jiffies(const uint32_t u)
{
    if (u > jiffies_to_usecs(MAX_JIFFY_OFFSET))
        return MAX_JIFFY_OFFSET;

    return (u + (USEC_PER_SEC / HZ) - 1) / (USEC_PER_SEC / HZ);
}
