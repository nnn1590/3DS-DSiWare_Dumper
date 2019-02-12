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
	printf("exporting:%d %016llX to\n%s...\n", op, tid, fpath);
	res = AM_ExportTwlBackup(tid, op, workbuf, 0x20000, fpath);
	printf("twl export: %08X %s\n\n",(int)res, res ? "FAILED!" : "SUCCESS!");
	
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
	
	u8 *buf = (u8*)malloc(BUF_SIZE);
	Result res = nsInit();
	printf("nsInit: %08X\n",(int)res);
	res = amInit();
	printf("amInit: %08X\n",(int)res);
	res = AM_GetTWLPartitionInfo(&info);
	printf("twlInfo: %08X\n\n",(int)res);
	res = export(0x0004800542383841, op, buf);
	svcSleepThread(SECOND(7));

    free(buf);
	nsExit();
	amExit();
	gfxExit();
	
	return 0;
}