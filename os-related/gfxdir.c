
// Disk directory listing in proportional text
// Should work on the Sharp OZ-700, CP/M systems with graphics and OSCA (v8z80p)

#include <stdlib.h>
#include <ozdev.h>

main()
{
    static int pos=0;
	static int xpos=0;
    static byte mode=FIND_RESET;
    register char *name;
    ozcls();
    while(NULL!=(name=ozgetnextfilename(mode)))
    {
        if(pos>=(getmaxy()-8))
        {
            ozputs(0,pos+8,"Press any key to continue");
            ozgetch();
            ozcls();
            pos=0;
        }
        mode=0;
        ozputs(xpos,pos,name);
		xpos+=getmaxx()/2;
		if (xpos>getmaxx()/2) {
			pos+=8;
			xpos=0;
		}
    }
    ozputs(0,pos,"Done: press any key to continue");
    ozgetch();
}

