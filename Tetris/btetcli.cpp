#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <termios.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "colors.h"
#include "CTetris.h"

#define MAX_BLK_TYPES 7
#define MAXDW 50

using namespace std;

static struct termios oldterm, newterm;

char myname[32], peername[32];
pthread_mutex_t mutex_key;

/* Initialize new terminal i/o settings */

void initTermios(int echo); 
/* Restore old terminal i/o settings */
void resetTermios(void);
/* Read 1 character - echo defines echo mode */
char getch_(int echo);
/* Read 1 character without echo */
char getch(void);
/* Read 1 character with echo */
char getche(void);
int connect_server(char *ipaddr, char *portno);
int sendto_server(int sd, char *line);
void countdown(int sec);
void printScreen1(CTetris *p_board);
void run_in_solo_mode(CTetris *p_board);
void printScreen(CTetris *p_board, CTetris *q_board);
void printScreen1(CTetris *p_board);
char net_getch(int sd);
void run_in_dual_mode(int sd, CTetris *p_board, CTetris *q_board);
void run_in_send_mode(int sd, CTetris *p_board);
void run_in_recv_mode(int sd, CTetris *p_board);

int main(int argc, char *argv[]) {
	char line[128];
	int sd, flagsolo = 0, flagsend = 0, flagrecv = 0, flagdual = 0;
	int dy, dx;

	if (argc != 7) {
		cout << "usage: " << argv[0] << " ipaddr portnum myname peername dy dx" << endl;
		exit(1);
	}
	if (strstr(argv[0], "btetclisend") != NULL) {
		cout << "running mode: send" << endl;
		flagsend = 1;
	}
	if (strstr(argv[0], "btetclirecv") != NULL) {
		cout << "running mode: recv" << endl;
		flagrecv = 1;
	}
	if (strstr(argv[0], "btetclisolo") != NULL) {
		cout << "running mode: solo" << endl;
		flagsolo = 1;
	}
	if (strstr(argv[0], "btetcli") != NULL) {
		cout << "running mode: dual" << endl;
		flagdual = 1;
	}

	dy = atoi(argv[5]);
	dx = atoi(argv[6]);

	/*
	int T1D0[10] = { 0, 1, 0, 1, 1, 1, 0, 0, 0, -1 };
	int T1D1[10] = { 0, 1, 0, 0, 1, 1, 0, 1, 0, -1 };
	int T1D2[10] = { 1, 1, 1, 0, 1, 0, 0, 0, 0, -1 };
	int T1D3[10] = { 0, 1, 0, 1, 1, 0, 0, 1, 0, -1 };
	int T2D0[10] = { 0, 1, 0, 0, 1, 0, 1, 1, 0, -1 };
	int T2D1[10] = { 1, 0, 0, 1, 1, 1, 0, 0, 0, -1 };
	int T2D2[10] = { 0, 1, 1, 0, 1, 0, 0, 1, 0, -1 };
	int T2D3[10] = { 1, 1, 1, 0, 0, 1, 0, 0, 0, -1 };
	int T3D0[10] = { 0, 1, 0, 0, 1, 0, 0, 1, 1, -1 };
	int T3D1[10] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, -1 };
	int T3D2[10] = { 0, 1, 1, 0, 0, 1, 0, 0, 1, -1 };
	int T3D3[10] = { 0, 0, 0, 0, 0, 1, 1, 1, 1, -1 };
	int T4D0[10] = { 1, 1, 0, 0, 1, 1, 0, 0, 0, -1 };
	int T4D1[10] = { 0, 0, 1, 0, 1, 1, 0, 1, 0, -1 };
	int T4D2[10] = { 1, 1, 0, 0, 1, 1, 0, 0, 0, -1 };
	int T4D3[10] = { 0, 0, 1, 0, 1, 1, 0, 1, 0, -1 };
	int T5D0[10] = { 0, 1, 1, 1, 1, 0, 0, 0, 0, -1 };
	int T5D1[10] = { 0, 1, 0, 0, 1, 1, 0, 0, 1, -1 };
	int T5D2[10] = { 0, 1, 1, 1, 1, 0, 0, 0, 0, -1 };
	int T5D3[10] = { 0, 1, 0, 0, 1, 1, 0, 0, 1, -1 };
	int T6D0[17] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1 };
	int T6D1[17] = { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1 };
	int T6D2[17] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1 };
	int T6D3[17] = { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1 };
	int T7D0[5] = { 1, 1, 1, 1, -1 };
	int T7D1[5] = { 1, 1, 1, 1, -1 };
	int T7D2[5] = { 1, 1, 1, 1, -1 };
	int T7D3[5] = { 1, 1, 1, 1, -1 };
	*/

	int T1D0[10] = { 0, 10, 0, 10, 10, 10, 0, 0, 0, -1 };
	int T1D1[10] = { 0, 10, 0, 0, 10, 10, 0, 10, 0, -1 };
	int T1D2[10] = { 10, 10, 10, 0, 10, 0, 0, 0, 0, -1 };
	int T1D3[10] = { 0, 10, 0, 10, 10, 0, 0, 10, 0, -1 };
	int T2D0[10] = { 0, 20, 0, 0, 20, 0, 20, 20, 0, -1 };
	int T2D1[10] = { 20, 0, 0, 20, 20, 20, 0, 0, 0, -1 };
	int T2D2[10] = { 0, 20, 20, 0, 20, 0, 0, 20, 0, -1 };
	int T2D3[10] = { 20, 20, 20, 0, 0, 20, 0, 0, 0, -1 };
	int T3D0[10] = { 0, 30, 0, 0, 30, 0, 0, 30, 30, -1 };
	int T3D1[10] = { 30, 30, 30, 30, 0, 0, 0, 0, 0, -1 };
	int T3D2[10] = { 0, 30, 30, 0, 0, 30, 0, 0, 30, -1 };
	int T3D3[10] = { 0, 0, 0, 0, 0, 30, 30, 30, 30, -1 };
	int T4D0[10] = { 40, 40, 0, 0, 40, 40, 0, 0, 0, -1 };
	int T4D1[10] = { 0, 0, 40, 0, 40, 40, 0, 40, 0, -1 };
	int T4D2[10] = { 40, 40, 0, 0, 40, 40, 0, 0, 0, -1 };
	int T4D3[10] = { 0, 0, 40, 0, 40, 40, 0, 40, 0, -1 };
	int T5D0[10] = { 0, 50, 50, 50, 50, 0, 0, 0, 0, -1 };
	int T5D1[10] = { 0, 50, 0, 0, 50, 50, 0, 0, 50, -1 };
	int T5D2[10] = { 0, 50, 50, 50, 50, 0, 0, 0, 0, -1 };
	int T5D3[10] = { 0, 50, 0, 0, 50, 50, 0, 0, 50, -1 };
	int T6D0[17] = { 60, 60, 60, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1 };
	int T6D1[17] = { 0, 60, 0, 0, 0, 60, 0, 0, 0, 60, 0, 0, 0, 60, 0, 0, -1 };
	int T6D2[17] = { 60, 60, 60, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1 };
	int T6D3[17] = { 0, 60, 0, 0, 0, 60, 0, 0, 0, 60, 0, 0, 0, 60, 0, 0, -1 };
	int T7D0[5] = { 70, 70, 70, 70, -1 };
	int T7D1[5] = { 70, 70, 70, 70, -1 };
	int T7D2[5] = { 70, 70, 70, 70, -1 };
	int T7D3[5] = { 70, 70, 70, 70, -1 };

	int *setOfBlockArrays[MAX_BLK_TYPES * MAX_BLK_DEGREES] = {
	  T1D0, T1D1, T1D2, T1D3, T2D0, T2D1, T2D2, T2D3,
	  T3D0, T3D1, T3D2, T3D3, T4D0, T4D1, T4D2, T4D3,
	  T5D0, T5D1, T5D2, T5D3, T6D0, T6D1, T6D2, T6D3,
	  T7D0, T7D1, T7D2, T7D3
	};

	srand((unsigned int)time(NULL));
	char key;
	TetrisState state;
	CTetris::init(setOfBlockArrays, MAX_BLK_TYPES, MAX_BLK_DEGREES);

	CTetris *p_board = new CTetris(dy, dx);
	CTetris *q_board = new CTetris(dy, dx);

	OnCBlkLeft *myOnLeft = new OnCBlkLeft();
	OnCBlkRight *myOnRight = new OnCBlkRight();
	OnCBlkDown *myOnDown = new OnCBlkDown();
	OnCBlkUp *myOnUp = new OnCBlkUp();
	OnCBlkDrop *myOnDrop = new OnCBlkDrop();
	OnCBlkCW *myOnCW = new OnCBlkCW();
	OnCBlkCCW *myOnCCW = new OnCBlkCCW();
	OnNewCBlock *myOnNewBlock = new OnNewCBlock();
	OnCFinished *myOnFinished = new OnCFinished();

	//p_board->setOperation('a', Running, myOnLeft, Running, myOnRight, Running);
	//q_board->setOperation('a', Running, myOnLeft, Running, myOnRight, Running);

	//p_board->setOperation('d', Running, myOnRight, Running, myOnLeft, Running);
	//q_board->setOperation('d', Running, myOnRight, Running, myOnLeft, Running);

	//p_board->setOperation('s', Running, myOnDown, Running, myOnUp, NewBlock);
	//q_board->setOperation('s', Running, myOnDown, Running, myOnUp, NewBlock);

	//p_board->setOperation('w', Running, myOnCW, Running, myOnCCW, Running);
	//q_board->setOperation('w', Running, myOnCW, Running, myOnCCW, Running);

	p_board->setOperation('a', Running, myOnLeft, Running, myOnRight, NewBlock);
	q_board->setOperation('a', Running, myOnLeft, Running, myOnRight, NewBlock);

	p_board->setOperation('d', Running, myOnRight, Running, myOnLeft, NewBlock);
	q_board->setOperation('d', Running, myOnRight, Running, myOnLeft, NewBlock);

	p_board->setOperation('s', Running, myOnDown, Running, myOnUp, NewBlock);
	q_board->setOperation('s', Running, myOnDown, Running, myOnUp, NewBlock);

	p_board->setOperation('w', Running, myOnCW, Running, myOnCCW, Running);
	q_board->setOperation('w', Running, myOnCW, Running, myOnCCW, Running);

	p_board->setOperation(' ', Running, myOnDrop, Running, myOnUp, NewBlock);
	q_board->setOperation(' ', Running, myOnDrop, Running, myOnUp, NewBlock);

	p_board->setOperation('0', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);
	q_board->setOperation('0', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);

	p_board->setOperation('1', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);
	q_board->setOperation('1', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);

	p_board->setOperation('2', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);
	q_board->setOperation('2', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);

	p_board->setOperation('3', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);
	q_board->setOperation('3', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);

	p_board->setOperation('4', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);
	q_board->setOperation('4', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);

	p_board->setOperation('5', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);
	q_board->setOperation('5', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);

	p_board->setOperation('6', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);
	q_board->setOperation('6', NewBlock, myOnNewBlock, Running, myOnFinished, Finished);

	if (flagsolo == 0) {
		if ((sd = connect_server(argv[1], argv[2])) < 0)  exit(1);
		strcpy(myname, argv[3]);
		strcpy(peername, argv[4]);
		sprintf(line, "/nick %s\n", myname);  sendto_server(sd, line);
		countdown(10);
	}

	if (flagsolo == 1)
		run_in_solo_mode(p_board);
	if (flagsend == 1)
		run_in_send_mode(sd, p_board);
	if (flagrecv == 1)
		run_in_recv_mode(sd, p_board);
	if (flagdual == 1)
		run_in_dual_mode(sd, p_board, q_board);

	sleep(1);
	close(sd);

	delete myOnLeft;
	delete myOnRight;
	delete myOnDown;
	delete myOnUp;
	delete myOnCW;
	delete myOnCCW;
	delete myOnNewBlock;
	delete myOnFinished;

	cout << "Program terminated!" << endl;

	return 0;
}

