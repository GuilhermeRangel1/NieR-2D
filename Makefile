TARGET = nier

CC = gcc

CFLAGS = -Wall -g

INCLUDES = -I/usr/local/include -I./curl/include

LIBS = -lraylib -lm -lpthread -ldl -lrt -lX11 -lcurl -lws2_32 -lwinssl -lcrypt32

SRCS = main.c gemini.c cJSON.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)