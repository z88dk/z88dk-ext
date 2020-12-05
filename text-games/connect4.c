//  Connect 4 game
//  This program is by Ale.gatti96
//  Original program:  http://www.pierotofy.it/pages/sorgenti/dettagli/19253-Forza_4/


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>

//Dimensioni campo da gioco
#define R 9					//Numero di righe
#define C 8					//Numero di colonne
#define RC R*C

//Caratteri stampati a video
#define g1 		 'X'		//Carattere del giocatore 1
#define g2 		 'O'		//Carattere del giocatore 2
#define vuoto  '.'		//Carattere per uno spazio vuoto
#define CVitt  'V' 		//Carattere per segnare la combinazione vincente

//Valori nella matrice
#define vuotoI 0		//Valore di uno spazio vuoto
#define g1I 1				//Valore del giocatore 1
#define g2I 2				//Valore del giocatore 2
#define vI 3				//Valore per segnare la combinazione vincente

//Valori che può assumere f in gioca
#define FGioca 0		//Partita in corso
#define FG1 1				//Ha vinto il giocatore 1
#define FG2 2				//Ha vinto il giocatore 2
#define FPareggio 3 //Pareggio

//Varie
#define CN 15							//Numero dei caratteri dei nomi dei giocatori
#define NGV 4							//Numero Gettoni per la Vittoria
//#define CLS system("cls")	//Funzione per pulire lo schermo su windows
#define CLS fputc_cons(12)	//Funzione per pulire lo schermo su windows

//Tipologia di Gioco
#define UvsU 1			//User contro User
#define UvsC 2			//User contro Computer
#define CvsC 3			//Computer contro Computer

//Tipologia Giocatore
#define User 1
#define CPU  2


//Prototipi funzioni
void azzera(int m[RC]);
void stampa(int m[RC]);
char stampaCasella(int x);
void stampaTitolo(void);
void stampaRigaPiena(void);
void gioca(char n1[CN], char n2[CN], int tg1, int tg2);
int mossaPC(int m[RC], int gp, int ga);
int inserisci(int m[RC], int c);
int controllaF(int m[RC], const int g, const int r, const int c, const int e);
int giocatoreP(int g);
int esiste(int r, int c);


//Corpi delle funzioni
int main(int argc, char *argv[]){
  char n1[CN];
  char n2[CN];
  unsigned char r, k=0;
  int m[RC], tipoGioco=0, tg1, tg2;
  srand(time(NULL));
  
  sprintf(n1,"Player A");
  sprintf(n2,"Player B");

  do{
  	CLS;
  	stampaTitolo();
  	printf("\n\t 1. User vs User");
		printf("\n\t 2. User vs Computer");
		printf("\n\t 3. Computer vs Computer");
  	printf("\n\n   Choose the game type: ");
  	scanf("%d", &tipoGioco);
  	fflush(stdin);
  }while(tipoGioco<1||tipoGioco>3);
  
  if(tipoGioco!=CvsC){
  	printf("Insert the 1st player's name:\n   ");
    scanf("%s", n1);
    fflush(stdin);
    if(tipoGioco==UvsU){
    	printf("Insert the 2nd player's name:\n   ");
      scanf("%15s", n2);
      fflush(stdin);
    }else
    	strcpy(n2, "CPU1");
  }else{
 	  strcpy(n1, "CPU1");
    strcpy(n2, "CPU2");
  }
  
  do{
    if(tipoGioco==UvsU)
      tg1=tg2=User;
    else if(tipoGioco==UvsC){
    	tg1=User; tg2=CPU;
    }else
      tg1=tg2=CPU;
    
		gioca(n1, n2, tg1, tg2);
    printf("\n\nAnother game? (y/n)");
    scanf("%c", &r);
    fflush(stdin);
  }while(r=='y' || r=='Y');
  return 0;
}


