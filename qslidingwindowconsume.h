#ifndef QSLIDINGWINDOWCONSUME_H
#define QSLIDINGWINDOWCONSUME_H
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
//#include "qslidingwindowconsume_global.h"

#define	SEQ_LT(a, b, c)		((int32_t) (((a) - (b)) << c) < 0)
#define SEQ_LEQ(a, b, c)	((int32_t) (((a) - (b)) << c) <= 0)
#define SEQ_GT(a, b, c)		((int32_t) (((a) - (b)) << c) > 0)
#define SEQ_GEQ(a, b, c)	((int32_t) (((a) - (b)) << c) >= 0)
#define SEQ_INCR(a, b, c)	(((a) + (b)) % (c))

typedef unsigned char  	    u8;	/**< UNSIGNED 8-bit data type */
typedef unsigned short 	    u16;/**< UNSIGNED 16-bit data type */
typedef unsigned int   	   	u32;/**< UNSIGNED 32-bit data type */
typedef unsigned long long 	u64;/**< UNSIGNED 64-bit data type */
typedef signed char         s8;	/**< SIGNED 8-bit data type */
typedef signed short       	s16;/**< SIGNED 16-bit data type */
typedef signed int         	s32;/**< SIGNED 32-bit data type */
typedef signed long long	s64;/**< SIGNED 64-bit data type */
typedef u8                  uint8_t;
typedef u32                 uint32_t;

struct data_buf_t
{
    unsigned write_next_seq;
    unsigned size;
    char 	 *buffer;//[MAX_STREAM_LEN];
};
//保存到缓冲区的结构体
#define HEAD_LEN (8)
const char BUFHEAD[] = { \
    0x55,0xaa, \
    0xaa,0x55, \
    0x55,0xaa, \
    0xaa,0x55, \
};
struct _frame_info_t
{
    u16 frame_type;
};
struct head_buf_t
{
    char flag[HEAD_LEN];
    u32 pkg_len;
    struct _frame_info_t frame;
    u16 rev0;
    u16 rev1;
    u16 rev2;
};
#define HEAD_SIZE (sizeof(head_buf_t))

#define BUILD_PKG_HEAD(p,len,_frame) \
  p->flag[0] = 0x55; \
  p->flag[1] = 0xaa; \
  p->flag[2] = 0xaa; \
  p->flag[3] = 0x55; \
  p->flag[4] = 0x55; \
  p->flag[5] = 0xaa; \
  p->flag[6] = 0xaa; \
  p->flag[7] = 0x55;  \
  p->pkg_len = len; \
  memcpy(&p->frame,(char *)_frame,sizeof(struct _frame_info_t));




//class QSLIDINGWINDOWCONSUMESHARED_EXPORT QSlidingWindowConsume
class QSlidingWindowConsume
{

public:
    QSlidingWindowConsume(data_buf_t *buffer,int no); //构造函数
    void read_init(); //读指针初始化
    int read_data_to_buffer(char *data_buffer); //将数据读取到缓冲区
    int read_data_and_head(char *data_buffer); //读取数据和头
    /*
     * add by antony 2016-7-15
     */
    int read_data_to_buffer(char *data_buffer,int locked); //非锁定方式读取数据
    /*int IsEmpty(); //delete by antony 2016-7-15 */

private:
    char chr_read_head[sizeof(head_buf_t)];  //缓冲区中头字符串
    head_buf_t *read_head; //缓冲区中头指针
    u32 read_seq;  //读指针(序列号)
    u32 write_next_seq; //写指针(序列号)
    data_buf_t *dataBuffer;  //数据缓冲区
    u32 wnd_size;  //窗口大小
    int SHIFT_BITS;  //移位位数
    int node_no;  //用户编号
    /*int empty_state; //delete by antony 2016-7-15 */
};

#endif // QSLIDINGWINDOWCONSUME_H
