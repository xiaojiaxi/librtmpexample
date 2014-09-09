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

/*
#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"lib/librtmp.lib")
#endif
*/

//RTMP_XXX()返回0表示失败，1表示成功


void PublishFlv(const char*rtmpurl,const char*flvfilename);

int MYINIT();//初始化相关
void MYCLEAR();//清除相关

int main(){
	const char* rtmpurl="rtmp://192.168.1.100:1935/live/test";//连接的URL
	const char* flvfilename="test.flv";//读取的flv文件
	RTMP_LogLevel loglevel=RTMP_LOGINFO;//设置RTMP信息等级
	RTMP_LogSetLevel(loglevel);//设置信息等级
//	RTMP_LogSetOutput(FILE*fp);//设置信息输出文件
	MYINIT();
	PublishFlv(rtmpurl,flvfilename);
	MYCLEAR();
	return 0;
}

void PublishFlv(const char*rtmpurl,const char*flvfilename){
	RTMP*rtmp=NULL;//rtmp应用指针
	RTMPPacket*packet=NULL;//rtmp包结构

	uint32_t start=0;
	uint32_t lasttime=0;
	uint32_t maxbuffsize=1024;
	char url[256]={0};
	MyFLV*myflv=MyFlvOpen(flvfilename);
	MyFrame myframe={0};

    printf("rtmpurl:%s\nflvfile:%s\nsend data ...\n",rtmpurl,flvfilename);
	if(myflv==NULL){
		printf("OpenFlvFile Err:%s\n",flvfilename);
		goto end;
	}
	printf("duration:%u\n",myflv->duration);

	rtmp=RTMP_Alloc();//申请rtmp空间
	RTMP_Init(rtmp);//初始化rtmp设置
	rtmp->Link.timeout=5;//设置连接超时，单位秒，默认30秒
////////////////////////////////连接//////////////////
	strcpy(url,rtmpurl);
	RTMP_SetupURL(rtmp,url);//设置url
	RTMP_EnableWrite(rtmp);//设置可写状态
	//连接服务器
	if (!RTMP_Connect(rtmp,NULL)){
		printf("Connect Err\n");
		goto end;
	}
	//创建并发布流(取决于rtmp->Link.lFlags)
	if (!RTMP_ConnectStream(rtmp,0)){
		printf("ConnectStream Err\n");
		goto end;
	}
	packet=(RTMPPacket*)malloc(sizeof(RTMPPacket));//创建包
	memset(packet,0,sizeof(RTMPPacket));	
	RTMPPacket_Alloc(packet,maxbuffsize);//给packet分配数据空间
	RTMPPacket_Reset(packet);//重置packet状态
	packet->m_hasAbsTimestamp = 0; //绝对时间戳
	packet->m_nChannel = 0x04; //通道
	packet->m_nInfoField2 = rtmp->m_stream_id;

////////////////////////////////////////发送数据//////////////////////
	start=time(NULL)-1;
	myframe.bkeyframe=1;

	while(TRUE){
		if(myflv->beof){
			break;
		}	
		if(((time(NULL)-start)<(myflv->currenttime/1000))&&myframe.bkeyframe){	
			//发的太快就等一下
			if(myflv->currenttime>lasttime){
				printf("TimeStamp:%8lu ms\n",myflv->currenttime);
				lasttime=myflv->currenttime;
			}
#ifdef WIN32
			Sleep(1000);
#else			
			sleep(1);
#endif
			continue;
		}	

		myframe=MyFlvGetFrameInfo(myflv,packet->m_body,maxbuffsize);
		if(myframe.breadbuf==0){
			if(maxbuffsize<myframe.datalength){
			    printf("ChangeMaxBuffSize %u->%u\n",maxbuffsize,myframe.datalength);
				maxbuffsize=myframe.datalength;
				RTMPPacket_Alloc(packet,maxbuffsize);//给packet分配数据空间
				RTMPPacket_Reset(packet);//重置packet状态
				packet->m_hasAbsTimestamp = 0; //绝对时间戳
				packet->m_nChannel = 0x04; //通道
				packet->m_nInfoField2 = rtmp->m_stream_id;
				myframe=MyFlvGetFrameInfo(myflv,packet->m_body,maxbuffsize);
			}
			if(myframe.breadbuf==0){
				printf("ReadData Err Is Not Enought Buffer:%d  %u %u\n",
				maxbuffsize<myframe.datalength,	maxbuffsize,myframe.datalength);
				goto end;
			}
		}		
//		printf("Type:%u DateLength:%u Timestamp:%u\n",myframe.type,myframe.datalength,myframe.timestamp);		
		if(myframe.type==0x08||myframe.type==0x09){
			packet->m_nTimeStamp = myframe.timestamp; 
			packet->m_packetType=myframe.type;
			packet->m_nBodySize=myframe.datalength;
			if (!RTMP_IsConnected(rtmp)){
				printf("rtmp is not connect\n");
				break;
			}
			if (!RTMP_SendPacket(rtmp,packet,0)){
				printf("Send Err\n");
				break;
			}
		}

		myframe=MyFlvGetFrameInfo(myflv,NULL,0);		
	}
	printf("\nSend Data Over\n");
end:
	if(rtmp!=NULL){
		RTMP_Close(rtmp);//断开连接
		RTMP_Free(rtmp);//释放内存
		rtmp=NULL;
	}
	if(packet!=NULL){
		RTMPPacket_Free(packet);//释放内存
		free(packet);
		packet=NULL;
	}
	myflv=MyFlvClose(myflv);
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

