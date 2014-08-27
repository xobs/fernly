#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int fernvale_reset(void) {
	int fd;
	uint8_t b;

	fd = open("/sys/class/gpio/gpio17/value", O_WRONLY);
	b = '0';
	write(fd, &b, 1);
	close(fd);
	
	usleep(250000);

	fd = open("/sys/class/gpio/gpio17/value", O_WRONLY);
	b = '1';
	write(fd, &b, 1);
	close(fd);

	return 0;
}

int fernvale_banner(int fd) {
	uint8_t b;

	while (-1 != read(fd, &b, 1)) {
		if (b == '>')
			return 0;
		printf("%c", b);
		fflush(stdout);
		if (b == '\r')
			printf("\t");
	}
	return -1;
}

int fernvale_print_line(int fd) {
	uint8_t b;

	while (-1 != read(fd, &b, 1)) {
		printf("%c", b);
		if (b == '\n')
			return 0;
	}
	return -1;
}

int main(int argc, char **argv) {
	int serfd, binfd;
	struct stat stats;
	int i;
	
	if (argc != 2) {
		printf("Usage: %s firmware.bin\n", argv[0]);
		exit(1);
	}

	serfd = open("/dev/ttymxc2", O_RDWR);
	if (-1 == serfd) {
		perror("Unable to open serial port");
		exit(1);
	}

	binfd = open(argv[1], O_RDONLY);
	if (-1 == binfd) {
		perror("Unable to open firmware file");
		exit(1);
	}

	if (-1 == fstat(binfd, &stats)) {
		perror("Unable to get file stats");
		exit(1);
	}

	printf("Resetting Fernvale... ");
	fflush(stdout);
	fernvale_reset();
	printf("Ok.\n");

	printf("Waiting for banner... ");
	fflush(stdout);
	fernvale_banner(serfd);
	printf("Ok.\n");

	printf("Writing %d bytes...", (int)stats.st_size);
	{
		uint8_t b;

		b = stats.st_size & 0xff;
		write(serfd, &b, 1);

		b = stats.st_size >> 8 & 0xff;
		write(serfd, &b, 1);

		b = stats.st_size >> 16 & 0xff;
		write(serfd, &b, 1);

		b = stats.st_size >> 24 & 0xff;
		write(serfd, &b, 1);
	}

	for (i = 0; i < stats.st_size; i++) {
		uint8_t b;
		if (-1 == read(binfd, &b, 1)) {
			perror("Error while reading binary");
			exit(1);
		}
		if (-1 == write(serfd, &b, 1)) {
			perror("Error while writing binary to serial port");
			exit(1);
		}
	}

	printf(" Done.\n");

	printf("Result: ");
	fernvale_print_line(serfd);

	close(serfd);
	close(binfd);
	return execl("/usr/bin/screen", "screen", "/dev/ttymxc2", "115200", NULL);
	return 0;
}
