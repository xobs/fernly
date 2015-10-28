#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/types.h>
#include <fcntl.h>

#include <pcap.h>

struct usb_pkt {
	uint8_t ignored1[16];
	uint8_t incoming;
	uint8_t ignored2[6];
	uint32_t length;
	uint8_t data[0];
} __attribute__((__packed__));

static int get_next_pkt(pcap_t *pcap, struct usb_pkt **pkt) {
	int ret;
	struct pcap_pkthdr *header;

	do {
		ret = pcap_next_ex(pcap, &header, (const uint8_t **)pkt);
		if (ret != 1)
			break;
	} while (((*pkt)->incoming));

	return (ret == 1);
}

int main(int argc, char **argv) {
	char *filename;
	char pcap_err[PCAP_ERRBUF_SIZE];
	pcap_t *pcap;
	struct usb_pkt *pkt;
	uint32_t pkt_index = 0;
	
	if (argc != 2) {
		printf("Usage: %s [dumpfile.pcap]\n", argv[0]);
		return 1;
	}

	filename = argv[1];
	pcap = pcap_open_offline(filename, pcap_err);

	if (!pcap) {
		fprintf(stderr, "Unable to open pcap file: %s\n", pcap_err);
		return 1;
	}

	/* Seek to offset 598, which is where command 0xd7 starts */
	while (get_next_pkt(pcap, &pkt)) {
		pkt_index++;

		if ((pkt->length == 1) && (pkt->data[0] == 0xd7)) {
			uint32_t addr;
			uint32_t bytes;
			uint32_t sig_bytes;
			printf("Found download at offset %d\n", pkt_index);

			get_next_pkt(pcap, &pkt);
			memcpy(&addr, pkt->data, 4);
			printf("Writing to address 0x%08x\n", be32toh(addr));

			get_next_pkt(pcap, &pkt);
			memcpy(&bytes, pkt->data, 4);
			printf("Writing %d bytes\n", be32toh(bytes));

			get_next_pkt(pcap, &pkt);
			memcpy(&sig_bytes, pkt->data, 4);
			printf("Signature is %d bytes\n", be32toh(sig_bytes));

			printf("\n");

			uint8_t data[be32toh(bytes)];
			uint32_t copied = 0;
			while (copied < be32toh(bytes)) {
				int ret;
				ret = get_next_pkt(pcap, &pkt);
				if (!ret) {
					fprintf(stderr, "Packet cut short\n");
					break;
				}
				memcpy(data + copied, pkt->data, pkt->length);
				copied += pkt->length;
			}

			char name[32];
			snprintf(name, sizeof(name) - 1, "prog-0x%08x", be32toh(addr));
			int fd = open(name, O_WRONLY | O_CREAT, 0777);
			if (fd == -1) {
				perror("Unable to open file for writing");
				return 1;
			}
			write(fd, data, sizeof(data));
			close(fd);
		}
	}

	return 0;
}