void initTermios(int echo) {
	tcgetattr(0, &oldterm); /* grab old terminal i/o settings */
	newterm = oldterm; /* make new settings same as old settings */
	newterm.c_lflag &= ~ICANON; /* disable buffered i/o */
	newterm.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
	tcsetattr(0, TCSANOW, &newterm); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) {
	tcsetattr(0, TCSANOW, &oldterm);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) {
	char ch;
	initTermios(echo);
	ch = getchar();
	resetTermios();
	return ch;
}

/* Read 1 character without echo */
char getch(void) { return getch_(0); }

/* Read 1 character with echo */
char getche(void) { return getch_(1); }

int connect_server(char *ipaddr, char *portno)
{
	int sd = -1;
	struct sockaddr_in sin;

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cerr << "socket error: " << strerror(errno) << endl;
		return -1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ipaddr);
	sin.sin_port = htons((short)atoi(portno));

	if (connect(sd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		cerr << "connect error: " << strerror(errno) << endl;
		return -1;
	}
	return sd;
}

int sendto_server(int sd, char *line)
{
	int len = strlen(line);
	if (write(sd, line, len) != len) {
		cerr << "write error: " << strerror(errno) << endl;
		return -1;
	}
	return 0;
}

void countdown(int sec)
{
	int i;
	for (i = 0; i < sec; i++) {
		cout << "Countdown: " << (sec - i) << " seconds..." << endl;
		sleep(1);
	}
}

