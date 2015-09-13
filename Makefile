CC = g++
CFLAGS = -std=c++98 -pedantic -W -Wall
LDFLAGS = `curl-config --libs`
COMPILE = $(CC) $(CFLAGS) -c
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: idleserver

idleserver: $(OBJFILES)
	$(CC) $(LDFLAGS) -o idleserver $(OBJFILES)

%.o: %.cpp
	$(COMPILE) -o $@ $<

clean:
	-rm -f *.o
    
install: idleserver
	mv -f idleserver /usr/local/sbin
	chmod 700 /usr/local/sbin/idleserver
	cp -f idleserver.init /etc/init.d/idleserver
	chmod +x /etc/init.d/idleserver
	/sbin/chkconfig --add idleserver
	/sbin/chkconfig idleserver on
