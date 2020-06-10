#ifndef _TYPES_H_
#define _TYPES_H_

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef unsigned char __u8;//Jody
typedef unsigned short __le16;//Jody
typedef unsigned short __u16;//Jody

typedef unsigned short __be16;
typedef unsigned int __le32;
typedef unsigned int __be32;
typedef unsigned int  __hc32;
typedef unsigned short  __hc16;


typedef unsigned gfp_t;
typedef unsigned int dma_addr_t;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;
typedef signed char		s8;
typedef short			s16;
typedef int			s32;
typedef long long		s64;

#define __GFP_HIGH	(0x20u)
#define GFP_ATOMIC	(__GFP_HIGH)

typedef struct {
	int counter;
} atomic_t;

struct kref {
	atomic_t refcount;
};

#define min(x,y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);	\
	_x < _y ? _x : _y; })

#define max(x,y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);	\
	_x > _y ? _x : _y; })

#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1: __min2; })

#define max_t(type, x, y) ({			\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1: __max2; })

#endif /* _TYPES_H_ */
