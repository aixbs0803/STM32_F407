FatFs 模块源文件 R0.16


文件说明

  00readme.txt   本文件。
  00history.txt  版本历史记录。
  ff.c           FatFs 模块。
  ffconf.h       FatFs 模块的配置文件。
  ff.h           FatFs 和应用模块的公共头文件。
  diskio.h       FatFs 和磁盘 I/O 模块的公共头文件。
  diskio.c       将现有磁盘 I/O 模块连接到 FatFs 的示例粘合函数。
  ffunicode.c    可选的 Unicode 实用函数。
  ffsystem.c     可选的操作系统相关函数示例。


  低级磁盘 I/O 模块未包含在此归档中，因为 FatFs
  模块只是一个通用文件系统层，不依赖于任何特定
  存储设备。您需要提供一个低级磁盘 I/O 模块来控制
  连接到目标系统的存储设备。
