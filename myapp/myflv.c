#include "myflv.h"
MyFLV*MyFlvOpen(const char*filename){
	MyFLV*myflv;
	MyFrame myframe;
	FILE*fp=fopen(filename,"rb");
	if(fp==NULL){
		return NULL;
	}
	myflv=(MyFLV*)malloc(sizeof(MyFLV));
	memset(myflv,0,sizeof(MyFLV));
	myflv->fp=fp;
//////////获取文件总长/////////////////////////////////
	fseek(fp,0,SEEK_END);
	myflv->totalsize=ftell(fp);
//////////获取时间播放/////////////////////////////////
	fseek(fp,myflv->totalsize-4,SEEK_SET);
	ReadU32(&myframe.alldatalength,fp);
	fseek(fp,myflv->totalsize-4-myframe.alldatalength,SEEK_SET);
	myframe=MyFlvGetFrameInfo(myflv,NULL,0);
	myflv->duration=myframe.timestamp;
	myflv->currenttime=0;
///////////////////////////////////////////
	fseek(fp,13,SEEK_SET);
	myflv->startpos=ftell(fp);
	return myflv;
}
MyFrame MyFlvGetFrameInfo(MyFLV*myflv,char*buf,uint32_t len){
	MyFrame myframe={0};
	do{	
	if(!ReadU8(&myframe.type,myflv->fp))
		break;
	if(!ReadU24(&myframe.datalength,myflv->fp))
		break;
	if(!ReadUTime(&myframe.timestamp,myflv->fp))
		break;
	if(!ReadU24(&myframe.streamid,myflv->fp))
		break;
	
	if(myflv->bloop)
		myframe.timestamp+=myflv->duration*myflv->looptimes;
	myflv->currenttime=myframe.timestamp;

	if((buf!=NULL)&&(len>=myframe.datalength)){
		if(fread(buf,1,myframe.datalength,myflv->fp)==myframe.datalength){
			ReadU32(&myframe.alldatalength,myflv->fp);
			myframe.breadbuf=1;
			if(ftell(myflv->fp)>=(long)myflv->totalsize){
				if(myflv->bloop){
					fseek(myflv->fp,myflv->startpos,SEEK_SET);
					myflv->looptimes++;
				}else{
					myflv->beof=1;
				}
			}
		}else{
			break;
		}
	}else{
		uint32_t type=0;	
		if(myframe.type==0x09){
			if(PeekU8(&type,myflv->fp)){
				if(type==0x17){
					myframe.bkeyframe=1;
				}
			}
		}
		fseek(myflv->fp,-11,SEEK_CUR);
	}
	}while(0);
	return myframe;
}

MyFrame*MyFLVFreeMyFrame(MyFrame*myframe){
	if (myframe==NULL){
		return NULL;
	}
	if(myframe->buffer){
		free(myframe->buffer);
		myframe->buffer=NULL;
	}
	free(myframe);
	myframe=NULL;
	return NULL;
}

MyFLV*MyFlvClose(MyFLV*myflv){
	if (myflv==NULL){
		return NULL;
	}
	if(myflv->fp){
		fclose(myflv->fp);
		myflv->fp=NULL;
	}
	myflv->ai=MyFLVFreeMyFrame(myflv->ai);
	myflv->vi=MyFLVFreeMyFrame(myflv->vi);
	free(myflv);
	myflv=NULL;
	return NULL;
}


int ReadU8(uint32_t *u8,FILE*fp){
	if(fread(u8,1,1,fp)!=1)
		return 0;
	return 1;
}
int ReadU16(uint32_t *u16,FILE*fp){
	if(fread(u16,2,1,fp)!=1)
		return 0;
	*u16=HTON16(*u16);
	return 1;
}
int ReadU24(uint32_t *u24,FILE*fp){
	if(fread(u24,3,1,fp)!=1)
		return 0;
	*u24=HTON24(*u24);
	return 1;
}
int ReadU32(uint32_t *u32,FILE*fp){
	if(fread(u32,4,1,fp)!=1)
		return 0;
	*u32=HTON32(*u32);
	return 1;
}
int PeekU8(uint32_t *u8,FILE*fp){
	if(fread(u8,1,1,fp)!=1)
		return 0;
	fseek(fp,-1,SEEK_CUR);
	return 1;
}
int ReadUTime(uint32_t *utime,FILE*fp){
	if(fread(utime,4,1,fp)!=1)
		return 0;
	*utime=HTONTIME(*utime);
	return 1;
}
