#include "diskio.h" /* FatFs 磁盘接口声明 */


#define SPI_FLASH_SECTOR_SIZE        4096U
#define SPI_FLASH_TOTAL_SECTORS      4096U
#define SPI_FLASH_FATFS_BASE_SECTOR  1536U
#define SPI_FLASH_EXPECTED_ID        0xEF4018U

/* 示例: 各驱动器的物理驱动器号映射 */
// #define DEV_FLASH	0	/* 将 FTL 映射到物理驱动器 0 */
// #define DEV_MMC		1	/* 将 MMC/SD 卡映射到物理驱动器 1 */
// #define DEV_USB		2	/* 将 USB MSD 映射到物理驱动器 2 */
#define SPI_FLASH_FatFs 0 /* 将 SPI FLASH 映射到物理驱动器 0 */

/*注意：
修改最大空间大小_MAX_SS
修改最大驱动设备数量_VOLUMES
#define FF_USE_MKFS		1  此选项切换 f_mkfs() 函数。(0:禁用 或 1:启用) 

*/

/*-----------------------------------------------------------------------*/
/* 获取驱动器状态                                                        */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv) /* 用于识别驱动器的物理驱动器号 */
{
	uint32_t ID = 0x00;
	uint8_t retry = 0;
	DSTATUS stat = STA_NOINIT;

	switch (pdrv)
	{
	case SPI_FLASH_FatFs:
		for (retry = 0; retry < 3; retry++)
		{
			ID = SPI_FLASH_DriverID();
			if (ID == SPI_FLASH_EXPECTED_ID)
			{
				stat &= ~STA_NOINIT;
				return stat;
			}
		}
		return STA_NOINIT;
	}
	stat = STA_NOINIT;
	return stat;
}

/*-----------------------------------------------------------------------*/
/* 初始化驱动器                                                          */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv) /* 用于识别驱动器的物理驱动器号 */
{
	DSTATUS stat = STA_NOINIT;
	Delay_struct delay_fatfs;
	switch (pdrv)
	{
	case SPI_FLASH_FatFs:					   // 如果选择了SPI FLASH
		//uint16_t i = 1000;
		SPI_Flash_Init();					   // 初始化SPI FLASH
		//while(i--);
		Delay_ms(&delay_fatfs,2);
		while(Delay_done_flag(&delay_fatfs) == 0);
		stat = disk_status(SPI_FLASH_FatFs);
		return stat;
	}
	return stat;
}

/*-----------------------------------------------------------------------*/
/* 读取扇区                                                              */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
	BYTE pdrv,	  /* 用于识别驱动器的物理驱动器号 */
	BYTE *buff,	  /* 存储读取数据的数据缓冲区 */
	LBA_t sector, /* LBA 起始扇区号 */
	UINT count	  /* 要读取的扇区数 */
)
{
	DRESULT res;
	// int result;

	switch (pdrv)
	{
	case SPI_FLASH_FatFs:
		sector += SPI_FLASH_FATFS_BASE_SECTOR;
		SPI_Flash_ReadDat(buff, sector * SPI_FLASH_SECTOR_SIZE, count * SPI_FLASH_SECTOR_SIZE);
		res = RES_OK;
		return res;
	}

	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* 写入扇区                                                              */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv,const BYTE *buff,LBA_t sector,UINT count)		/* 用于识别驱动器的物理驱动器号 */
																			/* 要写入的数据 */
	  																		/* LBA 起始扇区号 */
			  																/* 要写入的扇区数 */
{
	DRESULT res;
	// int result;

	switch (pdrv)
	{
	case SPI_FLASH_FatFs:
		sector += SPI_FLASH_FATFS_BASE_SECTOR;
		while (count--)
		{
			SPI_Flash_ClearFlash(sector);
			SPI_Flash_WriteDat((uint8_t *)buff, sector * SPI_FLASH_SECTOR_SIZE, SPI_FLASH_SECTOR_SIZE);
			sector++;
			buff += SPI_FLASH_SECTOR_SIZE;
		}
		res = RES_OK;
		return res;
	}

	return RES_PARERR;
}

#endif

/*-----------------------------------------------------------------------*/
/* 杂项函数                                                              */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) /* pdrv用于识别驱动器的物理驱动器号 */
													/* cmd控制命令码 */
													/* buff发送/接收控制数据的缓冲区 */
{
	DRESULT res;
	// int result;

	switch (pdrv)
	{
	case SPI_FLASH_FatFs:
		switch (cmd)
		{
		case CTRL_SYNC:
			res = RES_OK;
			return res;
		case GET_SECTOR_COUNT:		// 获取总扇区数
			*(DWORD *)buff = SPI_FLASH_TOTAL_SECTORS - SPI_FLASH_FATFS_BASE_SECTOR;
			res = RES_OK;
			return res;

		case GET_SECTOR_SIZE:
			*(WORD *)buff = SPI_FLASH_SECTOR_SIZE;
			res = RES_OK;
			return res;

		case GET_BLOCK_SIZE:
			*(DWORD *)buff = 1; // 获取擦除块大小，64KB块
			res = RES_OK;
			return res;
		}

	}

	return RES_PARERR;
}

DWORD get_fattime(void) {
	/* 返回当前时间戳 */
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}
