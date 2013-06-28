#include <libbladeRF.h>

int main(int argc, char *argv[]) {
    int numsam, numread;
    unsigned short *rb;
    unsigned char *buf;
    int bufidx;

    struct bladerf *dev = bladerf_open("/dev/bladerf1");
    if (!dev) {
        printf( "Couldn't open device\n" ) ;
        return 0;
    }
    printf("Is bladerf_is_fpga_configured = %d\n",bladerf_is_fpga_configured(dev));

    bladerf_load_fpga(dev, "bladerf.rbf");

    gpio_write( dev, 0x4f);
    si5338_set_rx_freq(dev, 40E6);
    lms_set_frequency( dev, RX, 314500000 ) ;
    lms_rxvga2_set_gain( dev, 25 ) ;
    lms_spi_write( dev, 0x59, 0x09 ) ;
    lms_rx_enable( dev );

//    si5338_set_tx_freq(dev, 40E6);
//    lms_set_frequency( dev, TX, 314000000 ) ;
//    lms_lpf_enable( dev, TX, BW_28MHz ) ;
//    lms_pa_enable( dev, PA_1 ) ;
    lms_tx_disable( dev );

    numsam = atoi(argv[1]);
    numread = (numsam + 1023) / 1024;
    numsam = numread * 1024;

    buf = malloc(4 * numsam);

    int nread, j,x ;
    memset(buf, 0, 2048);

    bufidx = 0;

    printf( "Reading...\n" ) ;
/*    for (x = 0; x < numread; x++) {
        nread = read(dev->fd, &buf[bufidx], 1024*4);
        bufidx += nread;
    }*/
    printf( "Done!\n" ) ;
    rb = (unsigned short *)buf;
    short i, q;
    for (j = 0; j < numsam; j++) {


        q = *rb++;
        i = *rb++;
        //printf("i %.4hx, q %.4hx\n", i, q);

        if (((i & 0xf000) >> 12) != 0xb)
            printf("What the fuck is wrong with i %x %x\n", i & 0xf000, (i & 0xf000) >> 12);
        if (((q & 0xf000) >> 12) != 0x3)
            printf("What the fuck is wrong with q %x %x\n", q & 0xf000, (q & 0xf000) >> 12);

        i &= 0xfff;
        q &= 0xfff;

        if (i & 0x800)
            i |= 0xf000;
        if (q & 0x800)
            q |= 0xf000;
        printf("%d, %d\n", (int)i, (int)q);
    }
    return 0 ;
}
