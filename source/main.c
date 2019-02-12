#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <3ds.h>
#define SECOND(x) (x*1000UL*1000UL*1000UL)
PrintConsole topScreen, bottomScreen;
AM_TWLPartitionInfo info;

Result export(u64 tid, u8 op, u8 *workbuf){
	Result res;
	char fpath[256]={0};
	memset(fpath, 0, 128);
	sprintf(fpath,"sdmc:/%08lX.BIN",(u32)tid);
	if(access(fpath, F_OK ) != -1 ) {
		printf("DSiWare already exists on SD\n\n");
		return 1;
	}
	printf("Exporting:%d %016llX to\n%s...\n", op, tid, fpath);
	res = AM_ExportTwlBackup(tid, op, workbuf, 0x20000, fpath);
	printf("Exporting %s\n\n", res ? "failed." : "succeeded.");
	
	return res;
}



int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	consoleSelect(&topScreen);
	u32 BUF_SIZE = 0x20000;
	u8 op=5;
	Result res;
	
	u8 *buf = (u8*)malloc(BUF_SIZE);

	res = amInit();
	if (res) {
		printf("Unable to initialize AM service\n");
		svcSleepThread(SECOND(7));
		return 1;
	}
	printf("amInit: %08X\n",(int)res);
	res = AM_GetTWLPartitionInfo(&info);
	if (res) {
		printf("Unable to get DSiWare Parition information.\n");
		svcSleepThread(SECOND(7));
		return 1;
	}
//	printf("twlInfo: %08X\n\n",(int)res);
	printf("Attempting to dump DSInternet\n");
	res = export(0x0004800542383841, op, buf);
	if (res) {
		printf("Attempting to dump US/EU/JP DS Download Play\n");
		res = export(0x00048005484E4441, op, buf);
		//if (res) {
			//printf("attempting to dump ")
			//res = export(0x000480044b385545, op, buf);
			//if (res) {
				//res = export(0x000480044b454e4a, op, buf);
				if (res) {
					printf("Failed to dump System DSiWare. Manually dump any DSiWare application using data management\n");
				}
			//}
		//}
	}
	svcSleepThread(SECOND(7));

    free(buf);

	amExit();
	gfxExit();
	
	return 0;
}