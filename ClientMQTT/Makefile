LIBS= -lpthread -lm #Libraries used if needed
CXX=/home/gonzalo/Documents/rpi/build/host/usr/bin/aarch64-buildroot-linux-gnu-gcc
SRCS= acc.c color.c main.c

BIN=sensors
CFLAGS+= -g -O0
OBJS=$(subst .cpp,.o,$(SRCS))
all : $(BIN)
$(BIN): $(OBJS)
	@echo [link] $@
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
%.o: %.cpp
	@echo [Compile] $<
	$(CXX) -c $(CFLAGS) $< -o $@
	
clean:
	rm -rf *.o $(BIN)