#ifndef _MYDEF_H_
#define _MYDEF_H_

/*----------------------------------------------------------*/
/* 全局编译参数定义	*/										//
/*----------------------------------------------------------*/
/* 数据类型宏定义											*/
typedef	signed char				s08;						//
typedef	signed int				s16;						//
typedef	signed long				s32;						//
typedef	unsigned char			u08;						//
typedef	unsigned int			u16;						//
typedef	unsigned long			u32;						//
typedef	const unsigned char		c08;						//
typedef	const unsigned int		c16;						//
typedef	const unsigned long		c32;						//
/*__________________________________________________________*/
/* 编译宏定义												*/
#define	s08	signed char										//
#define	s16	signed int										//
#define	u16	unsigned int									//
#define	u08	unsigned char									//
#define	c08	const unsigned char								//
#define c16 const unsigned int								//
#define	true					1							//
#define	false					0							//
#define error					0							//
#define high					1							//
#define low						0							//
#define	bit7					0x80						//
#define	bit6					0x40						//
#define	bit5					0x20						//
#define	bit4					0x10						//
#define	bit3					0x08						//
#define	bit2					0x04						//
#define	bit1					0x02						//
#define	bit0					0x01						//
/*__________________________________________________________*/
/* 常用逻辑运算指令											*/
#define	bit( x )				( 1<<( x ) )				// x是小于8的整数
#define	sbi( x,	y )				( x	|= y )					// x第y位置1 y是十六进制数
#define	cbi( x,	y )				( x	&=~y )					// x第y位置0 y是十六进制数
#define	kbi( x,	y )				( x	^= y )					// x第y位置反 y是十六进制数
#define	cki( x,	y )				( x	&  y )					// 测试x的第y位	结果0表示0 结果非0表示1	y是十六进制数
#define	SBI( x,	y )			 sbi( x, bit(y)	)				// x第y位置1 y是小于8的整数
#define	CBI( x,	y )			 cbi( x, bit(y)	)				// x第y位置0 y是小于8的整数
#define	KBI( x,	y )			 kbi( x, bit(y)	)				// x第y位置反 y是小于8的整数
#define	CKI( x,	y )			 cki( x, bit(y)	)				// 测试x的第y位	结果0表示0 结果非0表示1	y是小于8的整数
#define	BitMap(	x )				( 1<<( x ) )
#define	GetMax(	x1,	x2 ) ( ( x1	) >	( x2 ) ? ( x1 )	: (	x2 ) )//获取x1,x2的最大值
#define	GetMin(	x1,	x2 ) ( ( x1	) >	( x2 ) ? ( x2 )	: (	x1 ) )//获取x1,x2的最小值
/*----------------------------------------------------------*/

















#endif