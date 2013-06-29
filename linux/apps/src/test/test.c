#include <libbladeRF.h>
#include <sys/time.h>

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
	if (!bladerf_is_fpga_configured(dev))
	{
		bladerf_load_fpga(dev,"f:\\downloads\\hosted(1).rbf");
	}
	gpio_write( dev, 0x4f);
    si5338_set_rx_freq(dev, 5E6);
	lms_set_frequency( dev, RX, 866500000 ) ;
	lms_rx_enable(dev);
	lms_tx_disable(dev);
	bladerf_StartRX(dev);
	lms_spi_write(dev, 0x59, 0x29);

//    si5338_set_tx_freq(dev, 40E6);
//    lms_set_frequency( dev, TX, 314000000 ) ;
    //lms_lpf_enable( dev, TX, BW_28MHz ) ;
//    lms_pa_enable( dev, PA_1 ) ;
/*    lms_tx_disable( dev );
	lms_dump_registers(dev);
	bladerf_StartRX(dev);*/
#define BlockSize (8192)
    numsam = 16384; //atoi(argv[1]);
    numread = (numsam + (BlockSize-1)) / BlockSize;
    numsam = numread * BlockSize;

    buf = malloc(4 * numsam);

    int nread, j,x ;
    memset(buf, 0, 2048);

    bufidx = 0;
	//bladerf_StartRX(dev);
	//bladerf_setInterface(dev,1);

	struct timeval begin, end;
	int loops = 100;
    printf( "Reading...\n" ) ;
	nread=0;
	double totalTime=0;
	double totalSamples=0;
    gettimeofday(&begin, NULL);
	nread=0;
	while(true)
	{
		nread += bladerf_read_c16(dev,(int16_t*) &buf[0] , BlockSize);
		gettimeofday(&end, NULL);
		double timesec =  (double)(end.tv_sec - begin.tv_sec);
		double timeusec = (double)(end.tv_usec - begin.tv_usec);
		double time = timesec + (timeusec / 1000000.0 ); 
		if (time >=1)
		{
			double SamplesSec = (1 * nread)/ time;
			totalTime+=time;
			totalSamples += nread;
			double SamplesSecTotal = totalSamples / totalTime;
			printf("Samples %d time = %f samp/sec = %f global:%f\n",nread,time,SamplesSec,SamplesSecTotal);
			gettimeofday(&begin, NULL);
			nread=0;
		}
        
    }
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
        //printf("%d, %d\n", (int)i, (int)q);
    }
	bladerf_close(dev);
	printf("Exit\n");
    return 0 ;
}
