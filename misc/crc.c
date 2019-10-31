/******************************************************************************
*                                                                             *
*               Cyclic Redundancy Check (CRC) functions                       *
*                                                                             *
******************************************************************************/

#include <stdio.h>

/*
*   crc_clear:
*	This function clears the CRC to zero. It should be called prior to
*	the start of the processing of a block for both received messages,
*	and messages to be transmitted.
*
*	Calling sequence:
*
*	unsigned int crc;
*	crc = crc_clear();
*/
unsigned int crc_clear()
{
	return(0);
}
/*
*   crc_update:
*	this function must be called once for each character which is
*	to be included in the CRC for messages to be transmitted.
*	This function is called once for each character which is included
*	in the CRC of a received message, AND once for each of the two CRC
*	characters at the end of the received message. If the resulting
*	CRC is zero, then the message has been correctly received.
*
*   Calling sequence:
*
*	crc = crc_update(crc,next_char);
*/
unsigned int crc_update(crc,crc_char)
unsigned int crc;
unsigned char crc_char;
{
	long x;
	unsigned int i;

/* "x" will contain the character to be processed in bits 0-7 and the CRC    */
/* in bits 8-23. Bit 24 will be used to test for overflow, and then cleared  */
/* to prevent the sign bit of "x" from being set to 1. Bits 25-31 are not    */
/* used. ("x" is treated as though it is a 32 bit register).                 */
	x = ((long)crc << 8) + crc_char;    /* Get the CRC and the character */

/* Repeat the following loop 8 times (for the 8 bits of the character).      */
	for(i = 0;i < 8;i++)
	{

/* Shift the high-order bit of the character into the low-order bit of the   */
/* CRC, and shift the high-order bit of the CRC into bit 24.                 */
		x = x << 1;                        /* Shift "x" left one bit */

/* Test to see if the old high-order bit of the CRC was a 1.                 */
		if(x & 0x01000000)                     /* Test bit 24 of "x" */

/* If the old high-order bit of the CRC was a 1, exclusive-or it with a one  */
/* to set it to 0, and exclusive-or the CRC with hex 1021 to produce the     */
/* CCITT-recommended CRC generator of: X**16 + X**12 + X**5 + 1. To produce  */
/* the CRC generator of: X**16 + X**15 + X**2 + 1, change the constant from  */
/* 0x01102100 to 0x01800500. This will exclusive-or the CRC with hex 8005    */
/* and produce the same CRC that IBM uses for their synchronous transmission */
/* protocols.                                                                */
			x = x ^ 0x01102100;     /* Exclusive-or "x" with a...*/
                                              /* ...constant of hex 01102100 */
/* And repeat 8 times.                                                       */
	}                                               /* End of "for" loop */

/* Return the CRC as the 16 low-order bits of this function's value.         */
	return(((x & 0x00ffff00) >> 8)); /* AND off the unneeded bits and... */
                                  /* ...shift the result 8 bits to the right */

}
/*
*   crc_finish:
*	This function must be called once after all the characters in a block
*	have been processed for a message which is to be TRANSMITTED. It
*	returns the calculated CRC bytes, which should be transmitted as the
*	two characters following the block. The first of these 2 bytes
*	must be taken from the high-order byte of the CRC, and the second
*	must be taken from the low-order byte of the CRC. This routine is NOT
*	called for a message which has been RECEIVED.
*
*   Calling sequence:
*
*	crc = crc_finish(crc);
*/
unsigned int crc_finish(crc)
unsigned int crc;
{
/* Call crc_update twice, passing it a character of hex 00 each time, to     */
/* flush out the last 16 bits from the CRC calculation, and return the       */
/* result as the value of this function.                                     */
	return(crc_update(crc_update(crc,'\0'),'\0'));

}

/*
* This is a sample of the use of the CRC functions, which calculates the
* CRC for a 1-character message block, and then passes the resulting CRC back
* into the CRC functions to see if the "received" 1-character message and CRC
* are correct.
*/
main()
{

	unsigned int crc;                                     /* The calculated CRC */
	unsigned char crc_char;                            /* The 1-character message */
	unsigned char x, y;            /* 2 places to hold the 2 "received" CRC bytes */

	crc_char = 'A';                    /* Define the 1-character message */
	crc = crc_clear();      /* Reset the CRC to "transmit" a new message */
	crc = crc_update(crc,crc_char);   /* Update the CRC for the first... */
                                   /* ...(and only) character of the message */
	crc = crc_finish(crc);        /* Finish the transmission calculation */
	x = (unsigned char)((crc & 0xff00) >> 8);  /* Extract the high-order CRC byte */
	y = (unsigned char)(crc & 0x00ff);          /* And extract the low-order byte */
	printf("%04x\n",crc);                           /* Print the results */

	crc = crc_clear();                 /* Prepare to "receive" a message */
	crc = crc_update(crc,crc_char);   /* Update the CRC for the first... */
                                   /* ...(and only) character of the message */
	crc = crc_update(crc,x);     /* Pass both bytes of the "received"... */
	crc = crc_update(crc,y);           /* ...CRC through crc_update, too */
	printf("%04x\n",crc);    /* If the result was 0, then the message... */
                                            /* ...was received without error */

}

