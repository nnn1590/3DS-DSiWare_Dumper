#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <3ds.h>
#define SECOND(x) (x*1000ULL*1000ULL*1000ULL)
PrintConsole topScreen, bottomScreen;
AM_TWLPartitionInfo info;

Result export(u64 tid, u8 op, u8 *workbuf){
	Result res;
	char fpath[256]={0};
	memset(fpath, 0, 128);
	sprintf(fpath,"sdmc:/%08lX.BIN",(u32)tid);
	if(access(fpath, F_OK ) != -1 ) {
		//printf("DSiWare already exists on SD\n\n");
		return 1;
	}
	//printf("Exporting:%d %016llX to\n%s...\n", op, tid, fpath);
	res = AM_ExportTwlBackup(tid, op, workbuf, 0x20000, fpath);
	//printf("Exporting %s\n\n", res ? "failed." : "succeeded.");
	
	return res;
}



int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	consoleSelect(&bottomScreen);
	u32 BUF_SIZE = 0x20000;
	u8 op=5;
	Result res;
	
	u8 *buf = (u8*)malloc(BUF_SIZE);
	printf("Initializing AM services\n");
	res = amInit();
	if (res) {
		printf("Unable to initialize AM service\n");
		svcSleepThread(SECOND(7));
		return 1;
	}
	//printf("amInit: %08X\n",(int)res);
	res = AM_GetTWLPartitionInfo(&info);
	if (res) {
		printf("Unable to get DSiWare Parition information.\n");
		svcSleepThread(SECOND(7));
		return 1;
	}
	printf("Retrieving number of titles\n");
	u32 title_count=0;
	u64 dsiTitle[50]={0};
	res = AM_GetTitleCount(MEDIATYPE_NAND, &title_count);
	if (res) {
		printf("Failed to get title count.\n");
	}else{
		u64 *titleID; 
		titleID=calloc(title_count,sizeof(u64));
		u32 titles_read=0;
		res = AM_GetTitleList(&titles_read,MEDIATYPE_NAND,title_count,titleID);
		if (res) {
			printf("failed to get title ids.\n");
		}else{
			//printf("%lu titles read\n", titles_read);
			//svcSleepThread(SECOND(10));
			title_count=0;
			for (int i=0;i<titles_read;i++) {
				u16 uCategory = (u16)((titleID[i] >> 32) & 0xFFFF);
				//printf("category: %lx\n", (long int)uCategory);
				//svcSleepThread(SECOND(5));
				if (uCategory==0x8004 || uCategory==0x8005 || uCategory==0x800F || uCategory==0x8015 ) {
					title_count+=1;
					//printf("DSi Title: %016llx\n",titleID[i]);
					dsiTitle[title_count] = titleID[i];
				}else{
					//printf("3DS Title:  %016llx",titleID[i]);
				}
			}
		}
		free(titleID);
	}
		//svcSleepThread(SECOND(20));
	//return 0;
//	printf("twlInfo: %08X\n\n",(int)res);
	for (int i=0;i<title_count;i++) {
		if ((dsiTitle[i] & 0xFFFFFFFF) == 0) {
			continue;
		}
		printf("Attempting to dump %08lx:      ",(u32)(dsiTitle[i] & 0xFFFFFFFF));
		res = export(dsiTitle[i], op, buf);
		if (!res) {
			printf("Success\n");
			svcSleepThread(SECOND(20LL));
		    free(buf);
			return 0;
		}else{
			printf("Failed\n");
		}
	}
	printf("Failed to dump System DSiWare. Manually dump any DSiWare application using data management\n");
	svcSleepThread(SECOND(20LL));

    free(buf);

	amExit();
	gfxExit();
	
	return 0;
}