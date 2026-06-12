/*---------------------------------------------------------------------------/
/  FatFs 模块配置
/---------------------------------------------------------------------------*/

#define FFCONF_DEF	80386	/* 版本 ID */

/*---------------------------------------------------------------------------/
/ 功能配置
/---------------------------------------------------------------------------*/

#define FF_FS_READONLY	0
/* 此选项切换只读配置。(0:读/写 或 1:只读)
/  只读配置会移除写入 API 函数：f_write()、f_sync()、
/  f_unlink()、f_mkdir()、f_chmod()、f_rename()、f_truncate()、f_getfree()
/  以及可选的写入函数。 */


#define FF_FS_MINIMIZE	0
/* 此选项定义最小化级别，以移除部分基本 API 函数。
/
/   0: 基本函数完全启用。
/   1: 移除 f_stat()、f_getfree()、f_unlink()、f_mkdir()、f_truncate() 和 f_rename()。
/   2: 在 1 的基础上再移除 f_opendir()、f_readdir() 和 f_closedir()。
/   3: 在 2 的基础上再移除 f_lseek() 函数。 */


#define FF_USE_FIND		0
/* 此选项切换过滤目录读取函数 f_findfirst() 和 f_findnext()。
/  (0:禁用, 1:启用 2:启用并同时匹配 altname[]) */


#define FF_USE_MKFS		1
/* 此选项切换 f_mkfs() 函数。(0:禁用 或 1:启用) */


#define FF_USE_FASTSEEK	0
/* 此选项切换快速寻道功能。(0:禁用 或 1:启用) */


#define FF_USE_EXPAND	0
/* 此选项切换 f_expand() 函数。(0:禁用 或 1:启用) */


#define FF_USE_CHMOD	0
/* 此选项切换属性控制 API 函数 f_chmod() 和 f_utime()。
/  (0:禁用 或 1:启用) 此外 FF_FS_READONLY 需要为 0 才能启用此选项。 */


#define FF_USE_LABEL	0
/* 此选项切换卷标 API 函数 f_getlabel() 和 f_setlabel()。
/  (0:禁用 或 1:启用) */


#define FF_USE_FORWARD	0
/* 此选项切换 f_forward() 函数。(0:禁用 或 1:启用) */


#define FF_USE_STRFUNC	0
#define FF_PRINT_LLI	0
#define FF_PRINT_FLOAT	0
#define FF_STRF_ENCODE	0
/* FF_USE_STRFUNC 切换字符串 API 函数：f_gets()、f_putc()、f_puts() 和
/  f_printf()。
/
/   0: 禁用。FF_PRINT_LLI、FF_PRINT_FLOAT 和 FF_STRF_ENCODE 无效。
/   1: 启用但不进行 LF-CRLF 转换。
/   2: 启用并进行 LF-CRLF 转换。
/
/  FF_PRINT_LLI = 1 使 f_printf() 支持 long long 参数，FF_PRINT_FLOAT = 1/2
/  使 f_printf() 支持浮点参数。这些功能需要 C99 或更高版本。
/  当 FF_LFN_UNICODE >= 1 且启用了 LFN 时，字符串 API 函数会转换其中的字符编码。
/  FF_STRF_ENCODE 选择通过这些函数读/写的文件上字符编码的设定。
/
/   0: 当前代码页中的 ANSI/OEM 编码
/   1: UTF-16LE 编码的 Unicode
/   2: UTF-16BE 编码的 Unicode
/   3: UTF-8 编码的 Unicode
*/


/*---------------------------------------------------------------------------/
/ 区域设置和命名空间配置
/---------------------------------------------------------------------------*/

#define FF_CODE_PAGE	437
/* 此选项指定目标系统上使用的 OEM 代码页。
/  错误的代码页设置可能导致文件打开失败。
/
/   437 - 美国
/   720 - 阿拉伯语
/   737 - 希腊语
/   771 - KBL
/   775 - 波罗的海语
/   850 - 拉丁语 1
/   852 - 拉丁语 2
/   855 - 西里尔语
/   857 - 土耳其语
/   860 - 葡萄牙语
/   861 - 冰岛语
/   862 - 希伯来语
/   863 - 加拿大法语
/   864 - 阿拉伯语
/   865 - 北欧语
/   866 - 俄语
/   869 - 希腊语 2
/   932 - 日语 (DBCS)
/   936 - 简体中文 (DBCS)
/   949 - 韩语 (DBCS)
/   950 - 繁体中文 (DBCS)
/     0 - 包含以上所有代码页，通过 f_setcp() 配置
*/