void run_in_solo_mode(CTetris *p_board)
{
	char key;
	TetrisState state;

	key = (char)('0' + rand() % MAX_BLK_TYPES);

	while (key != 'q') {
		state = p_board->accept(key);
		printScreen1(p_board);
		cout << endl;
		if (state == NewBlock) {
			key = (char)('0' + rand() % MAX_BLK_TYPES);
			state = p_board->accept(key);
			printScreen1(p_board);
			cout << endl;
			if (state == Finished)
				break;
		}
		key = getche();
		cout << endl;
	}
}

void printScreen(CTetris *p_board, CTetris *q_board)
{
	int dy = p_board->oScreen->get_dy();
	int dx = p_board->oScreen->get_dx();
	int dw = p_board->iScreenDw;
	int **p_array = p_board->oScreen->get_array();
	int **q_array = q_board->oScreen->get_array();
	cout << endl;
	for (int y = 0; y < dy - dw + 1; y++) {
		for (int x = dw - 1; x < dx - dw + 1; x++) {
			if (p_array[y][x] == -1)
				cout << "■ ";
			else if (p_array[y][x] == 0)
				cout << "□ ";
			else if (p_array[y][x] == 10)
				cout << color_black << "☆ " << color_normal;
			else if (p_array[y][x] == 20)
				cout << color_green << "★ " << color_normal;
			else if (p_array[y][x] == 30)
				cout << color_cyan << "● " << color_normal;
			else if (p_array[y][x] == 40)
				cout << color_red << "◆ " << color_normal;
			else if (p_array[y][x] == 50)
				cout << color_yellow << "▲ " << color_normal;
			else if (p_array[y][x] == 60)
				cout << color_magenta << "♣ " << color_normal;
			else if (p_array[y][x] == 70)
				cout << color_white << "♥ " << color_normal;
			else
				cout << "X ";
		}
		cout << " ";
		for (int x = dw - 1; x < dx - dw + 1; x++) {
			if (q_array[y][x] == -1)
				cout << "□ ";
			else if (q_array[y][x] == 0)
				cout << "□ ";
			else if (q_array[y][x] == 10)
				cout << color_black << "☆ " << color_normal;
			else if (q_array[y][x] == 20)
				cout << color_green << "★ " << color_normal;
			else if (q_array[y][x] == 30)
				cout << color_cyan << "● " << color_normal;
			else if (q_array[y][x] == 40)
				cout << color_red << "◆ " << color_normal;
			else if (q_array[y][x] == 50)
				cout << color_yellow << "▲ " << color_normal;
			else if (q_array[y][x] == 60)
				cout << color_magenta << "♣ " << color_normal;
			else if (q_array[y][x] == 70)
				cout << color_white << "♥ " << color_normal;
			else
				cout << "X ";
		}
		cout << endl;
	}
}

