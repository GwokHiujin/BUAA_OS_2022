/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 **/
void checkWriteDev(void *va, u_int dev, u_int len) {
	if (syscall_write_dev((u_int) va, dev, len) < 0) {
		user_panic("ide_error: Check Write Dev error!\n");
	}
}

void checkReadDev(void *va, u_int dev, u_int len) {
	if (syscall_read_dev((u_int) va, dev, len) < 0) {
		user_panic("ide_error: Check Read Dev error!\n");	
	}
}

#define DISK_ADDR_OFF      0x13000000
#define DISK_ADDR_ID       0x13000010
#define DISK_ADDR_OP       0x13000020
#define DISK_ADDR_STAT     0x13000030
#define DISK_ADDR_BUF      0x13004000

void
ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{
	/*
	// 0x200: the size of a sector: 512 bytes.
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int op = 0;

	while (offset_begin < offset_end) {
		// Your code here
		// error occurred, then panic.
		checkWriteDev(&offset_begin, DISK_ADDR_OFF, 4);
		checkWriteDev(&diskno, DISK_ADDR_ID, 4);
		checkWriteDev(&op, DISK_ADDR_OP, 4);		//read - 0
		u_int ret;
		checkReadDev(&ret, DISK_ADDR_STAT, 4);
		if (!ret) {
			user_panic("ide_read_error: Cannot read from disk!\n");
		}
		checkReadDev(dst, DISK_ADDR_BUF, 0x200);
		offset_begin += 0x200;
		dst += 0x200;
	}
	*/
	u_int offset, status;
	static u_int op = 0;
	int i;
	
	offset = secno * BY2SECT;

	for (i = 0; i < nsecs; i++) {
		checkWriteDev(&offset, DISK_ADDR_OFF, 4);
		checkWriteDev(&diskno, DISK_ADDR_ID, 4);
		checkWriteDev(&op, DISK_ADDR_OP, 4);
		checkReadDev(&status, DISK_ADDR_STAT, 4);
		if (!status) {
			user_panic("ide_read: Cannot read from disk!\n");	
		}	
		checkReadDev(dst, DISK_ADDR_BUF, BY2SECT);
		offset += BY2SECT;
		dst += BY2SECT;
	}
}


// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
	// Your code here
	int offset = secno * BY2SECT;
	static u_int op = 1;
	u_int ret;
	int i;

	// DO NOT DELETE WRITEF !!!
	//writef("diskno: %d\n", diskno);

	// while ( < ) {
		// copy data from source array to disk buffer.

		// if error occur, then panic.
	// }
	/*
	while (offset_begin < offset_end) {
		checkWriteDev(&offset_begin, DISK_ADDR_OFF, 4);
        checkWriteDev(&diskno, DISK_ADDR_ID, 4);
        checkWriteDev(&op, DISK_ADDR_OP, 4);        //write - 1
        checkReadDev(&ret, DISK_ADDR_STAT, 4);
        if (!ret) {
            user_panic("ide_write_error: Cannot write disk!\n");
        }
		checkReadDev(src, DISK_ADDR_BUF, 0x200);
        offset_begin += 0x200;
        src += 0x200;
	}
	*/
	for (i = 0; i < nsecs; i++) {
		checkWriteDev(&offset, DISK_ADDR_OFF, 4);
		checkWriteDev(&diskno, DISK_ADDR_ID, 4);
		checkWriteDev(src, DISK_ADDR_BUF, BY2SECT);
		checkWriteDev(&op, DISK_ADDR_OP, 4);
		checkReadDev(&ret, DISK_ADDR_STAT, 4);
		if (!ret) {
			user_panic("ide_write: Cannot write to disk!\n");	
		}
		offset += BY2SECT;
		src += BY2SECT;	
	}
}
