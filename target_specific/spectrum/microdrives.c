/*
    Demonstrates the power of the low-level microdrive functions:
    - seeks the drives and gives a status report
    - for the existing drives, searches the 'run' program and,
      if it exists, displays an 'A' icon near the drive.
*/

 #include <stdio.h>
 #include <zxinterface1.h>
 #include <games.h>
 #include <graphics.h>

 char drvlist[9];
 char bootlist[9];
 int i,drvcount,rocount,runcount;
 int j,sectcnt;
 
 struct M_CHAN myrec;

char numbers[] = {
 3, 5, 0x40 , 0x40 , 0x40 , 0x40 , 0x40 ,
 3, 5, 0xE0 , 0x20 , 0xE0 , 0x80 , 0xE0 ,
 3, 5, 0xE0 , 0x20 , 0xE0 , 0x20 , 0xE0 ,
 3, 5, 0xA0 , 0xA0 , 0xE0 , 0x20 , 0x20 ,
 3, 5, 0xE0 , 0x80 , 0xE0 , 0x20 , 0xE0 ,
 3, 5, 0xE0 , 0x80 , 0xE0 , 0xA0 , 0xE0 ,
 3, 5, 0xE0 , 0x20 , 0x20 , 0x20 , 0x20 ,
 3, 5, 0xE0 , 0xA0 , 0xE0 , 0xA0 , 0xE0  };

char sprite[] = { 
 15, 11, 0x3F , 0xF8 , 0x40 , 0x04 , 0x90 , 0x02 , 0x80 , 0x02 , 0x80 , 0x02 , 0x8F ,
 0xE2 , 0x88 , 0x22 , 0x8F , 0xE2 , 0x80 , 0x02 , 0x40 , 0x04 , 0x3F , 0xF8 ,
 15, 11, 0x3F , 0xF8 , 0x40 , 0x04 , 0x90 , 0x02 , 0x80 , 0x02 , 0x80 , 0x02 , 0x8F ,
 0xE2 , 0x8F , 0xE2 , 0x8F , 0xE2 , 0x80 , 0x02 , 0x40 , 0x04 , 0x3F , 0xF8 ,
 15, 11, 0xEF , 0xEE , 0x70 , 0x1C , 0xB8 , 0x3A , 0x9C , 0x72 , 0x8E , 0xE2 , 0x87 ,
 0xC2 , 0x8E , 0xE2 , 0x9C , 0x72 , 0xB8 , 0x3A , 0x70 , 0x1C , 0xEF , 0xEE  
};

//char arrow_msk[] = { 7, 8, 0x10 , 0x38 , 0x7C , 0xFE , 0xFE , 0xFE , 0x7C , 0x7C  };
char arrow[] = { 7, 8, 0x00 , 0x10 , 0x28 , 0x44 , 0x6C , 0x28 , 0x38 , 0x00  };

char a_char[] = { 6, 6, 0x00 , 0x30 , 0x48 , 0x78 , 0x48 , 0x00  };
char a_mask[] = { 6, 6, 0x30 , 0x78 , 0xFC , 0xFC , 0xFC , 0xFC  };

 int main(void)
 {
    drvcount=0;  rocount=0;  runcount=0;
    clg();

/*
    if (!if1_installed()) {
    	printf("Interface 1 not present ");
    	return;
    }
*/

    printf("\n\n\nSeeking microdrives...\n");
    for (i=1;i<=8; i++)  {
       drvlist[i]=if1_mdv_status(i);
       if (drvlist[i]==1) rocount++;

       putsprite (spr_or,i*26-4,0,&numbers[(i-1)*7]);
       putsprite (spr_or,i*26,2,&sprite[drvlist[i]*24]);

       if (drvlist[i]!=2) {
       	 drvcount++;
       	 putsprite (spr_or,i*26+4,13,arrow);

         if (if1_load_record(i, "run", 0, myrec) != -1) {
            runcount++;
            printf ( "%u: volume %s has the 'run' autoboot program\n",i, if1_getname(myrec.hdname) );
            putsprite (spr_and,i*26+11,10,a_mask);
            putsprite (spr_or,i*26+11,10,a_char);
         }

         putsprite (spr_and,i*26+4,13,arrow);
       }
    }
    printf("done.\n\n");

    printf("This program has ");
    if (!if1_from_mdv()) printf("not ");
    printf("been loaded from microdrive\n\n");

    if (drvcount==0) {
       printf("No ready drives found");
       return;
    }

    printf("%u drive(s) ready, %u read-only cartridge(s).\n",drvcount,rocount);
    printf("%u cartridge(s) have 'run' for autoboot\n\n",runcount);
 }
