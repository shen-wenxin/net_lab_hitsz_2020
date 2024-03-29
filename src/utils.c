#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define IPTOSBUFFERS 12

/**
 * @brief ip转字符串
 * 
 * @param ip ip地址
 * @return char* 生成的字符串
 */
char *iptos(uint8_t *ip)
{
    static char output[IPTOSBUFFERS][3 * 4 + 3 + 1];
    static short which;
    which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
    sprintf(output[which], "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return output[which];
}

/**
 * @brief 初始化buffer为给定的长度，用于装载数据包
 * 
 * @param buf 要初始化的buffer
 * @param len 长度
 */
void buf_init(buf_t *buf, int len)
{
    buf->len = len;
    buf->data = buf->payload + BUF_MAX_LEN - len;
}

/**
 * @brief 为buffer在头部增加一段长度，用于添加协议头
 * 
 * @param buf 要修改的buffer
 * @param len 增加的长度
 */
void buf_add_header(buf_t *buf, int len)
{
    buf->len += len;
    buf->data -= len;
}

/**
 * @brief 为buffer在头部减少一段长度，去除协议头
 * 
 * @param buf 要修改的buffer
 * @param len 减少的长度
 */
void buf_remove_header(buf_t *buf, int len)
{
    buf->len -= len;
    buf->data += len;
}

/**
 * @brief 复制一个buffer到新buffer
 * 
 * @param dst 目的buffer
 * @param src 源buffer
 */
void buf_copy(buf_t *dst, buf_t *src)
{
    buf_init(dst, src->len);
    memcpy(dst->payload, src->payload, BUF_MAX_LEN);
}

/**
 * @brief 计算16位校验和
 *        1. 把首部看成以 16 位为单位的数字组成，依次进行二进制求和
 *           注意：求和时应将最高位的进位保存，所以加法应采用 32 位加法
 *        2. 将上述加法过程中产生的进位（最高位的进位）加到低 16 位
 *           采用 32 位加法时，即为将高 16 位与低 16 位相加，
 *           之后还要把该次加法最高位产生的进位加到低 16 位
 *        3. 将上述的和取反，即得到校验和。  
 *        
 * @param buf 要计算的数据包
 * @param len 要计算的长度
 * @return uint16_t 校验和
 */
uint16_t checksum16(uint16_t *buf, int len)
{
    /*uint16_t ans = 0;
    uint32_t temp = 0;
    uint16_t * temp_buf = buf;
    int i ;
    for(i = len;i > 1;i = i - sizeof(uint16_t)){
       temp += (uint32_t)(*temp_buf);
	temp_buf ++;
    }
    if(i > 0){
    	char left_over[2] = {0};
	left_over[0] = *buf;
	temp += *(unsigned short *)left_over;
    }
    ////将高16位与低16 位相加
    uint32_t a = (temp >> 16)&(0xffff);//得到高的16位
    uint32_t b = temp & 0xffff; //得到低16位
    temp = a + b;   //将高的16位和低的16位直接相加

    // //再把这次假发的最高位加到低十六位
    temp = (temp & 0xffff) + ((temp >> 16)&(0xffff));

    ans = ~(temp&0xffff);

    // //将上述和取反
    return ans;*/
    long sum = 0;
    while(len > 1){
	sum += *(unsigned short *)buf ++;
	len -= 2;
    }
    if(len > 0){
	char left_over[2] = {0};
	left_over[0] = *buf;
	sum += *(unsigned short *)left_over;
    }
    while(sum >> 16){
	sum = (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;

 
}
