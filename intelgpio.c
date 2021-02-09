#include <stdio.h>
#include <getopt.h>
#include <sys/io.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#define GPIO_BASE	0x48
#define GP_LVL		0x0c
#define GP_LVL2		0x38
#define GP_LVL3		0x48
#define MAX_GPIO_NUMBER	75
#define LPC_DEV_CONFIG	"/sys/bus/pci/devices/0000:00:1f.0/config"

void usage(const char *name)
{
	printf("usage: %s OPTIONS\n", name);
	printf("Options:\n"
	       "	-h: Print this help\n"
	       "	-n: GPIO pin number\n");
}

int get_gpio(int base, int num)
{
	static const int gpio_reg_offsets[] = {GP_LVL, GP_LVL2, GP_LVL3};
	int index, bit;

	index = num / 32;
	bit = num % 32;

	return (inl(base + gpio_reg_offsets[index]) >> bit) & 1;
}

int main(int argc, char *argv[])
{
	int c;
	int num = -1;
	uint16_t base;
	while ((c = getopt(argc, argv, "hn:")) != -1) {
		switch (c) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 'n':
			num = strtoul(optarg, NULL, 10);
			if (num > MAX_GPIO_NUMBER) {
				fprintf(stderr, "error: invalid number\n");
				return 1;
			}
			break;
		case '?':
			return -1;
		}
	}

	if (num == -1) {
		usage(argv[0]);
		return 0;
	}

	if (geteuid() != 0) {
		fprintf(stderr, "error: you must be root\n");
		return 1;
	}

	FILE *f = fopen(LPC_DEV_CONFIG, "r");
	if (!f) {
		fprintf(stderr, "failed to open %s: %s\n", LPC_DEV_CONFIG, strerror(errno));
		return 1;
	}

	fseek(f, GPIO_BASE, SEEK_SET);
	fread(&base, 2, 1, f);
	fclose(f);

	base &= 0xfffe;

	if (ioperm(base, 0x100, 1)) {
		fprintf(stderr, "ioperm: %s\n", strerror(errno));
		exit(1);
	}

	int val = get_gpio(base, num);
	printf("%d\n", val);

	return 0;
}