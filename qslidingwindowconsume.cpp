#include "qslidingwindowconsume.h"
#include "math.h"
#include "unistd.h"
#include "pthread.h"
/****************************************************
 * 构造函数，初始化参数
 *   输入:
 *     data_buf_t *buffer:存放数据的缓冲区指针
 *     int no:消费者编号
 ****************************************************
*/
QSlidingWindowConsume::QSlidingWindowConsume(data_buf_t *buffer,int no)
{
    dataBuffer = buffer;
    wnd_size = buffer->size >> 2;
    SHIFT_BITS = 32 - int(log(dataBuffer->size) / log(2));
    printf("[Consume]all_size:%d,wnd_size:%d,SHIFT_BITS:%d\n",buffer->size,wnd_size,SHIFT_BITS);
    read_head = (head_buf_t *)chr_read_head;
    node_no = no;
    empty_state = 0;
}
/*****************************************
 * 读取之前，将读指针指向最近一次生产者指针
 *****************************************
*/
void QSlidingWindowConsume::read_init()
{
    read_seq = dataBuffer->write_next_seq;
}
/******************************************************************
 * 将数据从缓冲区dataBuffer中按指定格式读到data_buffer中，读取其中一帧数据
 *    输入:
 *       char *data_buffer:存放一帧数据的缓冲区
 *    输出:
 *       成功:数据长度  失败:-1
 ******************************************************************
*/
int QSlidingWindowConsume::read_data_to_buffer(char *data_buffer)
{
heads:

    //usleep(100);
#if defined(Q_OS_WIN32)
        usleep(1000);
#elif defined(Q_OS_MACX)
        pthread_yield_np();
#elif defined(Q_OS_UNIX)
        pthread_yield();
#endif
    write_next_seq = dataBuffer->write_next_seq;
    //判定读指针是否与写指针相差1帧以上，如果不是，继续等待写数据
    if(SEQ_LEQ(write_next_seq,read_seq,SHIFT_BITS))
    {
        empty_state = 1;
        goto heads;
    }
    empty_state = 0;
    //如果读与写相差的距离在窗口设定以外，则断开读功能
    int32_t incr = SEQ_INCR(read_seq,wnd_size,dataBuffer->size);
    if(SEQ_LEQ(incr,write_next_seq,SHIFT_BITS))
    {
        printf("send window stall, break send! incr=%d,write_next_seq=%d\n",incr,write_next_seq);

        return -1;
    }
    //如果缓冲区的大小与读指针比头大，测读头信息
    if((dataBuffer->size-read_seq) >= HEAD_SIZE)
    {
        memcpy((char *)chr_read_head,dataBuffer->buffer + read_seq,HEAD_SIZE);
        read_seq = SEQ_INCR(read_seq,HEAD_SIZE,dataBuffer->size);
    }
    else  //如果缓冲区大小与读指针没有头信息大，则头文件从缓冲区头开始存放，则从缓冲区头进行读写
    {
        memcpy((char *)&chr_read_head[0],dataBuffer->buffer,HEAD_SIZE);
        read_seq = HEAD_SIZE;
    }
    //解析头是否是正确的头信息，如果是，则进行数据读写
    int flag = 1;
    for(int i = 0;i < HEAD_LEN;i++)
    {
        if(chr_read_head[i] != __HEAD[i])
        {
            flag = 0;
            printf("i,read_flag,head:%d,%x,%x \n",i,chr_read_head[i],__HEAD[i]);
            break;
        }
    }
#if 0
    printf("[consume]frmae_type:%d\n",read_head->frame.frame_type);
#endif
    //从文件头中读取数据长度
    unsigned data_len =  read_head->pkg_len;
    if(flag == 1)
    {
        //如果缓冲区长度比指针大于数据长度，则进行数据读
        if((dataBuffer->size - read_seq) >= (unsigned int)data_len)
        {
            memcpy(data_buffer,(char *)&dataBuffer->buffer[read_seq],data_len);
            read_seq = SEQ_INCR(read_seq,data_len,dataBuffer->size);
        }
        else  //否则，从缓冲区头进行数据读
        {
            memcpy(data_buffer,(char *)&dataBuffer->buffer[0],data_len);
            read_seq = data_len;
        }
#if 0
        printf("read seq:%d ,read_next_seq:%d\n",read_head->rev1,read_seq);
#endif
        return data_len;
    }
    else
    {
        printf("data format error!\n");
        return -1;
    }

}
int QSlidingWindowConsume::read_data_and_head(char *data_buffer)
{
heads:

    //usleep(100);
#if defined(Q_OS_WIN32)
        usleep(1000);
#elif defined(Q_OS_MACX)
        pthread_yield_np();
#elif defined(Q_OS_UNIX)
        pthread_yield();
#endif
    write_next_seq = dataBuffer->write_next_seq;
    //判定读指针是否与写指针相差1帧以上，如果不是，继续等待写数据
    if(SEQ_LEQ(write_next_seq,read_seq,SHIFT_BITS))
    {
        goto heads;
    }
    //如果读与写相差的距离在窗口设定以外，则断开读功能
    int32_t incr = SEQ_INCR(read_seq,wnd_size,dataBuffer->size);
    if(SEQ_LEQ(incr,write_next_seq,SHIFT_BITS))
    {
        printf("send window stall, break send! incr=%d,write_next_seq=%d\n",incr,write_next_seq);

        return -1;
    }
    //如果缓冲区的大小与读指针比头大，测读头信息
    if((dataBuffer->size-read_seq) >= HEAD_SIZE)
    {
        memcpy((char *)chr_read_head,dataBuffer->buffer + read_seq,HEAD_SIZE);
        read_seq = SEQ_INCR(read_seq,HEAD_SIZE,dataBuffer->size);
    }
    else  //如果缓冲区大小与读指针没有头信息大，则头文件从缓冲区头开始存放，则从缓冲区头进行读写
    {
        memcpy((char *)&chr_read_head[0],dataBuffer->buffer,HEAD_SIZE);
        read_seq = HEAD_SIZE;
    }
    //解析头是否是正确的头信息，如果是，则进行数据读写
    int flag = 1;
    for(int i = 0;i < HEAD_LEN;i++)
    {
        if(chr_read_head[i] != __HEAD[i])
        {
            flag = 0;
            printf("i,read_flag,head:%d,%x,%x \n",i,chr_read_head[i],__HEAD[i]);
            break;
        }
    }
#if 0
    printf("[consume]frmae_type:%d\n",read_head->frame.frame_type);
#endif
    //从文件头中读取数据长度
    unsigned data_len =  read_head->pkg_len;
    if(flag == 1)
    {
        //如果缓冲区长度比指针大于数据长度，则进行数据读
        if((dataBuffer->size - read_seq) >= (unsigned int)data_len)
        {
            memcpy(data_buffer,chr_read_head,HEAD_SIZE);
            memcpy(data_buffer + HEAD_SIZE,(char *)&dataBuffer->buffer[read_seq],data_len);
            read_seq = SEQ_INCR(read_seq,data_len,dataBuffer->size);
        }
        else  //否则，从缓冲区头进行数据读
        {
            memcpy(data_buffer,chr_read_head,HEAD_SIZE);
            memcpy(data_buffer + HEAD_SIZE,(char *)&dataBuffer->buffer[0],data_len);
            read_seq = data_len;
        }
#if 0
        printf("read seq:%d ,read_next_seq:%d\n",read_head->rev1,read_seq);
#endif
        return data_len;
    }
    else
    {
        printf("data format error!\n");
        return -1;
    }
}
int QSlidingWindowConsume::IsEmpty()
{
    return empty_state;
}
