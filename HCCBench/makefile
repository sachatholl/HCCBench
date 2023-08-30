all: sender receiver receiver-2

sender: sender.c
	gcc -Wall -Wextra -o sender sender.c -lws2_32

receiver: receiver.c
	gcc -Wall -Wextra -o receiver receiver.c -lws2_32


receiver-2: receiver-2.c
	gcc -Wall -Wextra -o receiver-2 receiver-2.c -lws2_32

clean:
	rm -f sender receiver receiver-2
