main: main.c common.h
	gcc -g -Wall -o main main.c

client: client.c common.h
	gcc -g -Wall -o client client.c

attacker: attacker.c common.h
	gcc -g -Wall -lm -o attacker attacker.c

clean:
	rm -f main client attacker