//###############################
void gioca(char n1[CN], char n2[CN], int tg1, int tg2){
  int m[RC], t=1, f, nt=0, r, c, g=g2I;
  
  f=FGioca;
  azzera(m);
  
  do{
    CLS;
    stampaTitolo();
    g=giocatoreP(g);
//printf("g=%d   tg=%d\n",g, g==g1I?tg1:tg2);
    printf("Move number: %d\n",++nt);
    if(t)
      //printf("E' il turno di %s (%c)\n\n", n1,g1);
	  printf("%s's turn (%c)\n\n", n1,g1);
    else
      //printf("E' il turno di %s (%c)\n\n", n2,g2);
	  printf("%s's turn (%c)\n\n", n2,g2);
    stampa(m);
    
    //Se gioca l'utente
    if( (g==1 && tg1==User) || (g==2 && tg2==User)){
      while(1){
        //printf("In che colonna vuoi mettere il gettone?  ");
		printf("Enter your move:  ");
        scanf("%d", &c);
			  fflush(stdin);
			  c--;
        if(c<0 || c>=C)
          continue;
        r=inserisci(m,c);
        if(r!=-1)
          break;
      }
    }else{ // Se gioca il computer
 	    c=mossaPC(m, g, giocatoreP(g));
 	    r=inserisci(m,c); //mossaPc ritorna una colonna valida e con spazio
 	    m[r*8+c]=g;
      //printf("\nc = %d \n", c+1);
      //getch();
    }
    
    m[r*8+c]=g;
    f=controllaF(m,g,r,c,1) ? g : f;
    if(nt==R*C && f==0){
      f=FPareggio;
      break;
    }
  }while(!f);
  
  CLS;
  stampaTitolo();
  stampa(m);
  switch(f){
    case 1:
      //printf("\n\nHa vinto %s (%c)", n1, g1);
	  printf("\n\n%s wins (%c)", n1, g1);
      break;
    case 2:
      //printf("\n\nHa vinto %s (%c)", n2, g2);
	  printf("\n\n%s wins (%c)", n2, g2);
      break;
    case 3:
      //printf("\n\nPareggio");
	  printf("\n\nNo winner");
      break;
  }
}

//###############################
int mossaPC(int m[RC], int gp, int ga){
	int r,c,m2[RC],cas,g,i;
	
	//Riempio la matrice di supporto
	for(r=0; r<R ;r++)
	  for(c=0; c<C; c++)
	    m2[r*8+c]=m[r*8+c];
	
	//printf("mossaPC ( m, %d, %d )",gp,ga);
	
	// 1° ciclo Controlla se può vincere
	// 2° ciclo controlla se l'avversario può vincere
 	for(i=0; i<2; i++){
		g=(i? ga : gp);
	  for(c=0;c<C;c++){
	  	r=inserisci(m2,c);
	  	m2[r*8+c]=g;
	  	//printf("\n  c=%d  r=%d controllaF( m, %d, %d, %d, 0)=%d", c,r,g,r,c,controllaF(m2, g, r, c, 0));
	    if(r!=-1 && controllaF(m2, g, r, c, 0))
	      return c;
      m2[r*8+c]=vuotoI;
    }
  }
  
  while(1)   //Controlla se nella colonna casuale c'è spazio
    if(inserisci(m2,cas=rand()%C)!=-1)
      return cas;
}

//###############################
int giocatoreP(int g){
	return g==g1I ? g2I : g1I;
}	


//###############################
int inserisci(int m[RC], int c){
  int i;
  for(i=R-1; i>=0; i--)
    if(m[i*8+c]==vuotoI)
      return i;
  return -1;
}


