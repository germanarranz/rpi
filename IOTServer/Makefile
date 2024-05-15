LIBS= -lpthread -lm #Libraries used if needed
#CXX=/home/ubuntu/Documents/rpi/build/host/usr/bin/aarch64-buildroot-linux-gnu-gcc
SRCS= server.c

BIN=server
CFLAGS+= -g -O0
OBJS=$(subst .cpp,.o,$(SRCS))
all : $(BIN)
$(BIN): $(OBJS)
	@echo [link] $@
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
%.o: %.cpp
	@echo [Compile] $<
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	rm -rf *.o $(BIN)