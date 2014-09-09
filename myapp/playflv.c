#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifndef RTMPDUMP_VERSION
#define RTMPDUMP_VERSION "v2.4"
#endif 

#include "librtmp/rtmp_sys.h"
#include "librtmp/log.h"

#include "myflv.h"

void PlayFlv(const char*rtmpurl,const char*flvfilename,int bLive);

int MYINIT();//初始化相关
void MYCLEAR();//清除相关
int main(){
	const char* rtmpurl="rtmp://192.168.1.100:1935/live/test";//连接的URL
	const char* flvfilename="test2.flv";//保存的flv文件
	RTMP_LogLevel loglevel=RTMP_LOGINFO;//设置RTMP信息等级
	RTMP_LogSetLevel(loglevel);//设置信息等级
//	RTMP_LogSetOutput(FILE*fp);//设置信息输出文件
	MYINIT();
	PlayFlv(rtmpurl,flvfilename,0);
	MYCLEAR();
	return 0;
}

void PlayFlv(const char*rtmpurl,const char*flvfilename,int bLive){
	RTMP*rtmp=NULL;//rtmp应用指针
	RTMPPacket*packet=NULL;//rtmp包结构
	char url[256]={0};
	int buffsize=1024;
	char*buff=(char*)malloc(buffsize);
	double duration=-1;
	int nRead=0;
	FILE*fp=fopen(flvfilename,"wb");
	long 	countbuffsize=0;
	rtmp=RTMP_Alloc();//申请rtmp空间
	RTMP_Init(rtmp);//初始化rtmp设置
	rtmp->Link.timeout=25;//超时设置
	//由于crtmpserver是每个一段时间(默认8s)发送数据包,需大于发送间隔才行
	strcpy(url,rtmpurl);
	RTMP_SetupURL(rtmp,url);
	if (bLive){
		//设置直播标志
		rtmp->Link.lFlags|=RTMP_LF_LIVE;
	}
	RTMP_SetBufferMS(rtmp,100*1000);//10s
	if(!RTMP_Connect(rtmp,NULL)){
		printf("Connect Server Err\n");
		goto end;
	}
	if(!RTMP_ConnectStream(rtmp,0)){
		printf("Connect stream Err\n");
		goto end;
	}
#if 1
	packet=(RTMPPacket*)malloc(sizeof(RTMPPacket));//创建包
	memset(packet,0,sizeof(RTMPPacket));	
	RTMPPacket_Reset(packet);//重置packet状态
	while (RTMP_GetNextMediaPacket(rtmp,packet)){
		if(packet->m_packetType==0x09&&packet->m_body[0]==0x17)
			printf("TimeStamp:%u\n",packet->m_nTimeStamp);
		RTMPPacket_Free(packet);
		RTMPPacket_Reset(packet);//重置packet状态
	}
#else
	//它直接输出的就是FLV文件,包括FLV头,可对流按照flv格式解析就可提前音频,视频数据
	while(nRead=RTMP_Read(rtmp,buff,buffsize)){
		fwrite(buff,1,nRead,fp);
		countbuffsize+=nRead;
		printf("DownLand...:%0.2fkB",countbuffsize*1.0/1024);
	}
#endif
end:
	if(fp){
		fclose(fp);
		fp=NULL;
	}
	if(buff){
		free(buff);
		buff=NULL;
	}
	if(rtmp!=NULL){
		RTMP_Close(rtmp);//断开连接
		RTMP_Free(rtmp);//释放内存
		rtmp=NULL;
	}
}

int MYINIT(){
#ifdef WIN32
	WORD version;
	WSADATA wsaData;
	version=MAKEWORD(2,2);
	if(WSAStartup(version,&wsaData)!=0){
		return 0;
	}
#endif
	return 1;
}
void MYCLEAR(){
#ifdef WIN32
	WSACleanup();
#endif
}