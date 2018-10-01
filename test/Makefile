

CFLAGS += -pthread -lpthread

BINS := test \
	test-fb-ssd1308 \
	test-oled-ssd1308

vpath %.c ./

all: $(BINS)


.PHONY: clean
clean:
	rm -rf $(BINS) .#*
