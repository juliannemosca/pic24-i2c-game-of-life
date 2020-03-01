#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define GAME_WIDTH   30
#define GAME_HEIGHT  30

#define I2CBUS "/dev/i2c-0"
#define I2C_SLAVE_DEV_ADDR 0x77
#define ARRAY_ELEMENTS(x)  (sizeof(x) / sizeof(x[0]))
static int fd_i2c = -1;

int i2c_init_bus(void)
{
    if (fd_i2c >= 0) // device file already open
        return 0;

    if ((fd_i2c = open(I2CBUS, O_RDWR)) < 0) {
        fprintf(stderr, "i2c bus open error on %s", I2CBUS);
        return -1;
    }
    return 0;
}

int i2c_close_bus(void)
{
    if (fd_i2c < 0) // device file already closed
        return 0;

    if (-1 == close(fd_i2c)) {
        fprintf(stderr, "could not close i2c bus\n");
        return -1;
    }
    fd_i2c = -1;
    return 0;
}

int i2c_read(int addr, unsigned char * buf, unsigned int len)
{
	if (!len)
		return 0;

	if (!buf) {
		fprintf(stderr, "invalid argument: null buffer");
		return -1;
	}
	if (fd_i2c < 0) {
		fprintf(stderr, "i2c bus not open");
		return -1;
	}

    struct i2c_msg msg[] = {{addr, I2C_M_RD, len, buf}};
	struct i2c_rdwr_ioctl_data data = {msg, 1};

	if (ioctl(fd_i2c, I2C_RDWR, &data) == -1) {
        fprintf(stderr,
                "failed to read %u bytes from i2c device 0x%02x (%s)",
                len, addr, strerror(errno));
		return -1;
	}

	return 0;
}

void game_show(const unsigned char * read_buf,
               const unsigned int len)
{
	int n = 0;
	fprintf(stdout, "\033[H");
	for (n = 0; n < len; n++) {
        if ((n % 30) == 0)
			fprintf(stdout, "\033[E");

		fprintf(stdout, read_buf[n] ? "\033[07m  \033[m" : "  "); 
	}
}

int main()
{
    const unsigned int len = (GAME_WIDTH * GAME_HEIGHT);
    unsigned char read_buf[len];
    int errors = 0;

    if (i2c_init_bus() != 0)
        return -1;

    while(1) {

    retry:
        if (0 != i2c_read(I2C_SLAVE_DEV_ADDR, read_buf, len)) {
            errors++;
            goto retry; // if this loops forever... you know why! ;-)
		}

        game_show(read_buf, len);

        fprintf(stdout,
                "\n\n=============================="
                "==============================\n");
        fprintf(stdout, "i2c read errors: %d", errors);
        fprintf(stdout,
                "\n=============================="
                "==============================\n\n");

        errors = 0;
        fflush(stdout);

        // give the pic some time to recover
        // from such an exhausting computation...
        usleep(1000 * 500);
    }

    if (i2c_close_bus() != 0)
        return -1;

    return 0;
}