#define FF_USE_LFN		0
#define FF_MAX_LFN		255
/* FF_USE_LFN 切换 LFN（长文件名）支持。
/
/   0: 禁用 LFN。FF_MAX_LFN 无效。
/   1: 启用 LFN，使用 BSS 上的静态工作缓冲区。始终非线程安全。
/   2: 启用 LFN，使用栈上的动态工作缓冲区。
/   3: 启用 LFN，使用堆上的动态工作缓冲区。
/
/  要启用 LFN，需要将 ffunicode.c 添加到项目中。LFN 功能
/  需要一定的内部工作缓冲区，占用 (FF_MAX_LFN + 1) * 2 字节，
/  启用 exFAT 时还需要额外的 (FF_MAX_LFN + 44) / 15 * 32 字节。
/  FF_MAX_LFN 定义工作缓冲区的大小（以 UTF-16 编码单元为单位），
/  其取值范围为 12 到 255。建议设置为 255 以完全支持 LFN 规范。
/  当使用栈作为工作缓冲区时，需要注意栈溢出。当使用堆内存
/  作为工作缓冲区时，需要将内存管理函数 ff_memalloc() 和
/  ff_memfree()（示例见 ffsystem.c）添加到项目中。 */


#define FF_LFN_UNICODE	0
/* 此选项切换启用 LFN 时 API 上的字符编码。
/
/   0: 当前代码页中的 ANSI/OEM 编码 (TCHAR = char)
/   1: UTF-16 编码的 Unicode (TCHAR = WCHAR)
/   2: UTF-8 编码的 Unicode (TCHAR = char)
/   3: UTF-32 编码的 Unicode (TCHAR = DWORD)
/
/  字符串 I/O 函数的行为也会受此选项影响。
/  当 LFN 未启用时，此选项无效。 */


#define FF_LFN_BUF		255
#define FF_SFN_BUF		12
/* 这组选项定义 FILINFO 结构中文件名称成员的大小，
/  该结构用于读取目录项。这些值应足够容纳要读取的文件名。
/  读取的文件名的最大可能长度取决于字符编码。
/  当 LFN 未启用时，这些选项无效。 */


#define FF_FS_RPATH		0
/* 此选项配置相对路径功能的支持。
/
/   0: 禁用相对路径并移除相关 API 函数。
/   1: 启用相对路径和点名称。f_chdir() 和 f_chdrive() 可用。
/   2: 在 1 的基础上 f_getcwd() 也可用。
*/


#define FF_PATH_DEPTH	10
/* 此选项定义 exFAT 卷中目录的最大深度。它与
/  FAT/FAT32 卷无关。
/  例如，FF_PATH_DEPTH = 3 可以跟踪 "/dir1/dir2/dir3/file" 路径，
/  但 dir3 中的子目录将无法被跟踪和设置为当前目录。
/  文件系统对象 (FATFS) 的大小会增加 FF_PATH_DEPTH * 24 字节。
/  当 FF_FS_EXFAT == 0 或 FF_FS_RPATH == 0 时，此选项无效。
*/



/*---------------------------------------------------------------------------/
/ 驱动器/卷配置
/---------------------------------------------------------------------------*/

#define FF_VOLUMES		3
/* 要使用的卷（逻辑驱动器）数量。(1-10) */


#define FF_STR_VOLUME_ID	0
#define FF_VOLUME_STRS		"RAM","NAND","CF","SD","SD2","USB","USB2","USB3"
/* FF_STR_VOLUME_ID 切换对任意字符串卷 ID 的支持。
/  当 FF_STR_VOLUME_ID 设置为 1 或 2 时，可以在路径名中使用任意字符串
/  作为驱动器号。FF_VOLUME_STRS 定义每个逻辑驱动器的卷 ID 字符串。
/  项目数不能少于 FF_VOLUMES。卷 ID 字符串的有效字符为 A-Z、a-z 和 0-9，
/  但比较时不区分大小写。如果 FF_STR_VOLUME_ID >= 1 且未定义 FF_VOLUME_STRS，
/  则需要用户定义的卷字符串表，如下所示：
/
/  const char* VolumeStr[FF_VOLUMES] = {"ram","flash","sd","usb",...
*/


#define FF_MULTI_PARTITION	0
/* 此选项切换对物理驱动器上多个卷的支持。
/  默认情况下 (0)，每个逻辑驱动器号绑定到相同的物理驱动器号，
/  并且只挂载物理驱动器上找到的一个 FAT 卷。
/  启用此功能 (1) 后，每个逻辑驱动器号可以绑定到
/  任意的物理驱动器和分区，列在 VolToPart[] 中。此外 f_fdisk() 也可用。 */


#define FF_MIN_SS		512
#define FF_MAX_SS		4096
/* 这组选项配置支持的扇区大小范围。(512, 1024, 2048 或 4096)
/  对于大多数系统、通用存储卡和硬盘，始终将两者设置为 512，
/  但对于板载闪存和某些类型的光学介质，可能需要更大的值。
/  当 FF_MAX_SS 大于 FF_MIN_SS 时，FatFs 配置为可变扇区大小模式，
/  disk_ioctl() 需要实现 GET_SECTOR_SIZE 命令。 */


