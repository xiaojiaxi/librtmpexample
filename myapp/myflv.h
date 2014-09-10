#ifndef MYFLV_H
#define MYFLV_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef MAX_FILE_PATH
#define MAX_FILE_PATH 260
#endif

//#define MYFLVMALLOC(a) (void*b=malloc(a);memset(b,0,a);b)
//#define MYFLVFREE(a)   do{free(a);a=NULL;}while(0)


#define HTON16(x)  ((x>>8&0xff)|(x<<8&0xff00))
#define HTON24(x)  ((x>>16&0xff)|(x<<16&0xff0000)|(x&0xff00))
#define HTON32(x)  ((x>>24&0xff)|(x>>8&0xff00)|(x<<8&0xff0000)|(x<<24&0xff000000))
#define HTONTIME(x) ((x>>16&0xff)|(x<<16&0xff0000)|(x&0xff00)|(x&0xff000000))

int ReadU8(uint32_t *u8,FILE*fp);
int ReadU16(uint32_t *u16,FILE*fp);
int ReadU24(uint32_t *u24,FILE*fp);
int ReadU32(uint32_t *u32,FILE*fp);
int PeekU8(uint32_t *u8,FILE*fp);
int ReadUTime(uint32_t *utime,FILE*fp);


int WriteU8(uint32_t u8,FILE*fp);
int WriteU16(uint32_t u16,FILE*fp);
int WriteU24(uint32_t u24,FILE*fp);
int WriteU32(uint32_t u32,FILE*fp);
int WriteUTime(uint32_t utime,FILE*fp);

typedef struct _MyFrame{
uint32_t type;//类型，1字节0x08音频0x09视频
uint32_t datalength;//数据长度，3字节
uint32_t timestamp;//时间戳，4字节
uint32_t streamid;//流ID，3字节

uint32_t bkeyframe;//关键帧0x17

char*	 buffer;//存储音视频数据信息
uint8_t  breadbuf;//获取数据
uint32_t alldatalength;//该帧总长度，4字节
}MyFrame;

typedef struct _MyFLV{
FILE*fp;//文件指针
char filename[MAX_FILE_PATH];
MyFrame*vi;//视频信息
MyFrame*ai;//音频信息
uint32_t startpos;//起始位置，音视频信息后
uint32_t totalsize;//文件总大小
uint32_t pos;//当前位置
uint32_t currenttime;//当前时间戳ms
uint32_t duration;//总时间
uint32_t looptimes;//循环次数
uint8_t bloop;//是否循环
uint8_t beof;//文件读取结束
}MyFLV;
//打开FLV文件并处理
MyFLV*MyFlvOpen(const char*filename);
//获取帧信息
//buf不为空并且len足够，返回数据并跳到下一帧
//否则还在当前帧
MyFrame MyFlvGetFrameInfo(MyFLV*myflv,char*buf,uint32_t len);


MyFLV*MyFlvCreate(const char*filename);
int MyFlvWriteFrame(MyFLV*myflv,MyFrame myframe,char*buf,uint32_t len);

MyFLV*MyFlvClose(MyFLV*myflv);
#endif