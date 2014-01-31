/*
 * Copyright Andrew Smith 2001-2014 sem@k1k2.com
 *
 * My name and email address must stay here at the top
 * and you must email me enhancements
 *
 * The follow is in addition to the above.
 * If any terms below conflict with the above, then the terms above take
 * priority over the terms below that conflict
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <getopt.h>
#include <malloc.h>
#include <stdlib.h>

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
};

void usAge(char *prog)
{
	char *slash = strrchr(prog, '/');
	if (slash)
		slash++;
	else
		slash = prog;

	fprintf(stderr, "usAge: %s [-v] [-w] filename cmd [args...]\n", slash);
	fprintf(stderr, " where -v = print the process return status\n");
	fprintf(stderr, " where -w = wait to get it - retry every 1 second\n");
	fprintf(stderr, " where filename = the semaphore file\n");
	fprintf(stderr, " where cmd = the cmd to execute\n");

	exit(1);
}

int main(int argc, char *argv[])
{
	key_t	*key;
	int	*sem;
	struct	semid_ds seminfo;
	union	semun opt;
	int	ret;
	pid_t	cpid;
	pid_t	wpid;
	int	verbose = 0;
	int	dowait = 0;
	int	c;

	for (;;)
	{
		c = getopt(argc, argv, "+vw");
		if (c == -1)
			break;
		switch (c)
		{
		case 'v':
			verbose = !0;
			break;
		case 'w':
			dowait = !0;
			break;
		default:
			usAge(argv[0]);
			break;
		}
	}

	if ((argc - optind) < 2)
		usAge(argv[0]);

	key = malloc(sizeof(*key));
	if (!key)
	{
		fputs("OOM key\n", stderr);
		exit(1);
	}

	sem = malloc(sizeof(*sem));
	if (!sem)
	{
		fputs("OOM sem\n", stderr);
		exit(1);
	}

	*key = ftok(argv[optind], 'P');

	*sem = semget(*key, 1, IPC_CREAT | IPC_EXCL | 438);

	if (*sem < 0)
	{
		if (errno != EEXIST)
		{
			fprintf(stderr, "Failed to get SEM (%d) %s\n", errno, strerror(errno));
			exit(1);
		}

		*sem = semget(*key, 1, 0);
		if (*sem < 0)
		{
			fprintf(stderr, "Failed to access SEM (%d) %s\n", errno, strerror(errno));
			exit(1);
		}
	}

	opt.buf = &seminfo;
	for (;;)
	{
		if (semctl(*sem, 0, IPC_STAT, opt) == -1)
		{
			if (!dowait)
				exit(0);

			sleep(1);
		}
		else
			break;
	}

	struct sembuf sops[] = {
		{ 0, 0, IPC_NOWAIT | SEM_UNDO },
		{ 0, 1, IPC_NOWAIT | SEM_UNDO }
	};

	while (semop(*sem, sops, 2))
	{
		if (errno == EAGAIN)
		{
			if (!dowait)
				exit(0);
		}
		else
		{
			fprintf(stderr, "Failed to get SEM (%d) %s\n", errno, strerror(errno));
			exit(1);
		}

		sleep(1);
	}

	cpid = fork();
	if (cpid == 0)
	{
		execvp(argv[1+optind], &argv[1+optind]);
		perror(argv[1+optind]);
		exit(1);
	}

	wpid = wait(cpid, NULL, 0);

	if (verbose)
		fprintf(stderr, "Child process (%d) exited (%d)\n", cpid, wpid);

	exit(0);
}
