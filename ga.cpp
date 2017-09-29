// ga.cpp : Prepis armasm <--> GNUarm - BN, 23.2.2015 

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>

#define	TAB (9) 
#define	LF (10)
#define	CR (13)
#define	BL (32)
#define	VEJA (44) 
#define	PIKA (46) 
#define	DVOP (58) 
#define	PODP (59) 
#define	AFNA (64) 
#define CTR (126) //tilt

int main(int argc, char* argv[])
{
	int		nch=0, i = 0, n=0, way=1, prvi=0;
	int		bKoment=0, bKomentSolo=0, bLabela=0, bKonec=0, bUkaz=0, lin=0, lin1=0;
	char	ch='a';
	char	file1[25]="armasm.s", file2[25]="gnuarm.s",
			lab0[200]="", lab1[200]="", lab2[200]="", lab3[200]="", koment[200];
	char	buffer[120];
//	char	*buf;
	FILE	*fin, *fout;

	printf("\n*** Pretvorba formata med ARMASM in GNUarm, BN, 23.2.2015 ***");
	if(argc <= 1) {
		printf("\n format ga.exe format file1 file2");
		printf("\n format=1: file1=armasm ---> file2=GNUarm");
		printf("\n format=2: file1=GNUarm ---> file2=armasm\n");
		return 1;
	}
	if(argc > 1) way = (int)atof(argv[1]);
	else         way = 1;
	if(way < 0)  way = 0;
	if(argc > 2) strcpy_s(file1, argv[2]);
	if(argc > 3) strcpy_s(file2, argv[3]);
	fopen_s(&fin, file1, "r");
	fopen_s(&fout, file2, "w");
//	fread(buf, 1, n, fin);
	printf("\nizbrano: (%d) ", way);
	
	switch(way) {
		case 1: 
			printf("ARMASM --> GNUarm \n"); break;
		case 2: 
			printf("GNUarm --> ARMASM \n"); break;
		default: ;
	}

	bKonec = 0; lin = 0; lin1 = 0;
	if(way == 1) { // armasm --> GNU as
		do {
			strcpy_s(lab1, "");
			strcpy_s(lab2, "");
			strcpy_s(lab3, "");
			strcpy_s(koment, "");
			bKoment = 0;
			bKomentSolo = 0;
			bLabela = 0;
			i = 1;
			do //labela1
			{
				ch = (char)(fgetc(fin));    
				if(ch == CTR) { bKonec = 1; break; }
				if((i == 1) && (ch != BL) && (ch != LF)) bLabela = 1; // labela!
				i++;
				if(ch == PODP) { ch = AFNA; bKoment = 1; bKomentSolo = 1; break; }
				if(ch == LF) lin++;

//				printf("\rvrsta: %d/%d (%2X)    ", lin, i, (int)ch); 
				printf("\r %5d vrst ", lin); 

				if((ch > BL) && (ch != VEJA))
					strncat_s(lab1, &ch, 1);
			}
			while((ch != TAB) && (ch != VEJA) && (ch != BL) && (ch != LF) && (ch != CTR));
			if(ch == CTR) break;

			if((ch != LF) && (ch != 0xFF) && (bKoment == 0)) {
				do //labela2
				{
					ch = (char)(fgetc(fin)); i++;
					if(ch == PODP) { ch = AFNA; bKoment = 1; break; }
					if(ch == LF) lin++;

//					printf("\rvrsta: %d/%d (%2X)    ", lin, i, (int)ch); 
					printf("\r %5d vrst ", lin); 

					if((ch > BL) && (ch != VEJA))
						strncat_s(lab2, &ch, 1);
				}
				while((ch != TAB) && (ch != VEJA) && (ch != BL) && (ch != LF) && (ch != CTR));
				if(ch == CTR) break;

				if((ch != LF) && (ch != 0xFF) && (bKoment == 0)) {
					do //labela3
					{
						ch = (char)(fgetc(fin)); i++;
						if(ch == PODP) { ch = AFNA; } 
						if(ch == LF) lin++;

//						printf("\rvrsta: %d/%d (%2X)    ", lin, i, (int)ch); 
						printf("\r %5d vrst ", lin); 

						if((ch >= BL) || (ch == TAB))
							strncat_s(lab3, &ch, 1);
					}
					while((ch != LF) && (ch != CTR)); //vse je lahko komentar
				}

			}
			if(bKoment == 1) {
				strncat_s(koment, &ch, 1);
				do 
				{
					ch = (char)(fgetc(fin)); i++;
					if(ch == PODP) { ch = AFNA; }
					if(ch == TAB) { ch = BL; }
					if(ch == LF) lin++;

//					printf("\rvrsta: %d/%d (%2X)    ", lin, i, (int)ch); 
					printf("\r %5d vrst ", lin); 

					if((ch >= BL))
						strncat_s(koment, &ch, 1);
				}
				while((ch != LF));
			}
			if(bKomentSolo == 0) {
				if((strncmp(strupr(lab2), "EQU", 3) == 0)) {
					fputc(PIKA, fout);
					fputs(lab2, fout);
					fputc(BL, fout);
					fputs(lab1, fout);
					fputc(VEJA, fout);
				}
				else {
					if((strncmp(strupr(lab1), "END", 3) == 0) || 
						(strncmp(strupr(lab1), "ORG", 3) == 0)) { 
						fputc(PIKA, fout);
						fputs(lab1, fout);
					}
					else {
						fputs(lab1, fout);
						if((bLabela == 1) && (strlen(lab1) > 0)) fputc(DVOP, fout); //labela
					}
//					fputc(BL, fout);
					if(strncmp(strupr(lab2), "END", 3) == 0) { fputc(PIKA, fout); fputs(lab2, fout); }
					else if(strncmp(strupr(lab2), "ORG", 3) == 0) { fputc(PIKA, fout); fputs(lab2, fout); }
					else if(strncmp(strupr(lab2), "LTORG", 5) == 0) { fputc(PIKA, fout); fputs(lab2, fout); }
					else if(strncmp(strupr(lab2), "SPACE", 5) == 0) { fputc(PIKA, fout); fputs(lab2, fout); }
					else if(strncmp(strupr(lab2), "GLOBAL", 6) == 0) { fputc(PIKA, fout); fputs(lab2, fout); }
					else if(strncmp(strupr(lab2), "CODE", 4) == 0) { 
						fputc(PIKA, fout); lab2[6] = lab2[5]; lab2[5] = lab2[4]; lab2[4] = 0x20; fputs(lab2, fout); 
					}
					else if(strncmp(strupr(lab2), "ENTRY", 5) == 0) { fputs("", fout); }
					else if((strncmp(strupr(lab2), "AREA", 4) == 0) || 
						(strncmp(strupr(lab3), "AREA", 4) == 0)) {
						fputc(PIKA, fout); fputs("text", fout); fputc(LF, fout); 
						fputc(PIKA, fout); fputs("arm", fout); 
					}
					else if(strncmp(strupr(lab2), "MACRO", 5) == 0) { fputc(PIKA, fout); fputs(lab2, fout); }
					else if(strncmp(strupr(lab2), "MEND", 4) == 0) { fputc(PIKA, fout); fputs("endm", fout); }
					else if(strncmp(strupr(lab2), "INCBIN", 6) == 0) { 
						fputc(PIKA, fout); fputs(lab2, fout); 
						buffer[0] = '"'; buffer[1] = 0; strcat(buffer, lab3); strcat(buffer, "\""); strcpy(lab3, buffer);
					}
					else if(strncmp(strupr(lab2), "INCLUDE", 7) == 0) { 
						fputc(PIKA, fout); fputs(lab2, fout); 
						buffer[0] = '"'; buffer[1] = 0; strcat(buffer, lab3); strcat(buffer, "\""); strcpy(lab3, buffer);
					}
					else if(strncmp(strupr(lab2), "ALIGN", 5) == 0) { fputc(PIKA, fout); fputc('B', fout); fputs(lab2, fout); }
					else if(strncmp(strupr(lab2), "DCB", 3) == 0) {
						if(bLabela == 1 && (strlen(lab1) > 0)) fputc(LF, fout); 
						fputc(TAB, fout); 
						if(lab3[0] == '"')	{ 
							if(lab3[strlen(lab3)-1] == '0') { 
								fputc(PIKA, fout); fputs("asciz", fout); 
								if(lab3[strlen(lab3)-2] == ',') {
									lab3[strlen(lab3)-2] = 0; // odreži ",0"
								}
								else if(lab3[strlen(lab3)-3] == ',') {
									lab3[strlen(lab3)-3] = 0; // odreži ", 0"
								}
							}
							else { fputc(PIKA, fout); fputs("ascii", fout); }
						}
						else				{ fputc(PIKA, fout); fputs("byte", fout); }
					}
					else if(strncmp(strupr(lab2), "DCD", 3) == 0) { 
						if(bLabela == 1 && (strlen(lab1) > 0)) fputc(LF, fout);
						fputc(TAB, fout); fputc(PIKA, fout); fputs("word", fout); 
					}
					else { fputc(TAB, fout); fputs(lab2, fout); }
				}
				fputc(BL, fout);
				if(strncmp(strupr(lab2), "AREA", 4) != 0) fputs(lab3, fout);
			}
			else {
				fputs(koment, fout);
			}
			fputc(LF, fout);
			if(strncmp(strupr(lab1), "END", 3) == 0) bKonec = 1;
			if(strncmp(strupr(lab2), "END", 3) == 0) bKonec = 1;
			lin1 += i; // skupaj znakov
		} while((bKonec == 0));
	}
	else if(way == 2) { // GNU as --> armasm
		do {
			strcpy_s(lab0, "");
			strcpy_s(lab1, "");
			strcpy_s(lab2, "");
			strcpy_s(lab3, "");
			strcpy_s(koment, "");
			bKoment = 0;
			bKomentSolo = 0;
			bLabela = 0;
			bUkaz = 0;
			i = 0;
			prvi = 0;
			do //labela1
			{
				if(prvi == 0) {
					do {
						ch = (char)(fgetc(fin)); i++;
						if(ch != LF) strncat_s(lab0, &ch, 1);
					} while ((ch == BL) || (ch == TAB)); // preskoèi belo (BL, TAB)!
				}
				if(prvi > 0) { ch = (char)(fgetc(fin)); i++; }
				
				if(ch == CTR) { bKonec = 1; break; }
				if((ch == PIKA) && (prvi == 0)) { bUkaz = 1;  bLabela = 0;} // GNUarm ukaz v labeli 1! --> ukaz ne more biti labela:
				if(ch == DVOP) { bLabela = 1; } // labela! in, ce ni ukaz
				if(ch == AFNA) { ch = PODP; bKoment = 1; bKomentSolo = 1; break; }
				if(ch == LF) lin++;

//				printf("\rvrsta: %d/%d (%2X)     ", lin, i, (int)ch); 
				printf("\r %5d vrst ", lin); 

				if((ch != BL) && (ch != TAB) && (ch != VEJA) && (ch != PIKA) && (ch != DVOP) && (ch != LF))
					strncat_s(lab1, &ch, 1);
				if((ch != LF) && (prvi > 0)) strncat_s(lab0, &ch, 1);
				prvi = 1;
			}
			while((ch != BL) && (ch != TAB) && (ch != VEJA) && (ch != DVOP) && (ch != LF) && (ch != CTR));
			if(ch == CTR) break;

			if((ch != LF) && (ch != 0xFF) && (bKoment == 0)) {
				prvi = 0;
				do //labela2
				{
					if(prvi == 0) {
						do {
							ch = (char)(fgetc(fin)); i++;
							if((ch != LF) && (ch != AFNA)) strncat_s(lab0, &ch, 1);
						} while ((ch == BL) || (ch == TAB)); // preskoèi belo (BL, TAB)!
					}
					if(prvi > 0) { ch = (char)(fgetc(fin)); i++; }
				
					if((ch == PIKA) && (prvi == 0)) bUkaz = 2; // GNUarm ukaz v labeli 2!
					if(ch == AFNA) { ch = PODP; bKoment = 1; break; }
					if(ch == LF) lin++;

//					printf("\rvrsta: %d/%d (%2X)     ", lin, i, (int)ch); 
					printf("\r %5d vrst ", lin); 

					if((ch != BL) && (ch != TAB) && (ch != VEJA) && (ch != PIKA) && (ch != DVOP) && (ch != LF))
						strncat_s(lab2, &ch, 1);
					if((ch == PIKA) && (prvi > 0)) // dodaj piko samo, èe ni prvi znak (filename)
						strncat_s(lab2, &ch, 1);
					if((ch != LF) && (prvi > 0)) strncat_s(lab0, &ch, 1);
					prvi = 1;
				}
				while((ch != BL) && (ch != TAB) && (ch != VEJA) && (ch != LF) && (ch != CTR));
				if(ch == CTR) break;

				if((ch != LF) && (ch != 0xFF) && (bKoment == 0)) {
					do //labela3
					{
						ch = (char)(fgetc(fin)); i++;
						if(ch == AFNA) { ch = PODP; } 

						if(ch == TAB) { ch = BL; } 

						if(ch == LF) lin++;

//						printf("\rvrsta: %d/%d (%2X)     ", lin, i, (int)ch); 
						printf("\r %5d vrst ", lin); 

//						if((ch >= BL) || (ch == TAB))
						if((ch >= BL))
							strncat_s(lab3, &ch, 1);
						if(ch != LF) strncat_s(lab0, &ch, 1);
					}
					while((ch != LF) && (ch != CTR)); //vse je lahko komentar
				}
			}
			if(bKoment == 1) {
				strncat_s(koment, &ch, 1);
				do 
				{
					ch = (char)(fgetc(fin)); i++;
					if(ch == AFNA) { ch = PODP; }
					if(ch == LF) lin++;

//					printf("\rvrsta: %d/%d (%2X)     ", lin, i, (int)ch); 
					printf("\r %5d vrst ", lin); 

					if((ch >= BL))
						strncat_s(koment, &ch, 1);
				}
				while((ch != LF));
			}
			if(bKomentSolo == 0) {

				if(bLabela == 1) { fputs(lab1, fout); fputc(BL, fout); // labela
				}

				if((bUkaz == 1) || (bUkaz == 2)) {

					if((strncmp(strupr(lab1), "EQU", 3) == 0)) {
						fputs(lab2, fout);
//						fputc(BL, fout);
						fputc(TAB, fout);
						fputs(&lab1[0], fout); // brez .
						fputc(BL, fout);
						fputs(lab3, fout);
					}
					else { // ce ni EQU
						if(	(strncmp(strupr(lab1), "END", 4) == 0) ) { // 4 za razliko od ENDIF
							fputc(TAB, fout); fputs(lab1, fout); 
							fputc(BL, fout); fputs(lab2, fout); bKonec = 1; // TAB brez .
						}
						else if(strncmp(strupr(lab1), "ORG", 3) == 0) { 
							fputc(TAB, fout); fputs(lab1, fout); 
							fputc(BL, fout); fputs(lab2, fout); fputc(BL, fout); fputs(lab3, fout); // TAB brez .
						}
						else if(strncmp(strupr(lab1), "LTORG", 5) == 0) { 
							fputc(TAB, fout); fputs(lab1, fout); fputc(BL, fout); fputs(lab2, fout); }
						else if(strncmp(strupr(lab1), "TEXT", 4) == 0) {
							fputs("\tAREA RESET, CODE, READONLY, ALIGN=9", fout); fputc(LF, fout); 
							fputs("\tENTRY", fout); fputc(LF, fout); 
						}
						else if(strncmp(strupr(lab1), "ENTRY", 5) == 0) { fputc(BL, fout); }
						else if(strncmp(strupr(lab1), "ARM", 3) == 0) { fputc(BL, fout); }
						else if(strncmp(strupr(lab1), "SPACE", 5) == 0) { 
							fputc(TAB, fout); fputs(lab1, fout); fputc(BL, fout); fputs(lab2, fout); }
						else if(strncmp(strupr(lab1), "GLOBAL", 6) == 0) { 
							fputc(TAB, fout); fputs(lab1, fout); fputc(BL, fout); fputs(lab2, fout); }
						else if(strncmp(strupr(lab1), "CODE", 4) == 0) { 
							lab1[4] = lab2[0]; lab1[5] = lab2[1]; lab1[6] = 0; fputc(TAB, fout); fputs(lab1, fout); 
							fputc(BL, fout); fputs(lab3, fout); // lab2 je združen v lab1
						}
						else if(strncmp(strupr(lab1), "MACRO", 5) == 0) { 
							fputc(TAB, fout); fputs(lab1, fout); 
							fputc(BL, fout); fputs(lab2, fout); 
							fputc(BL, fout); fputs(lab3, fout);  }
						else if(strncmp(strupr(lab1), "ENDM", 4) == 0) { 
							fputc(TAB, fout); fputs("MEND", fout); 
							fputc(BL, fout); fputs(lab2, fout); }
						else if(strncmp(strupr(lab1), "IF", 3) == 0) { 
							fputc(TAB, fout); fputs(lab1, fout); strncat(lab2, "=0", 2);
							fputc(BL, fout); fputs(lab2, fout);
						}
						else if(strncmp(strupr(lab1), "ELSE", 5) == 0) { 
							fputc(TAB, fout); fputs(lab1, fout); 
							fputc(BL, fout); fputs(lab2, fout); }
						else if(strncmp(strupr(lab1), "ELSEIF", 6) == 0) { 
							fputc(TAB, fout); fputs("ELIF", fout);  strncat(lab2, "=0", 2);
							fputc(BL, fout); fputs(lab2, fout); 
						}
						else if(strncmp(strupr(lab1), "ENDIF", 5) == 0) { 
							fputc(TAB, fout); fputs(lab1, fout); 
							fputc(BL, fout); fputs(lab2, fout); 
						}
						else if(strncmp(strupr(lab1), "ALIGN", 5) == 0) { 
							fputc(TAB, fout); fputs(lab1, fout); 
							fputc(BL, fout); fputs(lab2, fout); 
						}
						else if(strncmp(strupr(lab1), "BALIGN", 6) == 0) { 
							fputc(TAB, fout); fputs(&lab1[1], fout); 
							fputc(BL, fout); fputs(lab2, fout); 
						}
						else if(strncmp(strupr(lab1), "INCLUDE", 7) == 0) { 
							buffer[0] = 0; strcat(buffer, &lab2[1]); buffer[strlen(buffer)-1] = 0; strcpy(lab2, buffer);
							fputc(TAB, fout); fputs(lab1, fout); 
							fputc(BL, fout); fputs(lab2, fout); 
							fputc(BL, fout); fputs(lab3, fout);
						}
						else if(strncmp(strupr(lab1), "INCBIN", 6) == 0) { 
							buffer[0] = 0; strcat(buffer, &lab2[1]); buffer[strlen(buffer)-1] = 0; strcpy(lab2, buffer);
							fputc(TAB, fout); fputs(lab1, fout); 
							fputc(BL, fout); fputs(lab2, fout); 
							fputc(BL, fout); fputs(lab3, fout);
						}
						else if(strncmp(strupr(lab1), "BYTE", 4) == 0) { 
							fputc(TAB, fout); fputs("DCB", fout); 
							fputc(BL, fout); fputs(lab2, fout); 
							fputc(VEJA, fout); fputc(BL, fout); fputs(lab3, fout);
						}
						else if(strncmp(strupr(lab1), "ASCIZ", 5) == 0) {
							fputc(TAB, fout); fputs("DCB", fout); strncat_s(lab2, ",0", 2);
							fputc(BL, fout); fputs(lab2, fout); 
							fputc(BL, fout); fputs(lab3, fout);
						}
						else if	(strncmp(strupr(lab1), "ASCII", 5) == 0) {
							fputc(TAB, fout); fputs("DCB", fout); 
							fputc(BL, fout); fputs(lab2, fout);
							fputc(BL, fout); fputs(lab3, fout);
						}
						else if(strncmp(strupr(lab1), "WORD", 4) == 0) { 
							fputc(TAB, fout); fputs("DCD", fout); 
							fputc(BL, fout);  fputs(lab2, fout);
							fputc(BL, fout);  fputs(lab3, fout);
						}
						else if(strncmp(strupr(lab2), "END", 4) == 0) { // 4 za razliko od ENDIF
							fputc(TAB, fout); fputs(&lab2[1], fout); // TAB brez .
							fputc(BL, fout); fputs(lab2, fout); bKonec = 1; 
						}
						else if(strncmp(strupr(lab2), "INCBIN", 6) == 0) { // ***TODO!
							lab3[strlen(lab3)-1] = 0; //labela brez ..." //labela brez "...
							fputc(TAB, fout); fputs(lab2, fout); // fputs(lab3, fout);  
							fputc(BL, fout); lab3[strlen(lab3)-1] = 0; fputs(&lab3[1], fout); 
						}
						else if((strncmp(strupr(lab2), "BYTE", 4) == 0) || 
							(strncmp(strupr(lab2), "ASCII", 5) == 0)) {
							fputc(TAB, fout); fputs("DCB", fout);  fputs(lab3, fout);
						}
						else if(strncmp(strupr(lab2), "ASCIZ", 5) == 0) {
							fputc(TAB, fout); fputs("DCB", fout); 
							strncat_s(lab3, ",0", 2); fputc(BL, fout); fputs(lab3, fout);
						}
						else if(strncmp(strupr(lab2), "WORD", 4) == 0) { 
							fputc(TAB, fout); fputs("DCD", fout); fputs(lab3, fout);
						}
						else {
							if((bLabela == 0)) { fputc(BL, fout); }
						}
					} //-konec, ce ni EQU
				
				} // -konec, ce je ukaz 1 ali 2
				else if(bLabela == 1) { // ni nobenega ukaza in je labela 
					fputs(lab2, fout); fputc(BL, fout); 
					fputs(lab3, fout);
				}
				else if(bLabela == 0) { // ni nobenega ukaza in ni labele 
					fputs(lab0, fout); 
				}
				
			} // -konec KomentSolo=0
			
			else {
				fputs(koment, fout);
			}
			fputc(LF, fout);
			if(strncmp(strupr(lab1), "END", 4) == 0) bKonec = 1; // 4 za razliko od ENDIF
			if(strncmp(strupr(lab2), "END", 4) == 0) bKonec = 1; // varnost
			lin1 += i; // skupaj znakov
		} while((bKonec == 0));
	} // -konec way=2

	switch(way) {
		case 1: 
			printf("\r %5d vrst prepisano: /%s/ --> /%s/, (%d znakov)\n", lin, file1, file2, lin1); 
			break;
		case 2: 
			printf("\r %5d vrst prepisano: /%s/ --> /%s/, (%d znakov)\n", lin, file1, file2, lin1);
			break;
		default: 
			printf("\nNapaka: Format %d ni podprt!\n", way);
	}

	fclose(fin);
	fclose(fout);

	return 0;
}