void printScreen1(CTetris *p_board)
{
	int dy = p_board->oScreen->get_dy();
	int dx = p_board->oScreen->get_dx();
	int dw = p_board->iScreenDw;
	int **p_array = p_board->oScreen->get_array();

	for (int y = 0; y < dy - dw + 1; y++) {
		for (int x = dw - 1; x < dx - dw + 1; x++) {
			if (p_array[y][x] == 0)
				cout << "□ ";
			else if (p_array[y][x] == 10)
				cout << color_black << "☆ " << color_normal;
			else if (p_array[y][x] == 20)
				cout << color_green << "★ " << color_normal;
			else if (p_array[y][x] == 30)
				cout << color_cyan << "● " << color_normal;
			else if (p_array[y][x] == 40)
				cout << color_red << "◆ " << color_normal;
			else if (p_array[y][x] == 50)
				cout << color_yellow << "▲ " << color_normal;
			else if (p_array[y][x] == 60)
				cout << color_magenta << "♣ " << color_normal;
			else if (p_array[y][x] == 70)
				cout << color_white << "♥ " << color_normal;
			else
				cout << "X ";
		}
		cout << endl;
	}
}

char net_getch(int sd)
{
	char key, name[128], *pos;
	static char message[1024], *ppos = message;
	static int n = 0;

	if (ppos == message) { // read from the socket
		memset(message, 0, sizeof(message));
		if ((n = read(sd, message, sizeof(message))) < 0) {
			cerr << "read error: " << strerror(errno) << endl;
			return -1;
		}
	}
	//cout << "ppos: " << ppos << endl;
	if ((pos = strstr(ppos, "\n")) != NULL) {
		char *cpos = strstr(ppos, ":");
		*pos = 0;
		memcpy(name, ppos, cpos - ppos);
		name[cpos - ppos] = 0;
		key = cpos[2];
		if (strcmp(name, peername) != NULL) {
			cout << "peername mismatch!!" << endl;
			cout << "message: " << message << endl;
			exit(1);
		}
		//cout << "name: " << name << endl;
		//cout << "key: " << key << endl;
		n -= (pos + 1 - ppos);
		ppos = pos + 1;
		if (n == 0) // reached the end of the content
			ppos = message;
	}
	return key;
}

