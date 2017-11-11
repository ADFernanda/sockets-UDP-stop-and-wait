all: servidorFTP clienteFTP

clean: 
	@rm -f *.o 

servidorFTP: servidorFTP.c 
	gcc $(CFLAGS) -o $@ servidorFTP.c -lm tp_socket.c

clienteFTP: clienteFTP.c 
	gcc $(CFLAGS) -o $@ clienteFTP.c -lm tp_socket.c

tp_socket.o: tp_socket.c tp_socket.h
