/****************************************************************************/
/*                                                                          */
/*              DSP C6748 内存空间分配定义                                  */
/*                                                                          */
/*              2014年07月05日                                              */
/*                                                                          */
/****************************************************************************/

MEMORY
{
    SHDSPL2RAM   o = 0x11800000  l = 0x00040000  /* 256KB L2 共享内置 RAM */
    SHRAM        o = 0x80000000  l = 0x00020000  /* 128KB 共享 RAM */
    DDR2         o = 0xC3000000  l = 0x00800000  /* 8M    DSP 程序空间 */
}

SECTIONS
{
    .text:_c_int00	>  DDR2 align(1024)			 /* C 语言入口 */
    .text			>  DDR2             		 /* 可执行代码 */
    .stack  		>  DDR2 				     /* 软件系统栈 */

    .cio			>  DDR2                      /* C 输入输出缓存 */
    .vectors		>  DDR2      				 /* 中断向量表 */
    .const			>  DDR2                      /* 常量 */
    .data			>  DDR2                      /* 已初始化全局及静态变量 */
    .switch			>  DDR2                      /* 跳转表 */
    .sysmem			>  DDR2                      /* 动态内存分配区域 */

    .args			>  DDR2
    .ppinfo			>  DDR2
    .ppdata			>  DDR2

    /* TI-ABI 或 COFF */
    .pinit			>  DDR2                      /* C++ 结构表 */
    .cinit			>  DDR2                      /* 初始化表 */

    /* EABI */
    .binit			>  DDR2
    .init_array		>  DDR2
    .fardata		>  DDR2

    .c6xabi.exidx	>  DDR2
    .c6xabi.extab	>  DDR2

	.init_array     >  DDR2

	GROUP(NEARDP_DATA)
	{
	   .neardata
	   .rodata
	   .bss
	}               >  DDR2

    .far			>  DDR2
}