void run_in_dual_mode(int sd, CTetris *p_board, CTetris *q_board)
{
	char key, myCurrBlk, myNextBlk;
	char peerKey, peerCurrBlk, peerNextBlk;
	TetrisState myState, peerState;
	char line[128];

	int sd_state;
	struct timeval tv;
	fd_set readfds;

	myCurrBlk = (char)('0' + rand() % MAX_BLK_TYPES);
	sprintf(line, "/msg %s %c\n", peername, myCurrBlk); sendto_server(sd, line);
	myNextBlk = (char)('0' + rand() % MAX_BLK_TYPES);
	sprintf(line, "/msg %s %c\n", peername, myNextBlk); sendto_server(sd, line);
	myState = p_board->accept(myCurrBlk);
	peerCurrBlk = net_getch(sd);
	peerNextBlk = net_getch(sd);
	peerState = q_board->accept(peerCurrBlk); cout << endl;
	printScreen(p_board, q_board); cout << endl;
	do {
		FD_ZERO(&readfds);
		FD_SET(0, &readfds);
		FD_SET(sd, &readfds);
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		sd_state = select(sd + 1, &readfds, (fd_set *)0, (fd_set *)0, &tv);
		switch (sd_state) {
		case -1:
			perror("select error : ");
			exit(0);
			break;
		case 0: break;
		default:
			cout << "hello" << endl;
			if (FD_ISSET(0, &readfds) && (key = getche())) {
				sprintf(line, "/msg %s %c\n", peername, key);
				sendto_server(sd, line);
				myState = p_board->accept(key);
				if (myState == NewBlock) {
					myCurrBlk = myNextBlk;
					myNextBlk = (char)('0' + rand() % MAX_BLK_TYPES);
					sprintf(line, "/msg %s %c\n", peername, myNextBlk);
					sendto_server(sd, line);
					myState = p_board->accept(myCurrBlk);
					if (myState == Finished) {
						cout << "You lose." << endl;
						key = 'q';
					}
				}
			}
			if (FD_ISSET(sd, &readfds)) {
				peerKey = net_getch(sd);
				peerState = q_board->accept(peerKey);
				if (peerState == NewBlock) {
					peerCurrBlk = peerNextBlk;
					peerNextBlk = net_getch(sd);
					peerState = q_board->accept(peerCurrBlk);
					if (peerState == Finished) {
						cout << "You win." << endl;
						key = 'q';
					}
				}
			}
			printScreen(p_board, q_board); cout << endl;
		}
	} while (key != 'q');
	sprintf(line, "/msg %s %c\n", peername, 'q'); sendto_server(sd, line);
	sprintf(line, "/quit\n");  sendto_server(sd, line);
}

void run_in_send_mode(int sd, CTetris *p_board)
{
	char key;
	TetrisState state;
	char line[128];

	key = (char)('0' + rand() % MAX_BLK_TYPES); cout << key << endl;

	while (key != 'q') {
		state = p_board->accept(key);
		printScreen1(p_board);	cout << endl;
		sprintf(line, "/msg %s %c\n", peername, key); sendto_server(sd, line);

		if (state == NewBlock) {
			key = (char)('0' + rand() % MAX_BLK_TYPES); cout << key << endl;
			state = p_board->accept(key);
			printScreen1(p_board);	cout << endl;
			sprintf(line, "/msg %s %c\n", peername, key); sendto_server(sd, line);

			if (state == Finished)
				break;
		}
		//cin >> key;
		key = getche();
	}

	sprintf(line, "/msg %s %c\n", peername, 'q'); sendto_server(sd, line);
	sprintf(line, "/quit\n");  sendto_server(sd, line);
}

void run_in_recv_mode(int sd, CTetris *p_board)
{
	char key;
	TetrisState state;
	char line[128];

	key = net_getch(sd); cout << key << endl;

	while (key != 'q') {
		state = p_board->accept(key);
		printScreen1(p_board); cout << endl;

		if (state == NewBlock) {
			key = net_getch(sd); cout << key << endl;
			state = p_board->accept(key);
			printScreen1(p_board);
			cout << endl;

			if (state == Finished) break;
		}

		key = net_getch(sd); cout << key << endl;

	}

	sprintf(line, "/msg %s %c\n", peername, 'q'); sendto_server(sd, line);
	sprintf(line, "/quit\n"); sendto_server(sd, line);

}
