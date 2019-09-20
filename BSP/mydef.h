#ifndef _MYDEF_H_
#define _MYDEF_H_

/*----------------------------------------------------------*/
/* ȫ�ֱ����������	*/										//
/*----------------------------------------------------------*/
/* �������ͺ궨��											*/
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
/* ����궨��												*/
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
/* �����߼�����ָ��											*/
#define	bit( x )				( 1<<( x ) )				// x��С��8������
#define	sbi( x,	y )				( x	|= y )					// x��yλ��1 y��ʮ��������
#define	cbi( x,	y )				( x	&=~y )					// x��yλ��0 y��ʮ��������
#define	kbi( x,	y )				( x	^= y )					// x��yλ�÷� y��ʮ��������
#define	cki( x,	y )				( x	&  y )					// ����x�ĵ�yλ	���0��ʾ0 �����0��ʾ1	y��ʮ��������
#define	SBI( x,	y )			 sbi( x, bit(y)	)				// x��yλ��1 y��С��8������
#define	CBI( x,	y )			 cbi( x, bit(y)	)				// x��yλ��0 y��С��8������
#define	KBI( x,	y )			 kbi( x, bit(y)	)				// x��yλ�÷� y��С��8������
#define	CKI( x,	y )			 cki( x, bit(y)	)				// ����x�ĵ�yλ	���0��ʾ0 �����0��ʾ1	y��С��8������
#define	BitMap(	x )				( 1<<( x ) )
#define	GetMax(	x1,	x2 ) ( ( x1	) >	( x2 ) ? ( x1 )	: (	x2 ) )//��ȡx1,x2�����ֵ
#define	GetMin(	x1,	x2 ) ( ( x1	) >	( x2 ) ? ( x2 )	: (	x1 ) )//��ȡx1,x2����Сֵ
/*----------------------------------------------------------*/

















#endif