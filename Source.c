#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

enum pacman_mozgas { all, jobb, bal, le, fel }allapot;

int x = 9, y = 12, x_p = 9, y_p = 9, x_k = 9, y_k = 10;//pacman pozicioja,szellemek pozicioja
int palya[100][100];

typedef struct {
	int pontok;
	char nev[50];
}jatekos;

void cursor_elrejtes()
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = FALSE;
	cursor.dwSize = sizeof(cursor);
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(handle, &cursor);
}

void cursor(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void palya_beolvasas() {//max 100x100-as pályát lehet megadni
	int sor, oszlop, i, j;
	FILE* fp;
	fp = fopen("palya.txt", "r");
	fscanf(fp, "%d", &sor);
	fscanf(fp, "%d", &oszlop);
	for (i = 0; i < sor; ++i) {
		for (j = 0; j < oszlop; ++j) {
			fscanf(fp, "%d", &palya[i][j]);
		}
	}
	fclose(fp);
	for (i = 0; i < sor; ++i) {
		for (j = 0; j < oszlop; ++j) {
			if (palya[i][j] == 2) {//fal kirajzolás
				printf("°");
			}
			else if (palya[i][j] == 1) {//pontok kirajzolása az útra
				printf(".");
			}
			else if (palya[i][j] == 0 || palya[i][j] == 5) {// üres út
				printf(" ");
			}
			else if (palya[i][j] == 4) {// gyümölcs kirajzolása
				printf("O");
			}
		}
		printf("\n");
	}
	cursor(x, y);
	printf("\033[0;33m");
	printf("C");
	printf("\033[0m");
	cursor(x_p, y_p);
	printf("\033[0;31m");
	printf("&");
	printf("\033[0m");
	cursor(x_k, y_k);
	printf("@");
	cursor(x, y);
}

void bill_bemenet() {
	char a = 0;
	if (_kbhit()) {
		a = _getch();
		switch (a) {
		case 'w':
			allapot = fel;
			break;
		case 's':
			allapot = le;
			break;
		case 'd':
			allapot = jobb;
			break;
		case 'a':
			allapot = bal;
			break;
		default:;
		}
	}
}

void pontok_mentese(int pontok) {
	system("cls");
	char* nev = (char*)malloc(sizeof(char) * 50);
	printf("Add meg a neved!\n");
	fgets(nev, 50, stdin);
	FILE* fp;
	fp = fopen("eredmenyek.txt", "a");
	fprintf(fp, "%d  %s", pontok, nev);
	fclose(fp);
	free(nev);
	fclose(fp);
}

void eredmenyek_rendezese() {
	jatekos eredmeny[50];
	bool csere = true;
	int k, db, j;
	jatekos tarolo;
	FILE* fp;
	fp = fopen("eredmenyek.txt", "r");
	for (db = 0; !feof(fp); ++db) {
		fscanf(fp, "%d %s\n", &eredmeny[db].pontok, eredmeny[db].nev);
	}
	for (k = db; k > 0 && csere; --k) {
		csere = false;
		for (j = 0; j < k; j++) {
			if (eredmeny[j].pontok < eredmeny[j + 1].pontok) {
				tarolo = eredmeny[j];
				eredmeny[j] = eredmeny[j + 1];
				eredmeny[j + 1] = tarolo;
				csere = true;
			}
		}
	}
	fclose(fp);
	fp = fopen("eredmenyek.txt", "w");
	for (int l = 0; l < db; ++l) {
		fprintf(fp, "%d %s\n", eredmeny[l].pontok, eredmeny[l].nev);
	}
	fclose(fp);
	printf("RANGLETRA:\n");
	for (int g = 0; g < db; g++) {
		printf("%d %s\n", eredmeny[g].pontok, eredmeny[g].nev);
	}
}


int legrovidebb_ut(double r_fel, double r_le, double r_jobb, double r_bal) {
	int legrovidebb = 0;//szellemek prioritása egyenlõ utak esetén:fel,bal,le,jobb
	if (r_fel <= r_le && r_fel <= r_jobb && r_fel <= r_bal && r_fel != 1000) {
		legrovidebb = 1;//felfele
	}
	else if (r_le < r_fel && r_le <= r_jobb && r_le < r_bal && r_le != 1000) {
		legrovidebb = 2;//lefele
	}
	else if (r_jobb < r_fel && r_jobb < r_le && r_jobb < r_bal && r_jobb != 1000) {
		legrovidebb = 3;//jobbra
	}
	else if (r_bal <= r_jobb && r_bal < r_fel && r_bal <= r_le && r_bal != 1000) {
		legrovidebb = 4;//balra
	}
	return legrovidebb;
}

int szellem_irany(int elozo, int x1, int y1) {
	double r_fel = 0, r_le = 0, r_jobb = 0, r_bal = 0;
	char legrovidebb;
	//pacman helyzetére mutató vektor hosszanak kiszamolasa
	if (palya[y1 - 1][x1] != 2 && elozo != 'l') {//fel
		r_fel = (x - x1) * (x - x1) + (y - (y1 - 1)) * (y - (y1 - 1));
	}
	else
		r_fel = 1000;
	if (palya[y1 + 1][x1] != 2 && elozo != 'f') {//le
		r_le = (x - x1) * (x - x1) + (y - (y1 + 1)) * (y - (y1 + 1));
	}
	else
		r_le = 1000;
	if (palya[y1][x1 + 1] != 2 && elozo != 'b') {//jobb
		r_jobb = (x - (x1 + 1)) * (x - (x1 + 1)) + (y - y1) * (y - y1);
	}
	else
		r_jobb = 1000;
	if (palya[y1][x1 - 1] != 2 && elozo != 'j') {//bal
		r_bal = (x - (x1 - 1)) * (x - (x1 - 1)) + (y - y1) * (y - y1);
	}
	else
		r_bal = 1000;

	return legrovidebb = legrovidebb_ut(r_fel, r_le, r_jobb, r_bal);
}

void pac_mozgas() {//pacman mozgása a pályán
	if (allapot == jobb) {
		if (palya[y][x + 1] != 2) {
			cursor(x, y);
			printf(" ");
			printf("\033[0;33m");
			printf("C");
			printf("\033[0m");
			x += 1;
			cursor(x, y);
			if (y == 10 && x == 18) {//alagút a túloldalra
				Sleep(120);
				printf(" ");
				x = 0;
				cursor(x, y);
				printf("\033[0;33m");
				printf("C");
				printf("\033[0m");
				cursor(x, y);
			}
			Sleep(120);
		}
	}
	else if (allapot == bal) {
		if (palya[y][x - 1] != 2) {
			cursor(x, y);
			printf(" ");
			cursor(x - 1, y);
			printf("\033[0;33m");
			printf("C");
			printf("\033[0m");
			x -= 1;
			cursor(x, y);
			if (y == 10 && x == 0) {//alagút a túloldalra
				x = 18;
				printf(" ");
				cursor(x, y);
				printf("\033[0;33m");
				printf("C");
				printf("\033[0m");
				cursor(x, y);
			}

			Sleep(120);
		}
	}
	else if (allapot == fel) {
		if (palya[y - 1][x] != 2) {
			cursor(x, y);
			printf(" ");
			cursor(x, y - 1);
			printf("\033[0;33m");
			printf("C");
			printf("\033[0m");
			y -= 1;
			cursor(x, y);
			Sleep(120);
		}
	}
	else if (allapot == le) {
		if (palya[y + 1][x] != 2) {
			cursor(x, y);
			printf(" ");
			cursor(x, y + 1);
			printf("\033[0;33m");
			printf("C");
			printf("\033[0m");
			y += 1;
			cursor(x, y);
			Sleep(120);
		}
	}
}

char piros_szellem_mozgas(char elozo) {//1:fel 2:le 3:jobb 4:bal
	int mozgas_irany;
	if (x_p == 9 && y_p == 9) {
		cursor(x_p, y_p);
		printf(" ");
		cursor(x_p, y_p - 1);
		printf("\033[0;31m");
		printf("&");
		printf("\033[0m");
		y_p -= 1;

	}
	mozgas_irany = szellem_irany(elozo, x_p, y_p);
	if (mozgas_irany == 1) {
		cursor(x_p, y_p);
		if (palya[y_p][x_p] == 1)
			printf(".");
		else if (palya[y_p][x_p] == 4)
			printf("O");
		else
			printf(" ");
		cursor(x_p, y_p - 1);
		printf("\033[0;31m");
		printf("&");
		printf("\033[0m");
		y_p -= 1;
		elozo = 'f';
	}
	else if (mozgas_irany == 2) {
		cursor(x_p, y_p);
		if (palya[y_p][x_p] == 1)
			printf(".");
		else if (palya[y_p][x_p] == 4)
			printf("O");
		else
			printf(" ");
		cursor(x_p, y_p + 1);
		printf("\033[0;31m");
		printf("&");
		printf("\033[0m");
		y_p += 1;
		elozo = 'l';
	}
	else if (mozgas_irany == 3) {
		cursor(x_p, y_p);
		if (palya[y_p][x_p] == 1)
			printf(".");
		else if (palya[y_p][x_p] == 4)
			printf("O");
		else
			printf(" ");
		cursor(x_p + 1, y_p);
		printf("\033[0;31m");
		printf("&");
		printf("\033[0m");
		x_p += 1;
		elozo = 'j';
		if (y_p == 10 && x_p == 18) {//alagút a túloldalra
			Sleep(120);
			printf(" ");
			x_p = 0;
			cursor(x_p, y_p);
			printf("\033[0;31m");
			printf("&");
			printf("\033[0m");
		}
	}
	else if (mozgas_irany == 4) {
		cursor(x_p, y_p);
		if (palya[y_p][x_p] == 1)
			printf(".");
		else if (palya[y_p][x_p] == 4)
			printf("O");
		else
			printf(" ");
		cursor(x_p - 1, y_p);
		printf("\033[0;31m");
		printf("&");
		printf("\033[0m");
		x_p -= 1;
		elozo = 'b';
		if (y_p == 10 && x_p == 0) {//alagút a túloldalra
			x_p = 18;
			printf(" ");
			cursor(x_p, y_p);
			printf("\033[0;31m");
			printf("&");
			printf("\033[0m");
			cursor(x_p, y_p);
		}
	}
	return elozo;
}

char kek_szellem_mozgas(char elozo_k) {//1:fel 2:le 3:jobb 4:bal
	if (x_k == 9 && y_k == 10) {
		cursor(x_k, y_k);
		printf(" ");
		cursor(x_k, y_k - 1);
		printf("\033[0;36m");
		printf("@");
		printf("\033[0m");
		y_k -= 1;
	}
	int mozgas_irany = szellem_irany(elozo_k, x_k, y_k);
	if (mozgas_irany == 1) {
		cursor(x_k, y_k);
		if (palya[y_k][x_k] == 1)
			printf(".");
		else if (palya[y_k][x_k] == 4)
			printf("O");
		else
			printf(" ");
		cursor(x_k, y_k - 1);
		printf("\033[0;36m");
		printf("@");
		printf("\033[0m");
		y_k -= 1;
		elozo_k = 'f';
	}
	else if (mozgas_irany == 2) {
		cursor(x_k, y_k);
		if (palya[y_k][x_k] == 1)
			printf(".");
		else if (palya[y_k][x_k] == 4)
			printf("O");
		else
			printf(" ");
		cursor(x_k, y_k + 1);
		printf("\033[0;36m");
		printf("@");
		printf("\033[0m");
		y_k += 1;
		elozo_k = 'l';
	}
	else if (mozgas_irany == 3) {
		cursor(x_k, y_k);
		if (palya[y_k][x_k] == 1)
			printf(".");
		else if (palya[y_k][x_k] == 4)
			printf("O");
		else
			printf(" ");
		cursor(x_k + 1, y_k);
		printf("\033[0;36m");
		printf("@");
		printf("\033[0m");
		x_k += 1;
		elozo_k = 'j';
		if (y_k == 10 && x_k == 18) {//alagút a túloldalra
			Sleep(120);
			cursor(x_k, y_k);
			printf(" ");
			x_k = 0;
			cursor(x_k, y_k);
			printf("\033[0;36m");
			printf("@");
			printf("\033[0m");
		}
	}
	else if (mozgas_irany == 4) {
		cursor(x_k, y_k);
		if (palya[y_k][x_k] == 1)
			printf(".");
		else if (palya[y_k][x_k] == 4)
			printf("O");
		else
			printf(" ");
		cursor(x_k - 1, y_k);
		printf("\033[0;36m");
		printf("@");
		printf("\033[0m");
		x_k -= 1;
		elozo_k = 'b';
		if (y_k == 10 && x_k == 0) {//alagút a túloldalra
			x_k = 18;
			cursor(0, y_k);
			printf(" ");
			cursor(x_k, y_k);
			printf("\033[0;36m");
			printf("@");
			printf("\033[0m");
		}
	}

	return elozo_k;
}

int main() {
	bool gameover = false;
	int pontok = 0;
	char elozo = 'f', elozo_k = 'f';
	int k = 0;
	int sebezhetetlen = 0;
	allapot = all;
	cursor_elrejtes();
	palya_beolvasas();

	while (gameover == false) {//billentyûzet bemenet alapján mozgás eldöntése
		bill_bemenet();
		pac_mozgas();
		if (palya[y][x] == 1) {//pont számolás
			pontok += 10;
			palya[y][x] = 0;
		}
		//szellemek mozgása
		elozo = piros_szellem_mozgas(elozo);
		Sleep(120);
		elozo_k = kek_szellem_mozgas(elozo_k);
		Sleep(120);

		cursor(20, 0);
		printf("Pontjaid:%d", pontok);
		cursor(x, y);

		if (palya[y][x] == 4) {//sebezhetetlenség
			sebezhetetlen = 1;
			pontok += 100;
			palya[y][x] = 0;
		}
		if (sebezhetetlen == 0) {
			cursor(20, 1);
			printf("                   ");
			cursor(20, 1);
			printf("sebezheto vagy");
		}
		else {
			cursor(20, 1);
			printf("                   ");
			cursor(20, 1);
			printf("sebezhetetlen vagy");
		}
		if (sebezhetetlen == 0) {
			if (x == x_p && y == y_p || x == x_k && y == y_k) {
				gameover = true;
			}
		}
		else
			k += 1;

		if (k == 20) {//20 lépésig sebezhetetlen
			sebezhetetlen = 0;
			k = 0;
		}

	}

	if (gameover == true) {
		pontok_mentese(pontok);
		eredmenyek_rendezese();
	}
	return 0;
}
