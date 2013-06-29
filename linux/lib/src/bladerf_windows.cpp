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
#include <windows.h>
#include <cyapi.h>


extern "C"
{

CCyBulkEndPoint *GetEndPoint(struct bladerf *dev,int id)
{
	CCyUSBDevice *USBDevice = (CCyUSBDevice *) dev->Device;
	int eptCount = USBDevice->EndPointCount();
	for (int i=1; i<eptCount; i++) 
	{
		 if (USBDevice->EndPoints[i]->Address == id)
			 return (CCyBulkEndPoint *)USBDevice->EndPoints[i]; 
	}
	return NULL;
}


/*******************************************************************************
 * Device discovery & initialization/deinitialization
 ******************************************************************************/

GUID Blade1Guid = {0xd7d516b1,0xbf0e,0x4f1e,0x8e,0x0f,0xdb,0x96,0xb8,0x89,0xbc,0xe2};
struct bladerf * _bladerf_open_info(const char *dev_path,
                                    struct bladerf_devinfo *i)
{
        return NULL;

}


ssize_t bladerf_get_device_list(struct bladerf_devinfo **devices)
{
    return 1;
}

void bladerf_free_device_list(struct bladerf_devinfo *devices, size_t n)
{
}

struct bladerf * bladerf_open(const char *dev_path)
{
	struct bladerf *result = (struct bladerf *)malloc(sizeof(struct bladerf));
	CCyUSBDevice *USBDevice = new CCyUSBDevice(NULL, Blade1Guid); // Does not register for PnP events
	result->Device = USBDevice;
	
	//bladerf_setInterface(result,1);
    return result;
}

void bladerf_close(struct bladerf *dev)
{
	CCyUSBDevice *USBDevice = (CCyUSBDevice *) dev->Device;
	USBDevice->Close();
}

/*******************************************************************************
 * Data transmission and reception
 ******************************************************************************/

ssize_t bladerf_send_c12(struct bladerf *dev, int16_t *samples, size_t n)
{
    return 0;
}

ssize_t bladerf_send_c16(struct bladerf *dev, int16_t *samples, size_t n)
{
    return 0;
}

ssize_t bladerf_read_c16(struct bladerf *dev,
                            int16_t *samples, size_t max_samples)
{
     long size = max_samples *4;
     CCyBulkEndPoint *ep81 =NULL;
     ep81 = GetEndPoint(dev,0x81);
     bool res = ep81->XferData((PUCHAR)samples,size);
     if (res)
       return size/4;
     else
       return 0;
}
/* TODO - Devices do not currently support serials */
int bladerf_get_serial(struct bladerf *dev, uint64_t *serial)
{
    assert(dev && serial);

    *serial = 0;
    return 0;
}


bool bladerf_vendorcommand_i4(struct bladerf *dev,int Command, int *Result)
{
    assert(dev);

	CCyUSBDevice *USBDevice = (CCyUSBDevice *) dev->Device;

	USBDevice->ControlEndPt->ReqType =  ( CTL_XFER_REQ_TYPE ) (REQ_VENDOR);
	USBDevice->ControlEndPt->ReqCode = Command;
	USBDevice->ControlEndPt->Index = 0;
	LONG Len =4;
	bool res = USBDevice->ControlEndPt->Read((PUCHAR)Result,Len);
    return res;

}


bool bladerf_vendorcommand_write_i4(struct bladerf *dev,int Command, int value)
{
    assert(dev);

	CCyUSBDevice *USBDevice = (CCyUSBDevice *) dev->Device;

	USBDevice->ControlEndPt->ReqType =  ( CTL_XFER_REQ_TYPE ) (REQ_VENDOR);
	USBDevice->ControlEndPt->ReqCode = Command;
	USBDevice->ControlEndPt->Index = 0;
	LONG Len =4;
	bool res = USBDevice->ControlEndPt->Write((PUCHAR)&value,Len);
    return res;

}


bool bladerf_getInterface(struct bladerf *dev,UCHAR *interfaceID)
{
    assert(dev);

	CCyUSBDevice *USBDevice = (CCyUSBDevice *) dev->Device;
	USBDevice->ControlEndPt->ReqType =  REQ_STD;
	USBDevice->ControlEndPt->ReqCode = 10;
	USBDevice->ControlEndPt->Index = 0;
	USBDevice->ControlEndPt->Target = TGT_INTFC;
	LONG Len =1;
	bool res = USBDevice->ControlEndPt->Read((PUCHAR) interfaceID,Len);
    return res;
}

bool bladerf_setInterface(struct bladerf *dev,int interfaceID)
{
    assert(dev);

	UCHAR CurrentID = 99;
	/*bladerf_getInterface(dev, &CurrentID);
	if (CurrentID == interfaceID)
		return true;*/

	CCyUSBDevice *USBDevice = (CCyUSBDevice *) dev->Device;
	USBDevice->ControlEndPt->ReqType =  REQ_STD;
	USBDevice->ControlEndPt->ReqCode = 11;
	USBDevice->ControlEndPt->Index = interfaceID;
	USBDevice->ControlEndPt->Value =0;
	USBDevice->ControlEndPt->Target = TGT_DEVICE;
	//USBDevice->ControlEndPt->Direction = CTL_XFER_DIR_TYPE::DIR_FROM_DEVICE;
	LONG Len =0;
	bool res = USBDevice->ControlEndPt->Write(NULL,Len);
	//CurrentID=99;
	//bladerf_getInterface(dev, &CurrentID);

    return res;
}

int bladerf_is_fpga_configured(struct bladerf *dev)
{
    int result;
	if (bladerf_vendorcommand_i4(dev,BLADE_USB_CMD_QUERY_FPGA_STATUS, &result))
		return result;

	return 0;

}


int bladerf_BeginProgramming(struct bladerf *dev)
{
    int result;
	if (bladerf_vendorcommand_i4(dev,BLADE_USB_CMD_BEGIN_PROG, &result))
		return result;

	return 0;
}

int bladerf_EndProgramming(struct bladerf *dev)
{
    int result;
	if (bladerf_vendorcommand_i4(dev,BLADE_USB_CMD_END_PROG, &result))
		return result;

	return 0;

}


int bladerf_StartRX(struct bladerf *dev)
{
    int result;
	result = 1;
	if (bladerf_vendorcommand_write_i4(dev,BLADE_USB_CMD_RF_RX, result))
		return result;
	
	return 0;

}

/* TODO Not yet supported */
int bladerf_get_fpga_version(struct bladerf *dev,
                                unsigned int *major, unsigned int *minor)
{
    assert(dev && major && minor);

    *major = 0;
    *minor = 0;

    return 0;
}

int bladerf_get_fw_version(struct bladerf *dev,
                            unsigned int *major, unsigned int *minor)
{
    int status;
    struct bladeRF_version ver;

    assert(dev && major && minor);

	CCyUSBDevice *USBDevice = (CCyUSBDevice *) dev->Device;

	USBDevice->ControlEndPt->ReqType =  ( CTL_XFER_REQ_TYPE ) (REQ_VENDOR);
	USBDevice->ControlEndPt->ReqCode = BLADE_USB_CMD_QUERY_VERSION;
	USBDevice->ControlEndPt->Index = 0;
	LONG Len = sizeof(ver);
	bool res = USBDevice->ControlEndPt->Read((PUCHAR)&ver,Len);
	*major = ver.major;
	*minor = ver.minor;
    /* TODO return more appropriate error code based upon errno */
    return BLADERF_ERR_IO;
}

int bladerf_flash_firmware(struct bladerf *dev, const char *firmware)
{
    return 0;
}

#define STACK_BUFFER_SZ 1024
int bladerf_load_fpga(struct bladerf *dev, const char *fpga)
{
	 struct stat stat;

	/* FPGA is already programmed */
	/*if (bladerf_is_fpga_configured(dev))
        return 1;*/
    
	int  fpga_fd = open(fpga, _O_BINARY);
    if (fpga_fd < 0) {
        dbg_printf("Failed to open device (%s): %s\n", fpga, strerror(errno));
        return BLADERF_ERR_IO;
    }

    if (fstat(fpga_fd, &stat) < 0) {
        dbg_printf("Failed to stat fpga file (%s): %s\n", fpga, strerror(errno));
        close(fpga_fd);
        return BLADERF_ERR_IO;
    }

	PUCHAR Buffer = (PUCHAR) malloc(stat.st_size);

	int br  = read(fpga_fd, Buffer, stat.st_size);

	if (br != stat.st_size)
		return BLADERF_ERR_IO;
	bladerf_setInterface(dev,0);
	printf("programming begin :%d\n", 	bladerf_BeginProgramming(dev));
	CCyBulkEndPoint *ep2 = GetEndPoint(dev,0x2);
	long Len = stat.st_size;
	bool res = ep2->XferData(Buffer, Len);
	printf("res = %d transfered = %d\n",res,Len);
	printf("programming end :%d\n", 	bladerf_EndProgramming(dev));
	
	while(true)
	{
		int r = bladerf_is_fpga_configured(dev);
		if (r)
			break;
	}
	bladerf_setInterface(dev,1);
	printf("Score!\n");
	return 0;
}



/*------------------------------------------------------------------------------
 * Si5338 register read / write functions
 */

int si5338_i2c_write(struct bladerf *dev, uint8_t address, uint8_t val)
{
    long count = 16;
    UCHAR buf[16];
    buf[0] = (byte)'N';
	buf[1] = UART_PKT_MODE_DIR_WRITE | UART_PKT_DEV_SI5338 | 0x01;
	buf[2] = address;
    buf[3] = val;
	CCyBulkEndPoint *ep2 = GetEndPoint(dev,0x2);
	CCyBulkEndPoint *ep82 = GetEndPoint(dev,0x82);

	bool res = ep2->XferData(buf,count);
    count=16;
	res = ep82->XferData(buf,count);
    while (!res)
    {
        count = 16;
        res = ep82->XferData(buf,count);
    }
	return res;
}

int si5338_i2c_read(struct bladerf *dev, uint8_t address, uint8_t *val)
{
	long count = 16;
    UCHAR buf[16];
    buf[0] = (byte)'N';
	buf[1] = UART_PKT_MODE_DIR_READ | UART_PKT_DEV_SI5338 | 0x01;
	buf[2] = address;
    buf[3] = 0xff;
	CCyBulkEndPoint *ep2 = GetEndPoint(dev,0x2);
	CCyBulkEndPoint *ep82 = GetEndPoint(dev,0x82);

	bool res = ep2->XferData(buf,count);
    count=16;
	res = ep82->XferData(buf,count);
    while (!res)
    {
        count = 16;
        res = ep82->XferData(buf,count);
    }
	*val = buf[3];
	return res;
}

/*------------------------------------------------------------------------------
 * LMS register read / write functions
 */

int lms_spi_read(struct bladerf *dev, uint8_t address, uint8_t *val)
{
	long count = 16;
    UCHAR buf[16];
    buf[0] = (byte)'N';
	buf[1] = UART_PKT_MODE_DIR_READ | UART_PKT_DEV_LMS | 0x01;
	buf[2] = address;
    buf[3] = 0xff;
	CCyBulkEndPoint *ep2 = GetEndPoint(dev,0x2);
	CCyBulkEndPoint *ep82 = GetEndPoint(dev,0x82);

	bool res = ep2->XferData(buf,count);
    count=16;
	res = ep82->XferData(buf,count);
    while (!res)
    {
        count = 16;
        res = ep82->XferData(buf,count);
    }
	*val = buf[3];
	return res;
}

int lms_spi_write(struct bladerf *dev, uint8_t address, uint8_t val)
{
    long count = 16;
    UCHAR buf[16];
    buf[0] = (byte)'N';
    buf[1] = UART_PKT_MODE_DIR_WRITE | UART_PKT_DEV_LMS | 0x01;
	buf[2] = address;
    buf[3] = val;

	CCyBulkEndPoint *ep2 = GetEndPoint(dev,0x2);
	CCyBulkEndPoint *ep82 = GetEndPoint(dev,0x82);

	bool res = ep2->XferData(buf,count);
    count=16;
	res = ep82->XferData(buf,count);
    while (!res)
    {
        count = 16;
        res = ep82->XferData(buf,count);
    }
	return res;
}

	OVERLAPPED  ov;
	PUCHAR blah;
	short *TmpBuffer[2];
	int CurIdx=0;
CCyBulkEndPoint *ep81 =NULL;

int bladerf_read(struct bladerf *dev, short* Buffer, int size, int *BytesRead)
{
  *BytesRead = size;
  return 0;
}


int bladerf_read2(struct bladerf *dev, short* Buffer, int size, int *BytesRead)
{
	if (!ep81)
	{
		ep81 = GetEndPoint(dev,0x81);
		ep81->XferMode = XMODE_DIRECT;
		ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		TmpBuffer[0] = (short*)malloc(size);
		TmpBuffer[1] = (short*)malloc(size);
		CurIdx=0;
		blah = ep81->BeginDataXfer((PUCHAR)TmpBuffer[CurIdx],size, &ov);
	}
	long len = size;
	ep81->WaitForXfer(&ov,100);
	if (ep81->FinishDataXfer((PUCHAR)TmpBuffer[CurIdx], len, &ov, blah))
	{
		CurIdx = 1-CurIdx;
		blah=NULL;
		blah = ep81->BeginDataXfer((PUCHAR)TmpBuffer[CurIdx],size, &ov);

		int SamplesRead = len /4; 
		*BytesRead = len;
		for (int j = 0; j < SamplesRead; j++) 
		{
			short q = TmpBuffer[1-CurIdx][j*2];
			short i = TmpBuffer[1-CurIdx][(j*2)+1];

			i &= 0xfff;
			q &= 0xfff;

			if (i & 0x800)
				i |= 0xf000;
			if (q & 0x800)
				q |= 0xf000;
		
			Buffer[j*2]=i;
			Buffer[(j*2)+1]=q;
		}
	}
	else
		return 0;
}

/*------------------------------------------------------------------------------
 * GPIO register read / write functions
 */
int gpio_read(struct bladerf *dev, uint32_t *val)
{
    long count = 16;
    UCHAR buf[16];
    buf[0] = (byte)'N';
    buf[1] = UART_PKT_MODE_DIR_READ | UART_PKT_DEV_GPIO | 0x01;
    buf[2] = 0;
    buf[3] = 0xff;
	CCyBulkEndPoint *ep2 = GetEndPoint(dev,0x2);
	CCyBulkEndPoint *ep82 = GetEndPoint(dev,0x82);

	bool res = ep2->XferData(buf,count);
    count=16;
	res = ep82->XferData(buf,count);
    while (!res)
    {
        count = 16;
        res = ep82->XferData(buf,count);
    }
	*val = buf[3];
	return res;
}

int gpio_write(struct bladerf *dev, uint32_t val)
{
    long count = 16;
    UCHAR buf[16];
    buf[0] = (byte)'N';
    buf[1] = UART_PKT_MODE_DIR_WRITE | UART_PKT_DEV_GPIO | 0x01;
    buf[2] = 0;
    buf[3] = val;
	CCyBulkEndPoint *ep2 = GetEndPoint(dev,0x2);
	CCyBulkEndPoint *ep82 = GetEndPoint(dev,0x82);

	bool res = ep2->XferData(buf,count);
    count=16;
	res = ep82->XferData(buf,count);
    while (!res)
    {
        count = 16;
        res = ep82->XferData(buf,count);
    }
	return res;
}

//mingw doesn't have this, the cypress code depends on it, so unsafe replacement.
errno_t _imp__fopen_s( 
   FILE** pFile,
   const char *filename,
   const char *mode 
)
{
  *pFile = fopen(filename,mode);
  return 0;
}

}
