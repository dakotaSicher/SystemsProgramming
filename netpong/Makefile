#Netpong Makefile

clean:
	rm -f netpong


netpong: netpong.c socklib.c pong_lib.c sppbtp.c
	gcc -o netpong netpong.c pong_lib.c socklib.c sppbtp.c -lcurses
