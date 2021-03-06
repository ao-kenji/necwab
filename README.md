necwab
======

necwab - NEC Window Accelerator Board(WAB) test on OpenBSD/luna88k

This is my experimental program to use NEC and other 3rd-party-made WABs
(for PC-9801 series) on [OpenBSD/luna88k](http://www.openbsd.org/luna88k.html).

Target WABs
-----------
- PC-9801-96 "Windows Accelerator Board B3", Cirrus Logic GD-5428, 1MB VRAM
- PC-9801-85 "Windows Accelerator Board B", S3 928, 1MB VRAM
- MELCO WGN-A2, Cirrus Logic GD-5434, 2MB VRAM
- MELCO WSN-A2F, Cirrus Logic GD-5434, 2MB VRAM (and sound functions)

Current status
--------------
### PC-9801-96

1024x768(8bpp), 800x600(8bpp and 16bpp), and 640x480(8bpp and 16bpp) modes are working with this program.

Note: Although the GD-5428 itself has linear addressing feature, this board seems not support linear addressing.  So here I have to use "paging" addressing (base and offset) scheme on 1MB VRAM:-(

![256 color pattern on 1024x768 resolution](https://pbs.twimg.com/media/CDubVUwVIAAb02C.jpg)

![yaft runs on 800x600 resolution](https://pbs.twimg.com/media/CEDvIo0VAAE-BF0.jpg)

### PC-9801-85

Not working yet, just tweaking...

### MELCO WGN-A2

Both 1280x1024(8bpp) and 1024x768(16bpp) modes are working as well as other modes supported on PC-9801-96.

![WGN-A2, 1280x1024 resolution](https://pbs.twimg.com/media/CIjZBZhUcAAUo8S.jpg)

### MELCO WSN-A2F

Both 1280x1024(8bpp) and 1024x768(16bpp) modes are working as well as other modes supported on PC-9801-96.

![WSN-A2F, 1280x1024 resolution](https://pbs.twimg.com/media/CXrmLNLUkAIQ1cT.jpg)
![WSN-A2F, 1024x768 resolution](https://pbs.twimg.com/media/CXrmLu4UoAAc4_R.jpg)

References
----------
- "True Color VGA Family CL-GD542X Technical Reference Manual", Cirrus Logic.
- "Alpine(tm) VGA Family CL-GD543X/'4X Technical Reference Manual", Cirrus Logic.
- GD5428/5430/5440を使う [http://darudarudan.syuriken.jp/kai/pc9821.htm#GD54xx](http://darudarudan.syuriken.jp/kai/pc9821.htm#GD54xx)
- VGAグラフィックドライバー VGAについて [http://softwaretechnique.jp/OS_Development/Driver/GF/VGA/vga01.html](http://softwaretechnique.jp/OS_Development/Driver/GF/VGA/vga01.html)
- PC-9821シリーズのCL-GD5428,5430,5440を使う方法 [http://www.webtech.co.jp/amiware/lzh/nec_clgd.txt](http://www.webtech.co.jp/amiware/lzh/nec_clgd.txt)
- UNDOCUMENTED 9801/9821 Vol.2 - メモリ・I/Oポート編 ウィンドウアクセラレータボード [http://www.webtech.co.jp/company/doc/undocumented_mem/io_wab.txt](http://www.webtech.co.jp/company/doc/undocumented_mem/io_wab.txt)

Special Thanks
--------------
- @oshimyja: donated his PC-9801-96 WAB
