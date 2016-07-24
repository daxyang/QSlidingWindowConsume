TARGET=libQSlidingWindowConsume.so
CC=g++

.PHONY:clean

$(TARGET):QSlidingWindowConsume.o
	$(CC) -shared -fPIC -o $(TARGET) QSlidingWindowConsume.o

QSlidingWindowConsume.o:QSlidingWindowConsume.cpp QSlidingWindowConsume.h
	$(CC) -c QSlidingWindowConsume.cpp QSlidingWindowConsume.h

install:
	cp -f $(TARGET)  /usr/local/lib
clean:
	rm -f *.o* *.so*
