necwab
======

necwab - NEC Window Accelerator Board(WAB) test on OpenBSD/luna88k

This is my experimental program to use NEC WABs (for PC-9801 series)
on [OpenBSD/luna88k](http://www.openbsd.org/luna88k.html).

Target WABs
-----------
- PC-9801-96 "Windows Accelerator Board B3", Cirrus Logic GD-5428, 1MB VRAM
- PC-9801-85 "Windows Accelerator Board B", S3 928, 1MB VRAM

Current status
--------------
### PC-9801-96

1024x768(8bpp), 800x600(8bpp), and 640x480(8bpp) mode are working with this program.  This board does not support Linear Addressing, so we have to use segment and offset access on 1MB VRAM:-<  16bpp mode are not checked yet.

![1024x768 on LUNA-88K2](https://pbs.twimg.com/media/CDubVUwVIAAb02C.jpg)

### PC-9801-85

Not working yet, just tweaking...

References
----------
- "True Color VGA Family CL-GD542X Technical Reference Manual", Cirrus Logic.
- GD5428/5430/5440を使う [http://darudarudan.syuriken.jp/kai/pc9821.htm#GD54xx](http://darudarudan.syuriken.jp/kai/pc9821.htm#GD54xx)
- VGAグラフィックドライバー VGAについて [http://softwaretechnique.jp/OS_Development/Driver/GF/VGA/vga01.html](http://softwaretechnique.jp/OS_Development/Driver/GF/VGA/vga01.html)
- PC-9821シリーズのCL-GD5428,5430,5440を使う方法 [http://www.webtech.co.jp/amiware/lzh/nec_clgd.txt](http://www.webtech.co.jp/amiware/lzh/nec_clgd.txt)
- UNDOCUMENTED 9801/9821 Vol.2 - メモリ・I/Oポート編 ウィンドウアクセラレータボード [http://www.webtech.co.jp/company/doc/undocumented_mem/io_wab.txt](http://www.webtech.co.jp/company/doc/undocumented_mem/io_wab.txt)

Special Thanks
--------------
- @oshimya: donated his PC-9801-96 WAB
