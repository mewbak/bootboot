/* raspbootcom.c - upload INITRD via serial port to a machine running BOOTBOOT */
/* Copyright (C) 2013 Goswin von Brederlow <goswin-v-b@web.de>
 * minor modifications for BOOTBOOT: 2017 bzt (bztsrc@gitlab)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <endian.h>
#include <stdint.h>
#include <termios.h>
#include "../dist/bootboot.h"

#define BUF_SIZE 65536

struct termios old_tio, new_tio;

void do_exit(int fd, int res) {
    // close FD
    if (fd != -1) close(fd);
    // restore settings for STDIN_FILENO
    if (isatty(STDIN_FILENO)) {
	tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
    }
    exit(res);
}

// open serial connection
int open_serial(const char *dev) {
    // The termios structure, to be configured for serial interface.
    struct termios termios;

    // Open the device, read/write, not the controlling tty, and non-blocking I/O
    int fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
	// failed to open
	return -1;
    }
    // must be a tty
    if (!isatty(fd)) {
        fprintf(stderr, "%s is not a tty\n", dev);
	do_exit(fd, EXIT_FAILURE);
    }

    // Get the attributes.
    if(tcgetattr(fd, &termios) == -1)
    {
        perror("Failed to get attributes of device");
	do_exit(fd, EXIT_FAILURE);
    }

    // So, we poll.
    termios.c_cc[VTIME] = 0;
    termios.c_cc[VMIN] = 0;

    // 8N1 mode, no input/output/line processing masks.
    termios.c_iflag = 0;
    termios.c_oflag = 0;
    termios.c_cflag = CS8 | CREAD | CLOCAL;
    termios.c_lflag = 0;

    // Set the baud rate.
    if((cfsetispeed(&termios, B115200) < 0) ||
       (cfsetospeed(&termios, B115200) < 0))
    {
        perror("Failed to set baud-rate");
	do_exit(fd, EXIT_FAILURE);
    }

    // Write the attributes.
    if (tcsetattr(fd, TCSAFLUSH, &termios) == -1) {
	perror("tcsetattr()");
	do_exit(fd, EXIT_FAILURE);
    }
    return fd;
}

// send initrd to rpi
void send_initrd(int fd, const char *file) {
    int file_fd;
    off_t off;
    uint32_t size;
    ssize_t pos, total=0;
    char *p;
    int done = 0;

    // Set fd blocking
    if (fcntl(fd, F_SETFL, 0) == -1) {
	perror("fcntl()");
	do_exit(fd, EXIT_FAILURE);
    }

    // Open file. If not found, simply continue with terminal
    if (file==NULL || file[0]==0 || (file_fd = open(file, O_RDONLY)) == -1) {
        return;
    }

    // Get initrd size
    off = lseek(file_fd, 0L, SEEK_END);
    if (off >= INITRD_MAXSIZE*1024*1024) {
	fprintf(stderr, "initrd too big\n");
        close(file_fd);
        return;
    }
    // empty file
    if (off == 0) {
        close(file_fd);
        return;
    }
    size = htole32(off);
    lseek(file_fd, 0L, SEEK_SET);

    fprintf(stderr, "### sending initrd %s [%zu byte]\n", file, (size_t)off);

    // send initrd size to RPi
    p = (char*)&size;
    pos = 0;
    while(pos < 4) {
	ssize_t len = write(fd, &p[pos], 4 - pos);
	if (len == -1) {
	    perror("write()");
	    do_exit(fd, EXIT_FAILURE);
	}
	pos += len;
    }
    // wait for OK
    char ok_buf[2];
    p = ok_buf;
    pos = 0;
    while(pos < 2) {
	ssize_t len = read(fd, &p[pos], 2 - pos);
	if (len == -1) {
	    perror("read()");
	    do_exit(fd, EXIT_FAILURE);
	}
	pos += len;
    }
    if (ok_buf[0] != 'O' || ok_buf[1] != 'K') {
	fprintf(stderr, "error after sending size\n");
    close(file_fd);
	return;
    }

    while(!done) {
	char buf[BUF_SIZE];
	ssize_t pos = 0;
	ssize_t len = read(file_fd, buf, BUF_SIZE);
	switch(len) {
	case -1:
	    perror("read()");
	    close(file_fd);
        return;
	case 0:
	    done = 1;
	}
	while(len > 0) {
	    ssize_t len2 = write(fd, &buf[pos], len);
	    if (len2 == -1) {
		perror("write()");
		do_exit(fd, EXIT_FAILURE);
	    }
	    len -= len2;
	    pos += len2;
        total += len2;
        fprintf(stderr,"%3d%% %d / %d\r",total*100/off, total, off);
	}
    }
    close(file_fd);

    // Set fd non-blocking
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
	perror("fcntl()");
	do_exit(fd, EXIT_FAILURE);
    }

    fprintf(stderr, "### finished sending\n");

    return;
}

int main(int argc, char *argv[]) {
    int fd, max_fd = STDIN_FILENO;
    fd_set rfds, wfds, efds;
    char buf[BUF_SIZE];
    size_t start = 0;
    size_t end = 0;
    int done = 0, leave = 0;
    int breaks = 0;

    printf("Raspbootcom V1.0 - BOOTBOOT version\n\n");

    if (argc < 2) {
	printf("USAGE: %s <dev> [file]\n", argv[0]);
	printf("Example: %s /dev/ttyUSB0 BOOTBOOT/INITRD\n", argv[0]);
	exit(EXIT_FAILURE);
    }

    // Set STDIN non-blocking and unbuffered
    if (fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK) == -1) {
	perror("fcntl()");
	exit(EXIT_FAILURE);
    }
    if (isatty(STDIN_FILENO)) {
	// get the terminal settings for stdin
	if (tcgetattr(STDIN_FILENO, &old_tio) == -1) {
	    perror("tcgetattr");
	    exit(EXIT_FAILURE);
	}

	// we want to keep the old setting to restore them a the end
	new_tio=old_tio;

	// disable canonical mode (buffered i/o) and local echo
	new_tio.c_lflag &= (~ICANON & ~ECHO);

	// set the new settings immediately
	if (tcsetattr(STDIN_FILENO, TCSANOW, &new_tio) == -1) {
	    perror("tcsetattr()");
	    do_exit(-1, EXIT_FAILURE);
	}
    }

    while(!leave) {
	// Open device
	if ((fd = open_serial(argv[1])) == -1) {
	    // udev takes a while to change ownership
	    // so sometimes one gets EPERM
	    if (errno == ENOENT || errno == ENODEV || errno == EACCES) {
		fprintf(stderr, "\r### Waiting for %s...\r", argv[1]);
		sleep(1);
		continue;
	    }
	    perror(argv[1]);
	    do_exit(fd, EXIT_FAILURE);
	}
	fprintf(stderr, "### Listening on %s     \n", argv[1]);

	// select needs the largeds FD + 1
	if (fd > STDIN_FILENO) {
	    max_fd = fd + 1;
	} else {
	    max_fd = STDIN_FILENO + 1;
	}

	done = 0;
	start = end = 0;
	while(!done || start != end) {
	    // Watch stdin and dev for input.
	    FD_ZERO(&rfds);
	    if (!done && end < BUF_SIZE) FD_SET(STDIN_FILENO, &rfds);
	    FD_SET(fd, &rfds);

	    // Watch fd for output if needed.
	    FD_ZERO(&wfds);
	    if (start != end) FD_SET(fd, &wfds);

	    // Watch stdin and dev for error.
	    FD_ZERO(&efds);
	    FD_SET(STDIN_FILENO, &efds);
	    FD_SET(fd, &efds);

	    // Wait for something to happend
	    if (select(max_fd, &rfds, &wfds, &efds, NULL) == -1) {
		perror("select()");
		do_exit(fd, EXIT_FAILURE);
	    } else {
		// check for errors
		if (FD_ISSET(STDIN_FILENO, &efds)) {
		    fprintf(stderr, "error on STDIN\n");
		    do_exit(fd, EXIT_FAILURE);
		}
		if (FD_ISSET(fd, &efds)) {
		    fprintf(stderr, "error on device\n");
		    do_exit(fd, EXIT_FAILURE);
		}
		// RPi is ready to recieve more data, send more
		if (FD_ISSET(fd, &wfds)) {
		    ssize_t len = write(fd, &buf[start], end - start);
		    if (len == -1) {
			perror("write()");
			do_exit(fd, EXIT_FAILURE);
		    }
		    start += len;
		    if (start == end) start = end = 0;
		    // shift buffer contents
		    if (end == BUF_SIZE) {
			memmove(buf, &buf[start], end - start);
			end -= start;
			start = 0;
		    }
		}
		// input from the user, copy to RPi
		if (FD_ISSET(STDIN_FILENO, &rfds)) {
		    ssize_t len = read(STDIN_FILENO, &buf[end], BUF_SIZE - end);
		    switch(len) {
		    case -1:
			perror("read()");
			do_exit(fd, EXIT_FAILURE);
		    case 0:
			done = 1;
			leave = 1;
		    }
		    end += len;
		}
		// output from the RPi, copy to STDOUT
		if (FD_ISSET(fd, &rfds)) {
		    char buf2[BUF_SIZE];
		    ssize_t len = read(fd, buf2, BUF_SIZE);
		    switch(len) {
		    case -1:
			perror("read()");
			do_exit(fd, EXIT_FAILURE);
		    case 0:
			done = 1;
		    }
		    // scan output for tripple break (^C^C^C)
		    // send initrd on tripple break, otherwise output text
		    const char *p = buf2;
		    while(p < &buf2[len]) {
			const char *q = index(p, '\x03');
			if (q == NULL) q = &buf2[len];
			if (p == q) {
			    ++breaks;
			    ++p;
			    if (breaks == 3) {
				if (start != end) {
				    fprintf(stderr, "Discarding input after tripple break\n");
				    start = end = 0;
				}
				if(argv[2]!=NULL)
					send_initrd(fd, argv[2]);
				breaks = 0;
			    }
			} else {
			    while (breaks > 0) {
				ssize_t len2 = write(STDOUT_FILENO, "\x03\x03\x03", breaks);
				if (len2 == -1) {
				    perror("write()");
				    do_exit(fd, EXIT_FAILURE);
				}
				breaks -= len2;
			    }
			    while(p < q) {
				ssize_t len2 = write(STDOUT_FILENO, p, q - p);
				if (len2 == -1) {
				    perror("write()");
				    do_exit(fd, EXIT_FAILURE);
				}
				p += len2;
			    }
			}
		    }
		}
	    }
	}
	close(fd);
    }

    do_exit(-1, EXIT_SUCCESS);
}