#define FF_LBA64		0
/* 此选项切换 64 位 LBA 支持。(0:禁用 或 1:启用)
/  要启用 64 位 LBA，还需要启用 exFAT。(FF_FS_EXFAT == 1) */


#define FF_MIN_GPT		0x10000000
/* 在 f_mkfs() 和 f_fdisk() 中切换 GPT 作为分区格式的最小扇区数。
/  最大 2^32 个扇区。当 FF_LBA64 == 0 时，此选项无效。 */


#define FF_USE_TRIM		0
/* 此选项切换 ATA-TRIM 支持。(0:禁用 或 1:启用)
/  要启用此功能，还需要在 disk_ioctl() 中实现 CTRL_TRIM 命令。 */



/*---------------------------------------------------------------------------/
/ 系统配置
/---------------------------------------------------------------------------*/

#define FF_FS_TINY		0
/* 此选项切换微型缓冲区配置。(0:常规 或 1:微型)
/  在微型配置下，文件对象 (FIL) 的大小减少 FF_MAX_SS 字节。
/  文件数据不使用文件对象中省略的私有扇区缓冲区，
/  而是使用文件系统对象 (FATFS) 中共用的扇区缓冲区进行数据传输。 */


#define FF_FS_EXFAT		0
/* 此选项切换 exFAT 文件系统支持。(0:禁用 或 1:启用)
/  要启用 exFAT，还需要启用 LFN。(FF_USE_LFN >= 1)
/  注意，启用 exFAT 会放弃 ANSI C (C89) 兼容性。 */


#define FF_FS_NORTC		0
#define FF_NORTC_MON	1
#define FF_NORTC_MDAY	1
#define FF_NORTC_YEAR	2025
/* The option FF_FS_NORTC switches timestamp feature. If the system does not have
/  an RTC or valid timestamp is not needed, set FF_FS_NORTC = 1 to disable the
/  timestamp feature. Every object modified by FatFs will have a fixed timestamp
/  defined by FF_NORTC_MON, FF_NORTC_MDAY and FF_NORTC_YEAR in local time.
/  To enable timestamp function (FF_FS_NORTC = 0), get_fattime() need to be added
/  to the project to read current time form real-time clock. FF_NORTC_MON,
/  FF_NORTC_MDAY and FF_NORTC_YEAR have no effect.
/  These options have no effect in read-only configuration (FF_FS_READONLY = 1). */


#define FF_FS_CRTIME	0
/* This option enables(1)/disables(0) the timestamp of the file created. When
/  set 1, the file created time is available in FILINFO structure. */


#define FF_FS_NOFSINFO	0
/* If you need to know the correct free space on the FAT32 volume, set bit 0 of
/  this option, and f_getfree() on the first time after volume mount will force
/  a full FAT scan. Bit 1 controls the use of last allocated cluster number.
/
/  bit0=0: Use free cluster count in the FSINFO if available.
/  bit0=1: Do not trust free cluster count in the FSINFO.
/  bit1=0: Use last allocated cluster number in the FSINFO if available.
/  bit1=1: Do not trust last allocated cluster number in the FSINFO.
*/


#define FF_FS_LOCK		0
/* The option FF_FS_LOCK switches file lock function to control duplicated file open
/  and illegal operation to open objects. This option must be 0 when FF_FS_READONLY
/  is 1.
/
/  0:  Disable file lock function. To avoid volume corruption, application program
/      should avoid illegal open, remove and rename to the open objects.
/  >0: Enable file lock function. The value defines how many files/sub-directories
/      can be opened simultaneously under file lock control. Note that the file
/      lock control is independent of re-entrancy. */


#define FF_FS_REENTRANT	0
#define FF_FS_TIMEOUT	1000
/* The option FF_FS_REENTRANT switches the re-entrancy (thread safe) of the FatFs
/  module itself. Note that regardless of this option, file access to different
/  volume is always re-entrant and volume control functions, f_mount(), f_mkfs()
/  and f_fdisk(), are always not re-entrant. Only file/directory access to
/  the same volume is under control of this featuer.
/
/   0: Disable re-entrancy. FF_FS_TIMEOUT have no effect.
/   1: Enable re-entrancy. Also user provided synchronization handlers,
/      ff_mutex_create(), ff_mutex_delete(), ff_mutex_take() and ff_mutex_give(),
/      must be added to the project. Samples are available in ffsystem.c.
/
/  The FF_FS_TIMEOUT defines timeout period in unit of O/S time tick.
*/



/*--- End of configuration options ---*/
