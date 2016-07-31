#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

/*  Memory Map (see ../dsp/c6748.cmd and linux boot arguments)
 *
 *  1180_0000 - 1184_0000        4_0000  ( 256 KB) SHDSPL2RAM 
 *  8000_0000 - 8002_0000        2_0000  ( 128 KB) SHRAM
 *  C000_0000 - C7FF_FFFF      800_0000  ( 128 MB) External Memory (DDR2)
 *  ------------------------------------------------------------------------
 *     C000_0000 - C1FF_FFFF   200_0000  (  32 MB) Linux
 *     C200_0000 - C2FF_FFFF   100_0000  (  16 MB) -------- 
 *     C300_0000 - C37F_FFFF    80_0000  (   8 MB) DSP_PROG (code, data)
 *     C380_0000 - C3FF_FFFF    80_0000  (   8 MB) --------
 *     C400_0000 - C7FF_FFFF   400_0000  (  64 MB) Linux
 */

//#define SHARED_BUFFER_ADDR 0xC2000000
#define SHARED_BUFFER_SIZE 0x4000

//时域数据-------------------------------------------------------------------------------
#define ARM_to_DSP 			0xC2000000			//状态标记字节				0：忙		1：闲
#define A_to_D_operation 	0xC2000001			//上次一操作该区域处理器标记		0：ARM	1：DSP
#define A_to_D_check_A 		0xC2000002			//随机数A
#define A_to_D_data			0xC2000004			//时域数据
#define A_to_Dcheck_B 		0xC2000804			//随机数B

//频域数据-------------------------------------------------------------------------------
#define DSP_to_ARM 			0xC2001000			//状态标记字节				0：忙		1：闲
#define D_to_A_operation 	0xC2001001			//上次一操作该区域处理器标记		0：ARM	1：DSP
#define D_to_A_check_A 		0xC2001002			//随机数A
#define D_to_A_data			0xC2001004			//频域数据
#define D_to_A_check_B 		0xC2001404			//随机数B

//其他数据-------------------------------------------------------------------------------
#define ARM_setting		 	0xC2001600			//

#define	Free	0x00
#define Busy	0x01
#define ARM		0x00
#define DSP		0x01


#endif
