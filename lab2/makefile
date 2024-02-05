main: main.c common.h
	gcc -g -Wall -o main main.c

client: client.c common.h
	gcc -g -Wall -o client client.c

attacker: attacker.c common.h
	gcc -g -Wall -o attacker attacker.c -lm

clean:
	rm -f main client attacker