//###############################
int controllaF(int m[RC], const int g, const int r, const int c, const int e){
	int r2, c2, x, ret=0, i, ok;
	
	//Controllo diagonale \ ___
	if(esiste(r-1,c-1) && m[(r-1)*8+c-1]==g)
	  if(x=controllaF(m,g,r-1,c-1,e)!=0)
	    ret=ret||x;
  if(r<R-(NGV-1) && c<C-(NGV-1)){
  	ok=1;
    for(i=0; i<NGV && (ok*=(m[r*8+c]==m[(r+i)*8+c+i] || m[(r+i)*8+c+i]==vI)); i++) ;
    if(ok){
    	if(e)
        for(i=0; esiste(r+i, c+i) && m[(r+i)*8+c+i]==g; i++)
          m[(r+i)*8+c+i]=vI;
      ret=1;
    }
  }
  
  //Controllo diagonale /  ___
	if(esiste(r+1,c-1) && m[(r+1)*9+(c-1)]==g)
	  if(x=controllaF(m,g,r+1,c-1,e)!=0)
	    ret=ret||x;
  if(r>(NGV-2) && c<C-(NGV-2)){
  	ok=1;
    for(i=0; i<NGV && (ok*=(m[r*8+c]==m[(r-i)*8+c+i]) || m[(r-i)*8+c+i]==vI); i++) ;
    if(ok){
    	if(e)
        for(i=0; esiste(r-i, c+i) && m[(r-i)*8+c+i]==g; i++)
          m[(r-i)*8+c+i]=vI;
      ret=1;
    }
  }
  
	//Controllo riga
	if(esiste(r,c-1) && m[r*8+c-1]==g)
	  if(x=controllaF(m,g,r,c-1,e)!=0)
	    ret=ret||x;
  if(c<C-(NGV-1)){
  	ok=1;
    for(i=0; i<NGV && (ok*=(m[r*8+c]==m[r*8+c+i] || m[r*8+c+i]==vI)); i++) ;
    if(ok){
    	if(e)
        for(i=0; esiste(r, c+i) && m[r*8+c+i]==g; i++)
          m[r*8+c+i]=vI;
      ret=1;
    }
  }
	
	//Controllo colonna
	if(r<R-(NGV-1)){
	  ok=1;
		for(i=0; i<NGV && (ok*=(m[r*8+c]==m[(r+i)*8+c] || m[(r+i)*8+c]==vI)); i++) ;
		if(ok){
    	if(e)
			  for(i=0; i<NGV; i++) //Non serve controllare se sono più di 4
          m[(r+i)*8+c]=vI;
      ret=1;
		}
	}
	
  return ret;
}


//###############################
int esiste(int r, int c){
	if(r>=0&&r<R && c>=0&&c<C)
	  return 1;
	return 0;	
}


//###############################
void azzera(int m[RC]){
  int i,j;
  for(i=0; i<R; i++)
    for(j=0; j<C; j++)
      m[i*8+j]=0;
}


//###############################
void stampa(int m[RC]){
  int i,j;
  printf("\n");
//  stampaRigaPiena();  //Stampo prima riga |---|---|--...
  
  for(i=0; i<R; i++){ //Stampo le righe centrali
    printf("|");
    for(j=0; j<C; j++)
      printf(" %c |",stampaCasella(m[i*8+j]));
    printf("\n");
//    stampaRigaPiena();
  }
  stampaRigaPiena();  //Stampo prima riga |---|---|--...
  printf("|");
  for(j=1; j<=C; j++)
    printf(" %d |", j);
  printf("\n");
//  stampaRigaPiena();  //Stampo prima riga |---|---|--...
  printf("\n");
}

void stampaRigaPiena(){
  int i,j;
  printf("|");
  for(j=1; j<=C; j++)
    printf("---|");
  printf("\n");
}

char stampaCasella(int x){
	char c;
	switch(x){
		case 0:
	    c=vuoto; break;
    case 1:
    	c=g1; break;
   	case 2:
    	c=g2; break;
   	case 3:
   		c=CVitt; break;
	}
	return c;
}
void stampaTitolo(void){
//  printf("\n\n\t ### Forza quattro ###\n\n");
  printf("\n\n     ### Connect four ###\n\n");
}
