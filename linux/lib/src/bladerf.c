#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "bladeRF.h"        /* Driver interface */
#include "libbladeRF.h"     /* API */
#include "bladerf_priv.h"   /* Implementation-specific items ("private") */
#include "debug.h"

static inline size_t min_sz(size_t x, size_t y)
{
    return x < y ? x : y;
}

int bladerf_set_loopback(struct bladerf *dev, bladerf_loopback l)
{
    lms_loopback_enable( dev, l ) ;
    return 0;
}

int bladerf_set_sample_rate(struct bladerf *dev, bladerf_module module, unsigned int rate, unsigned int *actual)
{
    /* TODO: Program the Si5338 to be 2x the desired sample rate */
    int ret = -1;
    /* TODO: Use module to pick the correct clock output to change */
    if( module == TX ) {
        ret = si5338_set_tx_freq(dev, rate);
    } else {
        ret = si5338_set_rx_freq(dev, rate);
    }
    *actual = rate;
    return ret;
}

int bladerf_set_rational_sample_rate(struct bladerf *dev, bladerf_module module, unsigned int integer, unsigned int num, unsigned int denom)
{
    /* TODO: Program the Si5338 to be 2x the desired sample rate */
    return 0;
}

int bladerf_get_sample_rate( struct bladerf *dev, bladerf_module module, unsigned int *rate)
{
    /* TODO: Read the Si5338 and figure out the sample rate */
    return 0 ;
}

int bladerf_get_rational_sample_rate(struct bladerf *dev, bladerf_module module, unsigned int integer, unsigned int num, unsigned int denom)
{
    /* TODO: Read the Si5338 and figure out the sample rate */
    return 0 ;
}

int bladerf_set_txvga2(struct bladerf *dev, int gain)
{
    if( gain > 25 ) {
        fprintf( stderr, "%s: %d being clamped to 25dB\n", __FUNCTION__, gain ) ;
        gain = 25 ;
    }
    if( gain < 0 ) {
        fprintf( stderr, "%s: %d being clamped to 0dB\n", __FUNCTION__, gain ) ;
        gain = 0 ;
    }
    /* TODO: Make return values for lms call and return it for failure */
    lms_txvga2_set_gain( dev, gain ) ;
    return 0;
}

int bladerf_get_txvga2(struct bladerf *dev, int *gain)
{
    *gain = 0 ;
    /* TODO: Make return values for lms call and return it for failure */
    lms_txvga2_get_gain( dev, (uint8_t *)gain ) ;
    return 0;
}

int bladerf_set_txvga1(struct bladerf *dev, int gain)
{
    if( gain < -35 ) {
        fprintf( stderr, "%s: %d being clamped to -35dB\n", __FUNCTION__, gain ) ;
        gain = -35 ;
    }
    if( gain > -4 ) {
        fprintf( stderr, "%s: %d being clamped to -4dB\n", __FUNCTION__, gain ) ;
        gain = -4 ;
    }
    /* TODO: Make return values for lms call and return it for failure */
    lms_txvga1_set_gain( dev, gain ) ;
    return 0;
}

int bladerf_get_txvga1(struct bladerf *dev, int *gain)
{
    *gain = 0 ;
    /* TODO: Make return values for lms call and return it for failure */
    lms_txvga1_get_gain( dev, (int8_t *)gain ) ;
    return 0;
}

int bladerf_set_lna_gain(struct bladerf *dev, bladerf_lna_gain gain)
{
    /* TODO: Make return values for lms call and return it for failure */
    lms_lna_set_gain( dev, gain ) ;
    return 0;
}

int bladerf_get_lna_gain(struct bladerf *dev, bladerf_lna_gain *gain)
{
    /* TODO: Make return values for lms call and return it for failure */
    lms_lna_get_gain( dev, gain ) ;
    return 0 ;
}

int bladerf_set_rxvga1(struct bladerf *dev, int gain)
{
    /* TODO: Make return values for lms call and return it for failure */
    lms_rxvga1_set_gain( dev, (uint8_t)gain ) ;
    return 0;
}

int bladerf_get_rxvga1(struct bladerf *dev, int *gain)
{
    *gain = 0 ;
    /* TODO: Make return values for lms call and return it for failure */
    lms_rxvga1_get_gain( dev, (uint8_t *)gain ) ;
    return 0 ;
}

int bladerf_set_rxvga2(struct bladerf *dev, int gain)
{
    /* TODO: Make return values for lms call and return it for failure */
    lms_rxvga2_set_gain( dev, (uint8_t)gain ) ;
    return 0;
}

int bladerf_get_rxvga2(struct bladerf *dev, int *gain)
{
    *gain = 0 ;
    /* TODO: Make return values for lms call and return it for failure */
    lms_rxvga2_get_gain( dev, (uint8_t *)gain ) ;
    return 0 ;
}

int bladerf_set_bandwidth(struct bladerf *dev, bladerf_module module,
                            unsigned int bandwidth,
                            unsigned int *actual)
{
    /* TODO: Make return values for lms call and return it for failure */
    lms_bw_t bw = lms_uint2bw(bandwidth) ;
    lms_lpf_enable( dev, module, bw ) ;
    *actual = lms_bw2uint(bw) ;
    return 0;
}

int bladerf_get_bandwidth(struct bladerf *dev, bladerf_module module,
                            unsigned int *bandwidth )
{
    /* TODO: Make return values for lms call and return it for failure */
    lms_bw_t bw = lms_get_bandwidth( dev, module ) ;
    *bandwidth = lms_bw2uint(bw) ;
    return 0 ;
}

int bladerf_set_frequency(struct bladerf *dev,
                            bladerf_module module, unsigned int frequency)
{
    /* TODO: Make return values for lms call and return it for failure */
    lms_set_frequency( dev, module, frequency ) ;
    return 0;
}

int bladerf_get_frequency(struct bladerf *dev,
                            bladerf_module module, unsigned int *frequency)
{
    /* TODO: Make return values for lms call and return it for failure */
    struct lms_freq f ;
    lms_get_frequency( dev, module, &f ) ;
    *frequency = (unsigned int)(((uint64_t)((f.nint<<23) + f.nfrac)) * (f.reference/f.x) >>23) ;
    return 0 ;
}

/*------------------------------------------------------------------------------
 * Misc.
 *----------------------------------------------------------------------------*/

const char * bladerf_strerror(int error)
{
    switch (error) {
        case BLADERF_ERR_UNEXPECTED:
            return "An unexpected error occurred";
        case BLADERF_ERR_RANGE:
            return "Provided parameter was out of the allowable range";
        case BLADERF_ERR_INVAL:
            return "Invalid operation or parameter";
        case BLADERF_ERR_MEM:
            return "A memory allocation error occurred";
        case BLADERF_ERR_IO:
            return "File or device I/O failure";
        case BLADERF_ERR_TIMEOUT:
            return "Operation timed out";
        case 0:
            return "Success";
        default:
            return "Unknown error code";
    }
}

