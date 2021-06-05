/*************************************************************
                  f l o a t . c p p
**************************************************************
 Programm zur Berechnung grosser floating point Zahlen
 Siehe KIM math subroutines programming manual
 Jannuary 1977 Commodore MOS Technology
 Fuer 6502 Microprozessor assembler listing

 Uebersetzt von Turbo Pascal
 22-Sep-98
 Letzte Aenderung : 013-Oct-98 24:00
                      5-Dez-98 addsub und wurzel ausgebessert
                      9-Dez-98 eulerzahl und ln hinzugefuegt
                     11-Dez-98 ln 10 dazu
                     13-Dez-98 ln fertig fÅr alle Zahlen
                     14-Dez-98 test ln und log
                     19-Dez-98 10^x dazu und getestet
                               bug log fixed
                     22-Dez-98 10^x Trennung der Zahl weiter
                     26-Dez-98 10^x fertig  mit Test
                     27-Dez-98 arctan test
                     29-Dez-98 arctan weiter und verbessern
                     30-Dez-98 arcsin/ arccos fertig und getestet,
                               arctan verbessert und getestet
                      1-Jan-99 sin/cos
                      2-Jan-99 sin/cos verbessert und getestet
                      4-Jan-99 y^x begonnen
                      6-Jan-99 y^x und x wurzel y fertig
                      8-Jan-99 e^x dazu
                     10-Jan-99 y^x Test
                     22-Jan-99 neuer Algorithmus fuer PI
                     31-Jan-99 Gauss AGM Algorithmus
                     20-Feb-99 Versuch, mult schneller zu machen
                               e^(pi*sqrt(n)) berechnet
**************************************************************/
/* Die Rechenoperationen werden in folgenden Registern durch-
   gefuehrt :
   rx, ry, rz
   Funktionen : add_sub(a) : rz = rx + ry
                add_sub(s) : rz = rx - ry
                multiply    : rz = rx * ry
                divide    : rz = rx / ry
                wurzel    : rz = sqrt(rx)
   Aufbau der floating point Zahl :
    Vorzeichen Mantisse : 0 ... positiv  1 ... negativ
    Vorzeichen Exponent : 0     positiv  1 ... negativ
    Mantisse            : array  0..LÑnge, Feld 0 bleibt frei
                           an jeder Position steht die Zahl 0-9
    Exponent            : Integer Zahl, nur positiv
   Der (fiktive) Dezimalpunkt steht immer nach der 1. Stelle, das Ergebnis
   aller Operationen wird immer in dieser Darstellung prÑsentiert.
   Die Eingabe jeder Zahl in die Register muss immer in diesem
   Format erfolgen !!
   Beispiel : 3.88888 e-77  , 1.0 e0,  -9.999 e33
   Beispiel fÅr Eingabe : Zahl 1.9e-33
   rx.ma[1]=1; rx.ma[2]=9 ; rx.se = 1 ; rx.ex = 33; rx.sm = 0;

   Beachte, daò addsub die Register rx, ry so vertauscht, dass
   in rx immer der groesste Absolutbetrag steht.
   alle Register mÅssen vom Typ 'wortform' sein.
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
//#include <conio.h>
#include <math.h>

/* maximale Werte fÅer n = 1030, LÑenge = 1050
   LÑnge sollte immer um ca 20 gr˜sser sein   */

const int laenge=550;  /* Laenge des Mantissen arrays 0 - Laenge */
		int maxexp=2000;  /* max Groesse des Exponent */
      int n		 =500;  /* Zahl der signifikanten Stellen <= laenge */

typedef struct wortform
{
  short sm,se; /* Vorzeichen Mantisse, Exponent */
  int   ex;    /* Exponent */
  short ma[laenge];  /* Mantisse , 0 nicht verwendet */
} wortform1;

unsigned long countminus=0;
unsigned long countplus=0;

wortform rx,ry,rz;
wortform ra,rb,rq;
wortform rstore, pout, rn, rm;
wortform sieben8, null5, null0;
  /* Variable fÅr PI */
wortform zwei, eins, zahl1d18, zahl1d57, zahl1d239,
sum1, sum2, sum3, zahl1d57_2, zahl1d18_2, zahl1d239_2,
nenner, zaehler1, zaehler2, zaehler3, x0,
xx1, xx2, xx3, xxx, piold ;
 /* Variable fÅr Euler */
 wortform eulerz;
 /* Varibale fuer ln */
 wortform zzz;
 /* Variablen fuer Wilkinson Polynom */
wortform koef0,koef1, koef2, koef3, koef4 ,koef5, koef6,
koef7, koef8, koef9, koef10, koef11, koef12, koef13, koef14,
koef15, koef16, koef17, koef18, koef19, koef20, yyy, dy;

int  cnt, cnta, overr;
bool		xztst_flag,yztst_flag,aztst_flag;
bool 		multdiv;
short 	borrow;
/************************************************************/
/* Prototypen */
void x_zero_test();
void y_zero_test();
void a_zero_test();
void clear_rabq();
void clear_rz();
void infin();
void x_swap_y();
void move_rx_to_ra();
void move_ry_to_rb();
void move_rx_to_rq();
void move_rq_to_ra();
void move_ra_to_rz();
void leftshift_ra();
void rightshift_ra();
void ra_plus_rb();
void round_rb_off();
void rightshift_rb_count();
void ra_minus_rb();
void compare_x_y();
void print_zahl_s(char s[50]);
void mult_rb_rq();
void div_ra_rb();
void mult4();
void multiply();
void divide();
void add_sub(char addsubflag[1]);
void wurzel();
void pigauss();
void wilkilson();
void eulerzahl();
void logarithmusn();
void ln10();
void zehnhochx();
void arcustangens();
void arcussinus();
void sinus();
void cosinus();
void ypsilonhochx();
void ehochx();
void piramanujan();
void pigaussagm();
void wurzelk(); void wurzelk1();
void ehochpiwu();

/************************************************************/
/* teste x auf 0 */
void x_zero_test()
{
 int i;
 xztst_flag=true;
 i=n;
 do
 {
  if(rx.ma[i]!=0) xztst_flag=false;
  i--;
 }while(!((i==0)||(!xztst_flag)));
}
/************************************************************/
/* teste y auf 0 */
void y_zero_test()
{
 int i;
 yztst_flag=true;
 i=n;
 do
 {
  if(ry.ma[i]!=0) yztst_flag=false;
  i--;
 }while(!((i==0)||(!yztst_flag)));
}
/************************************************************/
/* teste ra auf 0 */
void a_zero_test()
{
 int i;
 aztst_flag=true;
 i=n;
 do
 {
  if(ra.ma[i]!=0) aztst_flag=false;
  i--;
 }while(!((i==0)||(!aztst_flag)));
}
/************************************************************/
/* clear ra,rb,rq */
void clear_rabq()
{
 int i;
 for(i=laenge - 1; i>=0; i--)
 {
  ra.ma[i]=0;
  rb.ma[i]=0;
  rq.ma[i]=0;
 }
 ra.sm=0; rb.sm=0; rq.sm=0;
 ra.se=0; rb.se=0; rq.se=0;
 ra.ex=0; rb.ex=0; rq.ex=0;
}
/************************************************************/
/* clear rz */
void clear_rz()
{
 int i;
 for(i=laenge - 1 ; i>=0; i--)
 	rz.ma[i]=0;
 rz.sm=0; rz.se=0; rz.ex=0;
}
/************************************************************/
/* st rz=9.99999 e99 und overr=1 */
void infin()
{
 int i;
 for(i=1; i<=n; i++)
 	rz.ma[i]=9;
 rz.ex=maxexp; rz.sm=0; rz.se=0;
 printf("\n Attention : overflow \n");
 overr =1;
}
/************************************************************/
/* swap rx und ry , alle Teile */
void x_swap_y()
{
 rstore=ry; ry=rx; rx=rstore;
}
/***********************************************************/
/* move rx to ra */
void move_rx_to_ra()
{
 int i;
 for(i=n+1; i>=1; i--)
 	ra.ma[i]=rx.ma[i];
}
/***********************************************************/
/* move ry to rb */
void move_ry_to_rb()
{
 int i;
 for(i=n+1; i>=1; i--)
 	rb.ma[i]=ry.ma[i];
}
/***********************************************************/
/* move rx to rq */
void move_rx_to_rq()
{
 int i;
 for(i=n+1; i>=1; i--)
 	rq.ma[i]=rx.ma[i];
}
/***********************************************************/
/* move rq to ra */
void move_rq_to_ra()
{
 int i;
 for(i=n+1; i>=0; i--)
 	ra.ma[i+1]=rq.ma[i];
}
/***********************************************************/
/* move ra to rz */
void move_ra_to_rz()
{
 int i;
 for(i=n+1; i>=1; i--)
 	rz.ma[i]=ra.ma[i];
}
/**********************************************************/
/* left shift ra one digit, ganz links ist 0 */
void leftshift_ra()
{
 int i;
 for(i=0; i<=n; i++) {
 	ra.ma[i]=ra.ma[i+1];
 }
 ra.ma[i+1]=0;
}
/**********************************************************/
/* right shift ra one digit, ganz links ist 0 */
void rightshift_ra()
{
 int i;
 for(i=n-1; i>=0; i--){
 	ra.ma[i+1]=ra.ma[i];
 }
 ra.ma[0]=0;
}
/******************************************************/
/* add rb to ra, bis ra[0] */
void ra_plus_rb()
{
 int i;
 countplus++;
 short carry=0;
 for(i=n; i>=0; i--)
 {
  ra.ma[i] = ra.ma[i] + rb.ma[i] + carry;
  // ra.ma[i] = ra.ma[i] + carry;
  carry=0;
  if(ra.ma[i]>9)
  {
   ra.ma[i]=ra.ma[i]-10;
   carry=1;
  }
 }
}
/*******************************************************/
/* nimm letzte Stelle n weg und runde auf;
   was passiert mit der letzten Stelle ?? */
void round_rb_off()
{
 int i;
 short carry;
 if(rb.ma[n+1] >= 5)
 	carry=1;
 else
 	carry=0;
 for( i=n; i>=0; i--)
 {
  rb.ma[i]=rb.ma[i]+ carry;
  carry=0;
  if(rb.ma[i] > 9)
  {
   rb.ma[i]=rb.ma[i]-10;
   carry=1;
  }
 }
}
/**********************************************************/
/* right shift rb cnt digits, links 0 einfÅgen
	wenn cnt=0, dann round off */
void rightshift_rb_count()
{
 int i,j;
 if(cnt==0) round_rb_off();
 for(j=1; j<=cnt; j++)
 {
  for(i=n; i>=0; i--)
  	rb.ma[i+1]=rb.ma[i];
  ra.ma[0]=0;
 }
}
/**********************************************************/
/* subtract rb from ra, bis ra[0]
   ra immer >= rb */
void ra_minus_rb()
{
 countminus++;
 int i;
 short x,carry=0;
 for(i=n; i>=0; i--)
 {
  x= rb.ma[i] + carry;
  carry = 0;
  if(ra.ma[i] < x)
  {
   ra.ma[i] = ra.ma[i]+10;
   carry = 1;
  }
  ra.ma[i] = ra.ma[i] - x;
 }
 borrow = carry;
}
/********************************************************/
/* compare x and y, substract y from x, cnta =1 if x < y
	x and y are not modified */
void compare_x_y()
{
 int i;
 short carry,x,y;
 carry=0;
 cnta=0;
 for(i=n+1; i>=0; i--)
 {
  x=rx.ma[i];
  y=ry.ma[i]+carry;
  carry=0;
  if(x<y)
  {
   /* x=x+10;  */
   carry=1;
  }
  /*brauchen wir die naechste Zeile ?? */
  /* x=x-y;  */
 }
 if(carry==1) cnta=1;
}
/**********************************************************/
/* Drucke in +-xxxxxxx E+-yyy */
void print_zahl_s(char s[30])
{
 int i;
 printf("%s\n",s);
 if(pout.sm ==1)
 	printf("-");
 else
 	printf("+");
 for(i=0; i <=70;i++)
 {
 	printf("%1d",pout.ma[i]);
	if(i==1)
   	printf(".");
 }
 printf("\\\n   ");
 for(i=71;i<=140;i++){
   if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
 }
 printf("\\\n   ");
 for(i=141;i<=210;i++){
   if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
 }
 printf("\\\n   ");
 for(i=211;i<=280;i++) {
   if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
 }
 printf("\\\n   ");
  for(i=281;i<=350;i++){
    if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
  }
 printf("\\\n   ");
  for(i=351;i<=420;i++){
    if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
  }
 printf("\\\n   ");
 for(i=421;i<=490;i++){
   if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
 }
 printf("\\\n   ");
 for(i=491;i<=560;i++) {
   if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
 }
 printf("\\\n   ");
  for(i=561;i<=630;i++) {
    if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
  }
 printf("\\\n   ");
  for(i=631;i<=700;i++) {
    if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
  }
 printf("\\\n   ");
  for(i=701;i<=770;i++){
    if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
  }
 printf("\\\n   ");
  for(i=771;i<=840;i++) {
    if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
  }
 printf("\\\n   ");
 for(i=841;i<=910;i++) {
   if( i > n )goto ende;
 	printf("%1d",pout.ma[i]);
 }
 printf("\\\n   ");
 for(i=911;i<=980;i++) {
   if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
 }
 printf("\\\n   ");
 for(i=981;i<=1030;i++) {
   if( i > n ) goto ende;
 	printf("%1d",pout.ma[i]);
 }
 ende:
 printf("   ");
 printf(" E");
 if(pout.se==1)
 	printf("-");
 else
 	printf("+");
 if(pout.ex>999)
 	printf("%4d",pout.ex);
 else if(pout.ex>99)
 	printf("%3d",pout.ex);
 else if(pout.ex>9)
 	printf("%2d",pout.ex);
 else
 	printf("%1d",pout.ex);
 printf("\n");
}
/**********************************************/
/* Berechne das Produkt der Mantissen von rq und rb durch
   wiederholte Addition. rb wird mit jeder Stelle von rq
   zu ra addiert, Ergebnis in ra, schiebe dann
   ra weiter . ra_plus_rb in die function integriert.
   Vielleicht wird mult schneller */
void mult_rb_rq()
{ int ii,i,jj ; short carry=0;
 for(i=n-1; i>=1; i--) {
    // cnt = rq.ma[i];
      for (jj=1; jj<= rq.ma[i]; jj++) {
        // printf("cnt,jj,i %d %d %d \n",cnt,jj, i);
        // void ra_plus_rb()
        //int i;
        countplus++;
         for(ii=n; ii>=0; ii--)
         {
          ra.ma[ii] = ra.ma[ii] + rb.ma[ii] + carry;
          // ra.ma[i] = ra.ma[i] + carry;
          carry=0;
          if(ra.ma[ii]>9)
           {
             ra.ma[ii]=ra.ma[ii]-10;
             carry=1;
           }
         }
        // uuuuu
        // ra_plus_rb();
      }
    rightshift_ra();
 }
 leftshift_ra();
}
/**********************************************/
/* Berechne den Quotient der Mantissen durch wieder-
   holte Subtraktion ra/rb. Ergebnis in rq
   Subtrahiere rb solange von ra, bis ra < rb, addiere
   dann wieder rb dazu, um die letzte Zahl groesser rb
   wieder zu bekommen. Dann wird ra links geschoben, usw .
   Die Zahl der Subtraktionen entspricht einer Stelle des
   Ergebnisses, Ergebnis in rq, beginnend von 0 bis n */
void div_ra_rb()
{ int j;
  j = 0;
  do
  {
    cnt = 0;
    div1:
     ra_minus_rb();
     // pout=ra; print_zahl_s("ra_minus_rb :");
     // cin >> eingabe;
     //getchar();
    if(ra.ma[0] == 9){
      goto div2;
    } else {
      cnt++;
      if(cnt <= 9) { goto div1;
       /* da die Zahlen immer normiert sind , darf diese Schleife
          maximal 9 * durchlaufen werden, sonst Fehler */
       } else { printf("error div_ra_rb : illegal condition, cnt =%d \n",cnt);}
    }
    div2:
    ra_plus_rb(); /* Divisor addieren fÅr positiv ra */
    leftshift_ra();  /* nÑchste Stelle */
    rq.ma[j] = cnt;
    j++;
  } while (!( j > n));
//  pout=rq ; print_zahl_s("div_ra_rb : rq :");
}

/**********************************************/
/* addiere und subtrahiere : rz = rx + ry
                             rz = rx - ry */
void add_sub(char addsubflag[1])
{
  /*  1. Teil : groesster Absolutbetrag steht in rx
                Vorzeichen und Mantisse,
                1.5e-2 ist gr˜sser als 1.5e-13
                Das wird wegen der Rechengenauigkeit gemacht */
  /* schalte Vorzeichen um, wenn Subtraktion */
//   printf("addsub: %s\n",addsubflag);
 if(!strcmp(addsubflag ,"s")) {
//   printf("subtrahiere: %s\n",addsubflag);
      ry.sm = ry.sm ^ 1;
  }else{
//  printf("addiere: %s\n",addsubflag);
  }
  clear_rabq();
  x_zero_test();
  if(xztst_flag) {
    x_swap_y();
    goto add3;
  }
  y_zero_test();
  if(yztst_flag) goto add3;
  if((rx.se ^ ry.se) == 0) {
  /* gleiches Vorzeichen Exponent  */
  // printf("gleiches Vorzeichen Exponent \n");

   compare_x_y();
   if(cnta == 1) x_swap_y();
   l1:
   if(ry.ex > rx.ex) {
     x_swap_y();
     goto l1;
   } else {
     if (ry.ex < rx.ex) goto add1;
   }
  } else {
   /* verschiedenes Vorzeichen Exponent */
   // printf("verschiedenes Vorzeichen Exponent \n");
   add1:
   // printf("rx.se %d\n",rx.se);
   if(rx.se == 1){ /* printf("vor swap\n"); */  x_swap_y();   }
  }
  add3:
  // pout=rx; print_zahl_s("add1 : rx ");
  // pout=ry; print_zahl_s("add1 : ry ");
  /* 2. Teil : bestimme Unterschied der Exponenten fuer shift
               der Mantisse */
  if((rx.se ^ ry.se) == 0){   /* gleiches Vorzeichen Exponent */
   if(rx.se == 1) {
     cnt= ry.ex - rx.ex;
   } else {
     cnt=rx.ex - ry.ex;
   }  /* verschiedenes Vorzeochen exponent */
  } else {
    cnt = rx.ex + ry.ex ;
    if( cnt > maxexp ) goto add5;
  }
  if( cnt > n ) goto add5;
  move_ry_to_rb();
  rightshift_rb_count(); /* schiebe rb cnt mal nach rechts */
  round_rb_off();
  add5:
  /* das Ergebnis hat immer den Exponenten von x */
  rz.ex = rx.ex;
  move_rx_to_ra();
/* 3. Teil: addiere oder subtrahiere rb zu/von ra in Ab-
            haengigkeit vom Vorzeichen der Mantisse und
            korrigiere das Ergebnis */
if((rx.sm ^ ry.sm) == 0) {
/* addiere
---------- */
  ra_plus_rb();
  if(ra.ma[0] != 0) {
    /* ra + rb gibt Uebertrag, korrigiere Exponent */
    rightshift_ra();
    if(rx.se == 0) {
     rz.ex = rx.ex + 1;
     if(rz.ex < maxexp){
      goto add120;
     } else {
       infin();
       goto fertig;
     }
    } else {
      rz.ex= rx.ex - 1;
      if(rz.ex == 0 ){
        rz.se = 0;
        rz.sm = rx.sm ;
        goto add121;
      } else {
        goto add120;
      }
    }
  }
  add120:
    rz.se = rx.se;
    rz.sm = rx.sm;
  add121:
    move_ra_to_rz();
    goto fertig;
} /* Ende addiere */
if((rx.sm ^ ry.sm) == 1) {
/* ungleiches Vorzeichen Mantisse */
/* subtrahiere
   ----------- */
// printf("subtrahiere \n");
/* pout=ra; print_zahl_s("sub ra : ");
  pout=rb; print_zahl_s("sub rb : ");  */
   ra_minus_rb(); /* ra = ra - rb */
   rz.ex = rx.ex;
   rz.se = rx.se;
   rz.sm = rx.sm;
   a_zero_test();
   if(aztst_flag) {
     clear_rz();
     goto fertig;
   }
   /* Die Subtraktion kann sehr kleine Werte in der Mantisse
      ergeben, dann muss wieder verschoben werden und der Exponent
      korrigiert werden, z.B. 3.5555e2 - 3.5554e2 ergibt
      0.0001e2, muss wieder 1.0e-4 werden. Das passiert im
      folgenden Block mit loop */
   subloop:
   // printf("nach subloop\n");
   if(ra.ma[1] != 0) {
     move_ra_to_rz();

     goto fertig;
   }
  // printf("vor lefthsift\n");
   leftshift_ra();
  // printf("nach left shift ra \n");
/* Wert des Exponenten ist immer positiv : hier wird
   das Vorzeichen umgedreht */
   if(rz.se == 0 ) {
     rz.ex--;
     if(rz.ex < 0) {
       rz.ex = 1;
       rz.se = 1;
     }
       goto subloop;

   } else {
     rz.ex ++ ;
     if(rz.ex > maxexp) {
       clear_rz();
       goto fertig;
     }
     goto subloop;
   }
} /* subtrahiere */
fertig:
;
} /* ende add_sub */
/**********************************************/
/* Korrektur fÅr mult und div
   Korrigiere das Ergebnis und den Exponent
   bestimme Vorzeichen Exponent und addiere Exponent  */
void mult4()
{ 
 if ((rx.se ^ ry.se) == 0 ) {
 /* gleiches Vorzeichen Exponent */
    if ( (rx.ex + ry.ex) > maxexp ){
    /* overflow */
      if( (rx.ex + ry.ex) != 0 ){
       /* zu klein oder zu groò */
       rz.se = rx.se ; rz.sm = rx.sm ;
         if(rz.se == 1) { clear_rz();} else { infin();}
       goto fertig;
      }
      if(multdiv == true) {  /* fÅr div */
//        printf("div teil mult4 1\n");
          if(rx.ex ==1) {
             clear_rz();
             goto fertig;
          }
          if(cnta == 0) {
            infin();
            goto fertig;
          }
          cnta = 0 ;
          goto mdov1;
      }
      if(rx.se == 0 ) {
        infin();
        goto fertig;
      }
      if(ra.ma[0] == 0) {
        rightshift_ra(); /* mdov1, wann kommt das vor ? */
      }
      mdov1:
      rz.ex = maxexp;
      goto md2;
    } else {
    rz.ex = rx.ex + ry.ex; goto md2;
    }
 } else {
 /* verschiedenes Vorzeichen Exponent */
    if((rx.ex - ry.ex) < 0 ){
       rz.ex = ry.ex - rx.ex;
       rz.se = ry.se ; rz.sm = ry.sm;
       goto md3;
    } else {
       rz.ex = rx.ex - ry.ex;
       goto md2;
    }
 } /* Ende verschiedenes Vorzeichen */
 /* Åbernimm richtiges Vorzeichen vom Exponent, wenn Differenz 0,
    dann setze sign exp = 0 */
  md2:
    if( rz.ex == 0 ) {
      rz.se = 0; rz.sm = rx.sm;
    } else{
      rz.se = rx.se ; rz.sm = rx.sm;
    }
  md3:
  /* Mantisse */
  /* minus * minus = plus */
  if((rx.sm ^ ry.sm) == 0){
    /* gleiches Vorzeichen Mantisse */
    rz.sm = 0;
  } else {
    /* minus mal plus */
    rz.sm = 1;
  }
  /* dividiere*/
  /* bei der Division wird unterschieden, ob x >= y (cnta = 0)
     oder x < y. Dann muò der Exponent noch extra korrigiert werden */
  if(multdiv == true) {  /* div*/
//    printf("div teil mult4 2\n");
    if(cnta == 0){
      move_ra_to_rz();
      goto fertig;
    }
    dvext0:
    if(rz.se == 0) {
      /* sign exponent positiv */
      if(rz.ex == 0) {
           rz.se = 1;
           goto dvext0;
      } else {
           rz.ex--;
           move_ra_to_rz();
           goto fertig;
      }
    } else {
      /* sign exponent negativ */
      rz.ex++;
      if(rz.ex == maxexp) {
        if(rz.se == 1) { infin();} else { clear_rz(); goto fertig;}
      } else {
        move_ra_to_rz(); goto fertig;
      }
    }
  } /* end div */
  /* ist das Ergebnis der Multiplikation > 10, z.B 9 * 2, dann
     steht die hoechste Stelle in ra.ma[0]; Schiebe rechts
     und korrigiere Exponent */
  if(ra.ma[0] == 0) {
    move_ra_to_rz();
    goto fertig;
  }
  rightshift_ra(); /* öbertrag, schiebe rechts */
  if(rz.se == 1) {
  /* sign Exponent negativ */
   rz.ex--;
   /* springe noch einmal zurÅck und setze Exp 0 */
   if(rz.ex == 0) {
     goto md2;
   } else {
     move_ra_to_rz(); goto fertig;
   }
  } else {
  /* Sign Exp positiv */
    rz.ex++;
    if(rz.ex == maxexp) {
      if(rz.se == 1) { clear_rz();} else { infin(); goto fertig;}
    } else {
      move_ra_to_rz();
      goto fertig;
    }
  }
  fertig:
;
}
/**********************************************/
/* rz = rx * ry */
void multiply()
{
  multdiv = false; /* mult */
  clear_rabq();
  cnt=0;
  /* Faktoren 0 ? */
  x_zero_test();
  if(xztst_flag){ clear_rz(); goto fertig;}
  y_zero_test();
  if(yztst_flag){ clear_rz(); goto fertig;}
  /* nicht 0 : ry nach rb und rx nach rq*/
  move_ry_to_rb();
  move_rx_to_rq();
  /* multipliziere Mantissen */
//   pout=rb; print_zahl_s("multiply: vor mult_rb_rq : rb");
//   pout=rq; print_zahl_s("multiply: vor mult_rb_rq : rq");
  mult_rb_rq();
//    pout=ra; print_zahl_s("multiply: nach mult_rb_rq : ra");
  /* berechne und korrigiere Exponent */
  mult4();
  fertig:
;
}
/**********************************************/
/* rz = rx / ry */
void divide()
{
  multdiv = true;  /* mult */
  clear_rabq();
  cnt = 0;
  /* Dividend, Divisor 0 ? */
  x_zero_test();
  if(xztst_flag){ clear_rz(); goto fertig;}
  y_zero_test();
  if(yztst_flag){ infin(); goto fertig;}
  clear_rabq();
  move_rx_to_ra();
  move_ry_to_rb();
  /* x groesser y, Ergebnis in rq */
  compare_x_y();
  div_ra_rb();
  /* Toggle Vorzeichen Exponent Divisor, Exponent wird gleich
     berechnet wie bei der Multiplikation
     1e3 / 1e-5 = e3 * e5 */
  ry.se = ry.se ^ 1 ;
  /* Ergebnis von div steht in rq, move nach ra */
  move_rq_to_ra();
  /* wann kommt die nÑchste Zeile vor */
  if(ra.ma[1] == 0){
    leftshift_ra();
  }
  /* Korrigiere Exponent, multdiv flag steuert mult/div */
  mult4();
  /* setze original Exp y wieder */
  ry.se = ry.se ^ 1;
  fertig:
;
}
/**********************************************/
/* Berechne Wurzel nach der Heron-Methode
   m0 = 7.8 , m1 =(zahl/m0 + m0) * 0.5,
   m2=(zahl/m1 + m1) * 0.5,
   rz = sqrt(rx)
   GÅltiger Bereich fÅr rx ist 1 -100
   Um die Wurzel aus jeder beliebig grossen oder kleinen
   Zahl ziehen zu koennen wird der Exponent vorher und nachher
   korrigiert, und zwar :
   sqrt(x) = 10 ^ (r/2) * sqrt(x) .....
   r = 0,2,4,6,8,.. 1 <= sqrt(x)< 10
   Der Exponent ist 0 oder 1, es wir die Wurzel x oder 10x be-
   rechnet. Der alte Exponent wird  durch 2 dividiert und abge-
   schnitten , Vorzeichen muss beruecksichtigt werden */
void wurzel()
{
 int i, exponent, sign_exponent;
 /* xxxxxxxx */
 /* Berechne Wurzel 0 */
 if(rx.ma[1]== 0) {
   rz = null0; return;
 }
 /* xxxxxxxx */
 /* Berechne neuen Exponenten  */
 if(rx.se == 0){
   /* exp positiv */
    sign_exponent = 0;
    exponent =(int)( rx.ex/2) ;
 } else {
   /* exp neg */
    sign_exponent = 1;
    if((rx.ex % 2) == 0){
      exponent = (int)(rx.ex/2);
    } else {
      exponent = (int)(rx.ex/2) + 1;
    }
 }
 /* sqrt(x) oder sqrt(10x) */
    if( (rx.ex % 2) == 0 ) {
      rx.ex = 0;
    } else {
      rx.ex = 1;
    }
 /* sign exp always + */
 rx.se = 0;
 rm = sieben8;
 rn = rx;
 /* 15 ist ausreichend fÅr 100 Stellen, bei groesserer
 Genauigkeit muss das getestet werden     */
for(i=1; i<=15; i++)
 {
  ry=rm; rx=rn; divide();
  // pout=rz; print_zahl_s("nach divide");
  ry=rz; rx=rm; add_sub("a");
  // pout=rz; print_zahl_s("nach add_sub");
  rx=rz; ry=null5; multiply(); rm=rz;
   //pout=rz; print_zahl_s("nach multiply");
 }
 /* korrigier Exponent Ergebnis */
  rz.ex = exponent;
  rz.se = sign_exponent;
}
/**********************************************/
/* berechne arcsin nach der Reihe x + 1*x^3/2*3 +
 + 1*3*x^5/2*4*5 + 1*3*5*x^7/2*4*6*7 +
   1*3*5*7*x^9/2*4*6*8*9 + ....
   arccos(x) = pi/2 - arcsin(x)
 Vergleich arcsin arctan :
   0.001   17    100 Stellen
   0.1     49
   0.2     70
   0.3     93
   0.4    122
   0.5    161   
   0.6    217
   0.7    310
   0.8    493
   0.9    exp overflow 2000  100
   0.9    283    30 Stellen
   0.99   overfl 30 Stellen
 arctan:
  0.99   3005   30 Stellen
  0.99   10959  100 Stellen
  0.9    1057   100 Stellen
  0.8    500
  0.7    315
  0.6    220
  0.5    163
  0.3     94
  0.1     50
  0.01    26
  arctan   arcsin    grad
   0.2       0.2      11
   0.4       0.37     21.8
   0.6       0.51     31
   0.8       0.62     38.6
   1         0.707    45
   1.2       0.76     50
   1.5       0.83     56
   2         0.89     63
   3         0.94     71.5
  Ergebnis : arctan 0.8 bis 1.2 Åber arcsin 0.62 bis 0.76 berechnen mit
   arctan(x) = arcsin(x/sqrt(1+x^2))
   arcsin von 0.8 bis 0.9999 Åber arctan 1.33 bis unendlich berechnen mit
   arcsin(x) = arctan(x/sqrt(1-x^2)) und arctan(x) = pi/2 - arctan(1/|x|)
   Rechenzeiten fuer 100 Stellen ( pi = 40 sek):
   1 .... pi
 Geschwindigkeit der arcsin-Berechnung Pentium 166 100 Stellen:
 ---------------------------------------------------------------
 Winkel rad    Funktion  Zeit   Iterationen   (pi 40s )
   0.999        arctan   15s + pi   37
   0.9          arctan   40s + pi  156
   0.81         arctan   80s + pi  347
   0.80         arcsin  120s       493
   0.75         arcsin   95s       383
   0.7          arcsin   75s       310
   0.6          arcsin   45s       217
   0.5          arcsin   30s       101
   */
 void arcussinus()
 {
   int i,minus, arcsinstellen, arctanstellen ;
   int arcsincosflag;
   // arcsincosflag = 0 : arcsin  arcsincosflag =1 :arccos
   arcsincosflag=1; // arccos
   arcsinstellen = 100; arctanstellen = 100;
   if(arcsincosflag==0) {
     printf("Berechne  arcsin: \n");
   } else {
     printf("Berechne  arccos: \n");
   }
     printf("------------------\n");
   zwei = null0; zwei.ma[1] = 2;
   eins = null0; eins.ma[1] = 1;
   sum1 = null0; piold = null0;
   minus=0;
   // Beispielzahl
   //0.9
   //rx=null0; rx.ma[1]=9;rx.ma[2]=0; rx.se=1; rx.ex=1;rx.sm=0;rx.sm=1;
   // 0.999
   //rx=null0; rx.ma[1]=9;rx.ma[2]=9;rx.ma[3]=9 ;rx.se=1; rx.ex=1;rx.sm=0;// rx.sm=1;
   // 1
   // rx=eins; rx.sm=1; //rx.ma[10]=1;
   // 0.81
    rx=null0; rx.ma[1]=3;rx.ma[2]=6; rx.se=1; rx.ex=1;rx.sm=0;// rx.sm=1;
   // rx=null0;
   // -0.5
   //rx=null0; rx.ma[1]=5;rx.ma[2]=0; rx.se=1; rx.ex=1; rx.sm=1;
   // 0.99
   // rx=null0; rx.ma[1]=9;rx.ma[2]=9; rx.se=1; rx.ex=1;rx.sm=0;
   //rx=eins;
   pout=rx ; print_zahl_s("Zahl:");
   // Ende Beispielzahl
   zzz= rx;
   // Abfrage auf negativ
   if(rx.sm==1) minus=1;
   // Abfrage auf 0 : arccos(0) = pi/2, arcsin(0)=0
   if(rx.ma[1] == 0) {
      if(arcsincosflag == 1) {
        // arccos(0) = pi/2 - arcsin(0)
        pigauss();
        rx=rz; ry=zwei; divide();
        pout=rz ; print_zahl_s("arccos 0:");
      } else {
        rz=null0;
        pout=rz ; print_zahl_s("arcsin 0:");
      }
        return;
    }
   // Abfrage auf genau 1
   // Abfrage auf |x|>1, nicht erlaubt , auch im Hauptmenue
   // 1 - |x|
   rx=eins; ry=zzz; ry.sm=0; add_sub("s");
   if(rz.ma[1]==0) {
     // arcsin(1) = pi/2  , arcsin(-1) = -pi/2
     // arccos(1) = 0 , arccos(-1) = pi
     if( arcsincosflag==0) {
        pigauss();
        ry=zwei; divide();
        if(minus==1) rz.sm=1;
        pout=rz ; print_zahl_s("arcsin(+-1)= +- pi/2:");
     } else {
        if(minus==1) {
          pigauss();
          pout=rz ; print_zahl_s("arccos(+-1)= 0/pi");
        } else {
          rz=null0;
          pout=rz ; print_zahl_s("arccos(+-1)= 0/pi");
        }
     }
     return;
   }
   if(rz.sm==1) {
    rz=null0; printf("arcsin : Betrag x > 1 nicht erlaubt \n");
    printf("arcsin/arccos beendet\n");
    return;
   }
   // pruefe auf |x| > 0.8  , 0.8 - |x|
   rx=null0; rx.ma[1]=8; rx.ma[2]=0; rx.ex=1; rx.se=1;
   ry=zzz; ry.sm=0; add_sub("s");
   if(rz.sm==0) {
       // Berechne die Reihe  fuer positive x bis 0.8 mit arcsin
       printf("berechne arcsin direkt\n");
       zaehler1 = zzz; zaehler1.sm=0;
       zaehler2 = eins; zaehler2.sm=1;  // -1
       x0 = null0; sum2=eins;
       rx=zzz; ry = zzz ; multiply(); zahl1d18_2 = rz;
       nenner = eins ; i=0; piold=null0;
       for(;;)
       {
         i++;
         // printf(" %d",i);
         /*  Taylorreihe */
         rx = zaehler1; ry=nenner; divide();
         rx = sum1; ry=rz; add_sub("a"); sum1 = rz;
         // berechne neuen Zaehler
         /* 1*x^3, 1*3*x^5, 1*3*5*x^7...*/
         rx = zaehler1 ; ry = zahl1d18_2; multiply(); zaehler1 = rz;
         rx = zaehler2 ; ry = zwei; add_sub("a"); zaehler2 = rz;
         rx = rz; ry = zaehler1; multiply(); zaehler1 = rz;
         // pout=zaehler1 ; print_zahl_s("Zaehler1 \n");
         // 0, 2, 4, 6,
         rx=x0; ry=zwei; add_sub("a"); x0=rz;
         rx=rz; ry=sum2; multiply(); sum2=rz;
         rx=zaehler2; ry=zwei; add_sub("a");
         rx=rz ; ry=sum2; multiply();nenner=rz;
         // pout=sum1 ; print_zahl_s("Summe \n");
         rx= sum1; ry = piold; add_sub("s");
         if(rz.ex > arcsinstellen) break ;
         piold = sum1;
   }
   } else { // Ende bis 0.8
       // berechne die Reihe von 0.81 bis 0.999 mit arctan  x/sqrt(1-x^2)
       //  = arctan(1.3 bis unendlich)
       printf("berechne arcsin ueber arctan\n");
       rx=zzz;ry=zzz; multiply(); rx=eins; ry=rz; add_sub("s");
       rx=rz; wurzel(); rx=zzz; ry=rz; divide() ;rz.sm=0;
       // arctan > 1 : arctan(x) = pi/2 - arctan(1/|x|)
       rx=eins; ry=rz; divide();
       // pout=rz; print_zahl_s("zum arctan direkt\n");
       zaehler1 = rz;
       rx=rz; ry = rz ; multiply(); zahl1d18_2 = rz;
       nenner = eins ; i=0; piold=null0; sum1=null0;
       printf("arctan Reihe :\n");
       for(;;)
       {
         i++;
         //printf(" %d",i);
         rx = zaehler1; ry=nenner; divide();
         if( (i % 2) != 0) {
            rx = sum1; ry=rz; add_sub("a"); sum1 = rz;
         } else {
            rx = sum1; ry=rz; add_sub("s"); sum1 = rz;
         }
         /* x^3,x^5,...*/
         rx = zaehler1 ; ry = zahl1d18_2; multiply(); zaehler1 = rz;
         rx = nenner; ry = zwei; add_sub("a"); nenner = rz;
         // pout=sum1 ; print_zahl_s("Summe \n");
         rx= sum1; ry = piold; add_sub("s");
         if(rz.ex > arctanstellen) break ;
         piold = sum1;
      }
      // pi/2 - ...
      printf("pi/2:\n");
      eulerz=sum1;
      // pout=rz; print_zahl_s("zum arctan \n");
      pigauss();
      // pout=rz; print_zahl_s("PI\n");
      rx=rz; ry=zwei; divide();
      rx=rz; ry=eulerz; add_sub("s"); sum1=rz;
   } // Ende 0.8 - 0.999
   // korrigiere Exponenten
   if(minus==1) sum1.sm=1;
   // Abfrage auf arcos/arcsin
   if(arcsincosflag == 1) {
      // arccos(x) = pi/2 - arcsin(x)
      eulerz=sum1;
      pigauss();
      rx=rz; ry=zwei; divide();
      rx=rz; ry=eulerz; add_sub("s"); sum1=rz;
      printf("arccos berechnet\n");
   }
    rz=sum1 ; pout=rz; print_zahl_s("arcsin/arccos x");
    printf("arcsin: Zahl der Iterationen =%d , Zahl der Stellen =%d\n",
            i, arcsinstellen);
 }
/**********************************************/
/* Berechne arctan nach der Reihe x - x^3/3! + x^5/5! -...
 Diese Reihe konvergiert nur fuer  |x| < 1
 Reihe konvergiert nur fuer kleine x schnell, fuer 1
 arctan 1 = pi/4
 arctan (-x) = - arctan(x)
 Fuer |x| > 1  : pi/2 - arctan (1/|x|)
 Fuer 0.8 < x <= 1.2 wird arctan ueber arcsin berechnet :
   arctan(x) = arcsin(x/sqrt(1+x^2))
 Geschwindigkeit der arctan-Berechnung Pentium 166 100 Stellen:
 ---------------------------------------------------------------
 Winkel rad    Funktion  Zeit   Iterationen  ( pi 40s )
    0.1         arctan    10s       50
    0.2         arctan    10s       71
    0.4         arctan    20s      124
    0.6         arctan    40s      220
    0.7         arctan    50s      315
    0.8         arctan    95s      501
    0.81        arcsin    70s      239
    0.9         arcsin    80s      275
    1.0          p1/4     40s
    1.1         arcsin   100s      366
    1.2         arcsin   120s      418
    1.21        arctan   150s+pi   586
    1.3         arctan   105s+pi   427
    2.0         arctan    30s+pi   163
    5.0         arctan    10s+pi    71
   10.0         arctan    10s+pi    50
   90.0         arctan     9s+pi    26

 arctan 0 = 0 eigene Berechnung
  0.99   3005   30 Stellen
  0.99   10959  100 Stellen
  0.9    1057   100 Stellen
  0.8    500
  0.7    315
  0.6    220
  0.5    163
  0.3     94
  0.1     50
  0.01    26   */
 void arcustangens()
 {
   int i,minus, breakflag, grossflag, arctanstellen, arcsinstellen ;
   int unten,oben;
   arctanstellen = 100; arcsinstellen = 100;
   printf("Berechne  arctan(x) \n");
   printf("--------------------\n");
   zwei = null0; zwei.ma[1] = 2;
   eins = null0; eins.ma[1] = 1;
   sum1 = null0; piold = null0;
   minus=0; grossflag=0;
   // Beispielzahl
   // 0.5
    rx=null0; rx.ma[1]=1;rx.ma[2]=1; rx.se=0; rx.ex=0;rx.sm=1;
    //rx=null0; rx.ma[1]=9;rx.ma[2]=0;rx.ma[3]=0; rx.se=0; rx.ex=1;rx.sm=1;
   //rx=eins;
   // -0.5
   // rx=null0; rx.ma[1]=1;rx.ma[2]=3; rx.se=0; rx.ex=0;rx.sm=0;
   // 0.99
   // rx=null0; rx.ma[1]=9;rx.ma[2]=9; rx.se=1; rx.ex=1;rx.sm=0;
   pout=rx ; print_zahl_s("Zahl:");
   // Ende Beispielzahl
   zzz= rx;
   // Abfrage auf negativ
   if(rx.sm==1) minus=1;
   // Abfrage auf 0
   if(rx.ma[1] == 0) {
      rz=null0;
      pout=rz ; print_zahl_s("arctan(0):");
      return;
   }
   // Abfrage auf 1
   rx=eins;ry=zzz; ry.sm=0; add_sub("s");
   if(rz.ma[1]==0){
     pigauss(); rx=rz; ry=null0; ry.ma[1]=4; divide();
     if(minus==1) rz.sm=1;
     pout=rz; print_zahl_s("arctan(1) = pi/4");
     return;
   }
   // Berechne die Reihe fuer positive x
   // pruefe auf |x| > 0.8  , 0.8 - |x|
   // pruefe auf |x| < 1.2  , 1.2 - |x|
   unten=0; oben=0;
   rx=null0; rx.ma[1]=8; rx.ma[2]=0; rx.ex=1; rx.se=1;
   ry=zzz; ry.sm=0; add_sub("s");
   if(rz.sm==1) unten=1;
   rx=null0; rx.ma[1]=1; rx.ma[2]=2; rx.ex=0; rx.se=0;
   ry=zzz; ry.sm=0; add_sub("s");
   if(rz.sm==0) oben=1;
   // wenn x nicht von 0.8 bis 1.2 , dann arctan, sonst arcsin
   if(!(unten==1 && oben==1) ) {
      // Berechne Reihe nach arctan
      // Abfrage auf |x| > 1
      zzz.sm=0;
      rx=zzz; ry=eins; add_sub("s");
      if(rz.sm == 0) {
         printf("x groesser 1\n");
         grossflag=1;
         // groesser 1 , berechne 1/x
         rx=eins; ry=zzz; divide(); zzz=rz;
      }
      // berechne Reihe nach arctan
      printf("Berechnung nach arctan\n");
      zaehler1 = zzz;
      rx=zzz; ry = zzz ; multiply(); zahl1d18_2 = rz;
      nenner = eins ; i=0; piold=null0;
      for(;;)
      {
        i++;
        // printf(" %d",i);
        /*  Taylorreihe */
        rx = zaehler1; ry=nenner; divide();
        if( (i % 2) != 0) {
           rx = sum1; ry=rz; add_sub("a"); sum1 = rz;
        } else {
           rx = sum1; ry=rz; add_sub("s"); sum1 = rz;
        }
        /* x^3,x^5,...*/
        rx = zaehler1 ; ry = zahl1d18_2; multiply(); zaehler1 = rz;
        rx = nenner; ry = zwei; add_sub("a"); nenner = rz;
        // pout=sum1 ; print_zahl_s("Summe \n");
        rx= sum1; ry = piold; add_sub("s");
        if(rz.ex > arctanstellen) break ;
        piold = sum1;
      }
      // pruefe auf groesser 1 , wenn ja, dann pi/2- arctan(1/x)
      if(grossflag==1){
       eulerz = sum1;
       pigauss(); rx=rz; ry=zwei; divide();
       rx=rz; ry=eulerz; add_sub("s"); sum1=rz;
      }
    } else {
       // berechne Reihe nach arcsin
       // arctan(x) = arcsin(x/sqrt(1+x^2)
       printf("berechne arctan ueber arcsin\n");
       rx=zzz;ry=zzz; multiply(); rx=eins; ry=rz; add_sub("a");
       rx=rz; wurzel(); rx=zzz; ry=rz; divide() ;rz.sm=0;
       // arctan > 1 : arctan(x) = pi/2 - arctan(1/|x|)
       // pout=rz; print_zahl_s("zum arctan ueber arcsin direkt\n");
       // arcsin Reihe
       zaehler1 = rz; zaehler1.sm=0;
       zaehler2 = eins; zaehler2.sm=1;  // -1
       x0 = null0; sum2=eins;  sum1=null0;// fehlt noch
       rx=rz; ry = rz ; multiply(); zahl1d18_2 = rz;
       nenner = eins ; i=0; piold=null0;
       for(;;)
       {
         i++;
         // printf(" %d",i);
         /*  Taylorreihe */
         rx = zaehler1; ry=nenner; divide();
         rx = sum1; ry=rz; add_sub("a"); sum1 = rz;
         // berechne neuen Zaehler
         /* 1*x^3, 1*3*x^5, 1*3*5*x^7...*/
         rx = zaehler1 ; ry = zahl1d18_2; multiply(); zaehler1 = rz;
         rx = zaehler2 ; ry = zwei; add_sub("a"); zaehler2 = rz;
         rx = rz; ry = zaehler1; multiply(); zaehler1 = rz;
         // pout=zaehler1 ; print_zahl_s("Zaehler1 \n");
         // 0, 2, 4, 6,
         rx=x0; ry=zwei; add_sub("a"); x0=rz;
         rx=rz; ry=sum2; multiply(); sum2=rz;
         rx=zaehler2; ry=zwei; add_sub("a");
         rx=rz ; ry=sum2; multiply();nenner=rz;
         // pout=sum1 ; print_zahl_s("Summe \n");
         rx= sum1; ry = piold; add_sub("s");
         if(rz.ex > arcsinstellen) break ;
         piold = sum1;
   }
  }
    // korrigiere Exponenten
    if(minus==1) sum1.sm=1;
    rx=sum1;
    pout=sum1; print_zahl_s("arctan(x):x");
    printf("arctan: Zahl der Iterationen =%d , Zahl der Stellen =%d\n",
        i, arctanstellen);
  }
/***********************************************/
/* Bercehnung von sin und cos nach der Reihe
    sin(x) = x - x^3/3! + x^5/5! - x^7/7!...
    cos(x) = 1 - x^2/2! + x^4/4! - x^6/6!...
    arctan= x - x^3/3 + x^5/5 -+ ...
*/
 void sinus()
 {
  int i, minus, exsum, mal10, sincosstellen;
  printf("Berechne sinus/cos nach Reihe \n");
  sincosstellen = 100;
 // Beispielzahl
   // 0.5
    rx=null0; rx.ma[1]=1;rx.ma[2]=1; rx.se=1; rx.ex=1;rx.sm=1;
    //rx=null0; rx.ma[1]=9;rx.ma[2]=0;rx.ma[3]=0; rx.se=0; rx.ex=1;rx.sm=1;
   // rx=eins;
   // -0.5
   // 610000
   // rx=null0; rx.ma[1]=6;rx.ma[2]=1; rx.se=0; rx.ex=4;rx.sm=0;
 // Ende Beispielzahl
    zzz=rx;
 // 0.99
   // rx=null0; rx.ma[1]=9;rx.ma[2]=9; rx.se=1; rx.ex=1;rx.sm=0;
    pout=rx ; print_zahl_s("Zahl:");
   // Ende Beispielzahl
   // Abfrage auf negativ
   if(rx.sm==1) minus=1;
   // Abfrage auf 0
   if(rx.ma[1] == 0) {
      rz=null0;
      pout=rz ; print_zahl_s("sin(0):");
      return;
   }
   // öberprÅfe auf Vielfaches von 2 pi
   pigauss();
   rx=rz;
   ry=zwei; multiply(); sum1=rz;
   rx=zzz; rx.sm=0; ry=rz; divide();
   // pout=rz ; print_zahl_s("Zwischenergebnis:"); // gggg
   // abschneiden
   // Konvertiere float in integer, neue Zahl in ry
   if(rz.se==0){
      rx=rz;
      ry= null0;
        for(i=1; i <= 1 + rx.ex; i++) {
         ry.ma[i] = rx.ma[i];
       }
      // multipliziere abgeschnittene Zahl mit 2 PI
      //pout=ry; print_zahl_s("abgeschnitten:");
      ry.ex=rx.ex; ry.sm=rx.sm; rx=sum1; multiply();
      // pout=rz; print_zahl_s("nach mult:");
      // pout=zzz; print_zahl_s("zzz:");
      rx=zzz; rx.sm=0; ry=rz; add_sub("s"); zzz= rz;
      //pout=rz; print_zahl_s("nach Korrektur 1:");
   }
      rz=zzz;
      //ry.ex = rz.ex; ry.sm=0; rx.sm=0; zahl1d57 = ry;
      // pout=ry; print_zahl_s("nachher x :");
      // 2. Schritt : subtrahiere die beiden Zahlen (Betrag)
      //add_sub("s"); zaehler3 = rz;
      // pout=rz; print_zahl_s("nachher x-y :");
      // ist die Zahl eine ganze Zahl, dann korrigiere Exponent und fertig
     // return;
   // Abfrage auf 1
   // rx=eins;ry=zzz; ry.sm=0; add_sub("s");
   // if(rz.ma[1]==0){
   // pigauss(); rx=rz; ry=null0; ry.ma[1]=4; divide();
   // if(minus==1) rz.sm=1;
   // pout=rz; print_zahl_s("sin(1) = pi/4");
   // return;
   // }
      eins=x0; eins.ma[1]=1;
      zaehler1 = zzz;
      rx=zzz; ry = zzz ; multiply(); zahl1d18_2 = rz;
      nenner = eins; i=0; piold=null0; xxx=eins; sum1=null0;
      for(;;)
      {
        i++;
        printf(" %d",i);
        /*  Taylorreihe */
        rx = zaehler1; ry=nenner; divide();
        if( (i % 2) != 0) {
           rx = sum1; ry=rz; add_sub("a"); sum1 = rz;
        } else {
           rx = sum1; ry=rz; add_sub("s"); sum1 = rz;
        }
        /* x^3, x^5, ...*/
        rx = zaehler1 ; ry = zahl1d18_2; multiply(); zaehler1 = rz;
        // 1! 2! 3! 4!
        rx = eins; ry= xxx; add_sub("a"); xxx=rz;
        rx=rz ; ry= nenner; multiply(); nenner=rz;
        rx = eins; ry= xxx; add_sub("a"); xxx=rz;
        rx=rz ; ry= nenner; multiply(); nenner=rz;
        // rx = nenner; ry = zwei; add_sub("a"); nenner = rz;
        // pout=sum1 ; print_zahl_s("Summe \n");
        rx= sum1; ry = piold; add_sub("s");
        if(rz.ex > sincosstellen) break ;
        piold = sum1;
      }
      // pout=sum1 ; print_zahl_s("sinus x \n");
   // korrigiere Exponenten
    if(minus==1) sum1.sm=1;
    rx=sum1;
    pout=sum1; print_zahl_s("sin(x):x");
    printf("sin: Zahl der Iterationen =%d , Zahl der Stellen =%d\n",
        i, sincosstellen);

 }
 /***********************************************/
/* Bercehnung von sin und cos nach der Reihe
    sin(x) = x - x^3/3! + x^5/5! - x^7/7!...
    cos(x) = 1 - x^2/2! + x^4/4! - x^6/6!...
    arctan= x - x^3/3 + x^5/5 -+ ...
*/
 void cosinus()
 {
  int i, minus, sincosstellen;
  printf("Berechne cos nach Reihe \n");
  sincosstellen = 100;
 // Beispielzahl
   // 0.5
    rx=null0; rx.ma[1]=5;rx.ma[2]=1; rx.se=0; rx.ex=2;// rx.sm=1;
   //rx=null0; rx.ma[1]=9;rx.ma[2]=0;rx.ma[3]=0; rx.se=0; rx.ex=1;rx.sm=1;
   // rx=eins;
   // -0.5
   // 610000
   // rx=null0; rx.ma[1]=6;rx.ma[2]=1; rx.se=0; rx.ex=4;rx.sm=0;
   // Ende Beispielzahl
    zzz=rx;
 // 0.99
   // rx=null0; rx.ma[1]=9;rx.ma[2]=9; rx.se=1; rx.ex=1;rx.sm=0;
    pout=rx ; print_zahl_s("Zahl:");
   // Ende Beispielzahl
   // Abfrage auf negativ
   if(rx.sm==1) minus=1;
   // Abfrage auf 0
   if(rx.ma[1] == 0) {
      rz=eins;
      pout=rz ; print_zahl_s("cos(0)=1");
      return;
   }
   // öberprÅfe auf Vielfaches von 2 pi
   pigauss();
   rx=rz;
   ry=zwei; multiply(); sum1=rz;
   rx=zzz; rx.sm=0; ry=rz; divide();
   // pout=rz ; print_zahl_s("Zwischenergebnis:"); // gggg
   // abschneiden
   // Konvertiere float in integer, neue Zahl in ry
   if(rz.se==0){
      rx=rz;
      ry= null0;
        for(i=1; i <= 1 + rx.ex; i++) {
         ry.ma[i] = rx.ma[i];
       }
      // multipliziere abgeschnittene Zahl mit 2 PI
      //pout=ry; print_zahl_s("abgeschnitten:");
      ry.ex=rx.ex; ry.sm=rx.sm; rx=sum1; multiply();
      // pout=rz; print_zahl_s("nach mult:");
      // pout=zzz; print_zahl_s("zzz:");
      rx=zzz; rx.sm=0; ry=rz; add_sub("s"); zzz= rz;
      //pout=rz; print_zahl_s("nach Korrektur 1:");
   }
      // cos(x) = 1 - x^2/2! + x^4/4! - x^6/6!...
      rz=zzz;
      eins=null0; eins.ma[1]=1;
      zaehler1 = eins;
      rx=zzz; ry = zzz ; multiply(); zahl1d18_2 = rz;
      nenner = eins; i=0; piold=null0; xxx=null0; sum1=null0;
      for(;;)
      {
        i++;
        printf(" %d",i);
        /*  Taylorreihe */
        rx = zaehler1; ry=nenner; divide();
        if( (i % 2) != 0) {
           rx = sum1; ry=rz; add_sub("a"); sum1 = rz;
        } else {
           rx = sum1; ry=rz; add_sub("s"); sum1 = rz;
        }
        /* x^2, x^4, ...*/
        rx = zaehler1 ; ry = zahl1d18_2; multiply(); zaehler1 = rz;
        // 1! 2! 3! 4!
        rx = eins; ry= xxx; add_sub("a"); xxx=rz;
        rx=rz ; ry= nenner; multiply(); nenner=rz;
        rx = eins; ry= xxx; add_sub("a"); xxx=rz;
        rx=rz ; ry= nenner; multiply(); nenner=rz;
        // rx = nenner; ry = zwei; add_sub("a"); nenner = rz;
        // pout=sum1 ; print_zahl_s("Summe \n");
        rx= sum1; ry = piold; add_sub("s");
        if(rz.ex > sincosstellen) break ;
        piold = sum1;
      }
      // pout=sum1 ; print_zahl_s("sinus x \n");
   // korrigiere Exponenten
    if(minus==1) sum1.sm=1;
    rx=sum1;
    pout=sum1; print_zahl_s("cos(x):x");
    printf("cos: Zahl der Iterationen =%d , Zahl der Stellen =%d\n",
        i, sincosstellen);
 }

/**********************************************/
/* Berechnung von PI nach GAUSS  */
/* PI = 48 arctan(1/18) + 32 arctan(1/57) - 20 arctan(1/239)
   arctan= x - x^3/3 + x^5/5 -+...(-1)^(k-1)/(2k-1) + (-1)^k/(2k+1) */
// mit laenge=750, n=730 , maxexp=200 ,i=300 stimmt PI auf 700
// Stellen
 void pigauss()
 {
  int i, pistellen ;
  pistellen = 100;
 printf("Berechne PI nach arctan Reihe Gauss, Zahl der Stellen: %d\n", pistellen);
  /* definiere die Konstanten */
  /* 1/18 , 1/57, 1/239 */
  x0 = null0;
  eins = x0; eins.ma[1]=1;
  ry=x0; ry.ma[1]=1; ry.ma[2]=8; ry.ex=1; /* 18 */
  rx=eins; divide(); zahl1d18 = rz;
  ry=x0; ry.ma[1] = 5; ry.ma[2]=7; ry.ex=1; /* 57 */
  rx=eins ; divide(); zahl1d57 = rz;
  ry=x0; ry.ma[1] = 2; ry.ma[2] = 3;
  ry.ma[3]=9; ry.ex = 2; /* 239 */
  rx=eins ; divide(); zahl1d239 = rz;
  /* quadrate von 1/18, 1/57, 1/239 */
  rx = zahl1d18 ; ry = zahl1d18 ; multiply(); zahl1d18_2 = rz;
  rx = zahl1d57 ; ry = zahl1d57 ; multiply(); zahl1d57_2 = rz;
  rx = zahl1d239 ; ry = zahl1d239 ; multiply(); zahl1d239_2 = rz;
  zwei = x0; zwei.ma[1] = 2;
  eins = x0; eins.ma[1] = 1;
  sum1 = x0; sum2 = x0; sum3 = x0, piold = x0;
  nenner = eins ;
  zaehler1 = zahl1d18; zaehler2 = zahl1d57; zaehler3 = zahl1d239;
  /* Genauigkeit bis 1000 Stellen, wenn bis 400 Glieder */
  // for(i=1; i<=100; i++)
  i=0;
  for(;;)
  {
    i++ ;
    // printf(" %d",i);
    /* 1. Taylorreihe */
    rx = zaehler1; ry=nenner; divide();
    if( (i % 2) != 0) {
      rx = sum1; ry=rz; add_sub("a"); sum1 = rz;
    } else {
      rx = sum1; ry=rz; add_sub("s"); sum1 = rz;
    }
    /* x^3,x^5,...*/
    rx = zaehler1 ; ry = zahl1d18_2; multiply(); zaehler1 = rz;
     /* 2. Taylorreihe */
    rx = zaehler2; ry=nenner; divide();
    if( (i % 2 )!= 0) {
      rx = sum2; ry=rz; add_sub("a"); sum2 = rz;
    } else {
      rx = sum2; ry=rz; add_sub("s"); sum2 = rz;
    }
    /* x^3,x^5,...*/
    rx = zaehler2 ; ry = zahl1d57_2; multiply(); zaehler2 = rz;
     /* 3. Taylorreihe */
    rx = zaehler3; ry=nenner; divide();
    if( (i % 2) != 0) {
      rx = sum3; ry=rz; add_sub("a"); sum3 = rz;
      // pout=rz ; print_zahl_s("Taylorglied + :\n");
    } else {
      rx = sum3; ry=rz; add_sub("s"); sum3 = rz;
      //pout=rz ; print_zahl_s("Taylorglied - :\n");
    }
    /* x^3,x^5,...*/
    rx = zaehler3 ; ry = zahl1d239_2; multiply(); zaehler3 = rz;
     //pout=rz ; print_zahl_s("Zaehler 3 \n");
    rx = nenner; ry = zwei; add_sub("a"); nenner = rz;
 // } /* end for */
 ry = x0 ; ry.ma[1] = 4; ry.ma[2] = 8; ry.ex = 1; /* 48 */
 rx = sum1 ; multiply(); xx1= rz; //sum1 = rz;
 // pout = rz; print_zahl_s("sum1 :");

 ry = x0 ; ry.ma[1] = 3; ry.ma[2] = 2; ry.ex = 1; /* 32 */
 rx = sum2 ; multiply(); xx2=rz;// sum2 = rz;
 // pout = rz; print_zahl_s("sum2:");

 ry = x0 ; ry.ma[1] = 2; ry.ma[2] = 0; ry.ex = 1; /* 20 */
 rx = sum3 ; multiply(); xx3=rz; // sum3 = rz;
 // pout = rz; print_zahl_s("sum3 :");

 rx= xx1; ry=xx2; add_sub("a"); rx = rz; ry = xx3; add_sub("s");
 // rx= sum1; ry=sum2; add_sub("a"); rx = rz; ry = sum3; add_sub("s");
 // pout = rz; printf("\n"); print_zahl_s("PI :");
 xxx= rz;
 rx=rz; ry=piold; add_sub("s"); //   pout= rz; print_zahl_s("Unterschied");
 if(rz.ex > pistellen) break ;
 piold = xxx;
 }
 // pout = xxx; printf("\n"); print_zahl_s("PI :");
 rz=xxx;
 }
/**********************************************/
/* Berechnung der Zahl e nach einer Reihe
  2 + 1/ 2! + 1/ 3! + 1/ 4! ............   */
 void eulerzahl()
{
  int i;
  /* definiere 1 */
 printf("Berechne e nach Reihe \n");
  x0 = null0;
  eins = x0; eins.ma[1]=1;
  eulerz = eins;
  xx1 = eins ;
  xx2 = null0 ;
  piold = null0;
  for (i=1; i<= 1000; i++) {
     rx= xx2; ry = eins; add_sub("a"); xx2 = rz;
     rx= xx1; ry=xx2; multiply(); xx1 = rz;
     // pout=rz; print_zahl_s("xx1:");
     rx=eins; ry=rz; divide();
     rx=rz; ry=eulerz; add_sub("a"); eulerz = rz;
     // printf("i= %d\n", i);
     // pout=rz; print_zahl_s("euler:");
     rx= rz; ry = piold; add_sub("s");
     if(rz.ex > 230) break ;
     // pout = rz; print_zahl_s("diff;");
     piold = eulerz;
  }
  printf("i= %d\n",i);
  pout=eulerz; print_zahl_s("euler:");
}
 /**********************************************/
/* Berechnung der Zahl e nach einer Reihe
  1 + x/1! + x^2/2! + x^3/3! + x^4/4! ............   */
 void ehochx()
{
  int i, ehochxstellen;
  ehochxstellen = 100;
  /* definiere 1 */
  printf("Berechne e hoch x nach Reihe\n");
  // Beispiel : x = 60.0
  // rx = null0; rx.ma[1]=1; rx.ma[2]=0;  rx.ex=0; rx.se= 0;
  // pout=rx; print_zahl_s("Zahl:");

  xxx= rx;
  x0 = null0;
  eins = x0; eins.ma[1]=1;

  zaehler1 = eins;
  eulerz = eins;
  xx1 = eins ;
  xx2 = null0 ;
  piold = null0;  i=0;
  // for (i=1; i<= 30; i++) {
   for (;;) {
     i++;
     // printf(" %d",i);
     rx= xx2; ry = eins; add_sub("a"); xx2 = rz;    // 1,2,3,4
     rx= xx1; ry=xx2; multiply(); xx1 = rz;         // 1, 1*2, 1*2*3,...
     rx= zaehler1; ry=xxx; multiply(); zaehler1= rz;    // x,x^2,x^3
     // pout=rz; print_zahl_s("xx1:");
     rx=zaehler1; ry=xx1; divide();
     rx=rz; ry=eulerz; add_sub("a"); eulerz = rz;
     // printf("i= %d\n", i);
     // pout=rz; print_zahl_s("euler:");
     rx= rz; ry = piold; add_sub("s");
     if(rz.ex > 200) break ;
     // pout = rz; print_zahl_s("diff;");
     piold = eulerz;
  }
  printf("Zahl der Iterationen : %d\n",i);
  pout=eulerz; print_zahl_s("e hoch x:");
 }
/**********************************************/
/* Berechnung den natÅrlichen Logarithmus nach einer Reihe
 uv = (u-v)/(u+v)
 ln(u/v) = 2 * ( uv + uv^3 /3 + uv^5/5 + ....)
 Konvergiert nur fuer klein u schnell
 v = 1 u = 0.3 .. 3.3
 fÅr 1<= x < 10 gilt :
 ln(x*10^r) = ln(x/sqrt(10)) + (r +0.5) * ln10
 lg(x*10^r) = ln(x/sqrt(10))/ln(10) + (r +0.5)
 1/ln(10) = lg(e) = 0.4343...
   Zahl der Iterationen von ln(x/sqrt(10))
            bei 100 Stellen Genauigkeit :
       log 9.9 ..171
       log 9 ... 154
       log 8 ... 135
       log 7 ... 116
       log 6 ...  95
       log 5 ...  77
       log 4 ...  54
       log 3 ...  32   3.2 ...  23  3.3 ...  30  3.5 ... 39
       log 2 ...  77   2.5 ...  54
       log 1.5 .. 110  1.1 ... 156
       log 1 ...  172
    log(1000) dauert 5 Minuten fuer 400 Stellen auf P166  */
 void logarithmusn ()
{
 int i, ll, kk;
 int logflag, breakflag;
 int lnstellen;
 char econ[20],econ1[20];
 // ln ..1 , lg ..0
 logflag = 1;
 lnstellen = 100;
 eins = null0; eins.ma[1]=1;
 zwei = null0; zwei.ma[1]=2;
 printf("Berechne den Natuerlichen Log oder Briggschen Log :\n");
 if(logflag) { printf("Natuerlicher Log von :");
 } else { printf("Briggscher Log von :");}
 // rx ist eine beliebige Zahl
 // x muss  sein 1 <= x < 10
 // Beispiel : x = 60.0
 // rx = null0; rx.ma[1]=1; rx.ma[2]=0;  rx.ex=1; rx.se= 1;
 pout=rx; print_zahl_s("rx=");
 // Abfrage auf log(1) = 0
 breakflag=0;
 if(rx.ma[1] == 1 && rx.ex == 0) {
   for(i=laenge-1; i>1; i--) {
     if(rx.ma[i] != 0 ){ breakflag=1; break;}
   }
   if(breakflag==0) {
     rz=null0;
     pout=rz; print_zahl_s("log1:");
     return;
   }
 }
 // Konvertiere Exponent in Zahl in x0
 sprintf(econ,"%d",rx.ex);
 strcpy(econ1,"");
 strcat(econ1,econ);
 ll= strlen(econ1);
 // printf("ll= %d string: %s\n",ll,econ1);
 x0=null0;
 kk=1;
 for (i=0; i <=ll-1; i++) {
   x0.ma[kk] = econ[i]- 48;
   kk++;
 }
 x0.ex=ll-1; x0.se=0;
 if(rx.se==1) x0.sm=1;
 // pout=x0; print_zahl_s("x0");
 // naechste Zeile Im programm von Martin loeschen
 // x0=null0; x0.ma[1]= rx.ex; x0.sm = rx.se ;
 // die Mantisse der Zahl ist immer 1.0 bis 9.9999
 // entferne den Exponenten
 zzz = rx; zzz.ex=0; zzz.se=0;
  // berechne Wurzel 10
 rx = null0; rx.ma[1]=1; rx.ex=1; wurzel();
 // dividiere Zahl durch Wurzel10
 rx=zzz; ry=rz; divide(); xx1=rz;
 //
 printf("Berechne Reihe fuer x/sqrt(10) :\n");
 // pout=rz; print_zahl_s("nach wurzel");
 rx = xx1; ry = eins; add_sub("s"); xx2 = rz; // u-v
 // pout=rz; print_zahl_s("u - v"); return;
 rx = xx1; ry = eins; add_sub("a");           // u+v
 rx = xx2; ry = rz; divide(); xxx= rz;        // (u-v)/(u+v) = uv
 // pout=rz; print_zahl_s("uv"); return;
 rx = xxx; ry = xxx; multiply(); xx2 = rz;    // uv^2
 xx1 = eins;
 piold = null0;
 // for (i=1; i<=30 ; i++) {
 i=0;
 // ca 1 min fuer 300 Stellen
 for(;;) {
    i++;
    if(i==1) {sum1 = xxx; continue;}         // uv
    rx=xx1; ry = zwei; add_sub("a"); xx1=rz; // 3,5,7,
    // pout=rz; print_zahl_s("3,5,7");
    rx = xxx; ry = xx2; multiply(); xxx = rz; // uv^3, uv^5,..
    // pout=rz; print_zahl_s("uv3,uv5,uv7");
    rx=rz; ry=xx1; divide();
    rx = sum1; ry=rz; add_sub("a"); sum1 = rz;
    // pout=sum1; print_zahl_s("xxx ln:");
    rx= rz; ry = piold; add_sub("s");
    if(rz.ex > lnstellen) break ;
    // pout = rz; print_zahl_s("diff;");
    piold = sum1;
 }
 rx=zwei; ry = sum1; multiply();
 printf("ln(x/sqrt(10)): Zahl der Iterationen =%d , Zahl der Stellen =%d\n",
        i, lnstellen);
 // pout=rz; print_zahl_s("ln InputZahl/Wurzel10:");
 sum3 = rz;
 // addiere Korrektur
 // in x0 steht der Exponent der eingegebenen zahl
 rx=x0;
 ry=null5; add_sub("a"); x0= rz;
  // pout=rz; print_zahl_s("0.5+r:");
 ln10();
 // pout=rz; print_zahl_s("ln10 das 2 . mal:");
 if(logflag==1) {
    rx= rz; ry=x0; multiply();
    // pout=rz; print_zahl_s("(0.5+r )* ln10:");
    rx=rz; ry=sum3; add_sub("a");
    pout=rz; print_zahl_s("Natuerlicher Logarithmus von x :");
 } else {
    rx= sum3; ry=rz; divide();
    rx=rz; ry=x0; add_sub("a");
    pout=rz; print_zahl_s("Briggscher Logarithmus von x :");
 }
}
/**********************************************/
/* Berechne 10^x = 1 +(x*ln10)/1 +(x*ln10)^2/2
  + (x*ln10)^3/6 +....
  Fuer Betrag x < 1 wird der Wert direkt nach der
  Reihe berechnet, fÅr groessere Werte wird die
  Gr˜sste ganze Zahl abgetrennt. Ganzzahlige Werte
  werden nicht mit der Reihe berechnet   */
 void zehnhochx()
 {
  int i, exsum, mal10, lnstellen, kleiner1flag;
  lnstellen = 100;
  kleiner1flag=1;
  printf("Berechne zehn hoch x :\n");
   // Beispielzahl
   // rx = null0;
   // 5.14
    // rx= null0; rx.ma[1]=5; rx.ma[2]=1; rx.ma[3]=4 ;
    // rx.ma[4]=0; rx.ex=0; rx.se=0; rx.sm=0;
   // -5.14
   // rx= null0; rx.ma[1]=5; rx.ma[2]=1; rx.ma[3]=4 ;
   // rx.ma[4]=0; rx.ex=0; rx.se=0; rx.sm=1;
       // -0.514
   // rx= null0; rx.ma[1]=5; rx.ma[2]=1; rx.ma[3]=4 ;
   // rx.ma[4]=0; rx.ex=1; rx.se=1; rx.sm=1;
       // 0.514
    //rx= null0; rx.ma[1]=5; rx.ma[2]=1; rx.ma[3]=4 ;
    //rx.ma[4]=0; rx.ex=1; rx.se=1; rx.sm=0;
      // 0.1
    // rx= null0; rx.ma[1]=1; rx.ma[2]=0; rx.ma[3]=0 ;
    // rx.ma[4]=0; rx.ex=1; rx.se=1; rx.sm=0;
      // -0.1
    //rx= null0; rx.ma[1]=1; rx.ma[2]=0; rx.ma[3]=0 ;
    //rx.ma[4]=0; rx.ex=1; rx.se=1; rx.sm=1;
      // 100
     rx= null0; rx.ma[1]=1; rx.ma[2]=0; rx.ma[3]=0 ;
     rx.ma[4]=1; rx.ex=2; rx.se=1; rx.sm=1;

   // Ende Beispielzahl
   // Pruefe auf 0 : 10^0 = 1
  pout=rx; print_zahl_s("Eingabe x :");
  if( rx.ma[1]==0){
    rz=rx; rz.ma[1]=1;
    pout=rz; print_zahl_s("10^x :"); return;
  }
  zaehler3 = rx;
  zahl1d18 = rx;
  // Wenn Betrag rx < 1, dann gehe direkt zur 10^x Berechnung
  // und Åberspringe nÑchsten Teil
  if(rx.se == 0 ) {
      // 1. Schritt : zerlege die Zahl in die gr˜sste Integerzahl
      // + Rest
      // pout=rx; print_zahl_s("vorher x :");
      // Konvertiere float in integer
      kleiner1flag=0;
      ry= null0;
      exsum=0; mal10 = pow(10,rx.ex);
        for(i=1; i <= 1 + rx.ex; i++) {
         // printf("mal10=%d\n",mal10);
         ry.ma[i] = rx.ma[i];
         exsum = exsum + mal10 * rx.ma[i];
         mal10=mal10/10;
       }
      printf("zahl: %d\n",exsum);
      ry.ex = rx.ex; ry.sm=0; rx.sm=0; zahl1d57 = ry;
      // pout=ry; print_zahl_s("nachher x :");
      // 2. Schritt : subtrahiere die beiden Zahlen (Betrag)
      add_sub("s"); zaehler3 = rz;
      // pout=rz; print_zahl_s("nachher x-y :");
      // ist die Zahl eine ganze Zahl, dann korrigiere Exponent und fertig
        if(rz.ma[1]==0) {
              rz=null0; rz.ma[1] = 1; rz.se= zahl1d18.sm; rz.ex=exsum;
              pout=rz; print_zahl_s("10^x ganz:");
              return;
        }
    }
  // 3. Schritt : berechne Reihe
  ln10();
  //  pout=rz; print_zahl_s("Zehn hoch x :"); return;
  rx= zaehler3; ry=rz; multiply(); x0 = rz;  // x*ln10
  // pout=rz; print_zahl_s("Zehn hoch x :"); return;
  sum1 = eins;  xx1= eins; xx2=null0;
  nenner=eins; zaehler1 = eins;
  piold = null0;
  i=0;
  for(;;) {
   i++;
   rx = zaehler1; ry = x0; multiply(); zaehler1 = rz;
   rx= xx2; ry=eins; add_sub("a"); xx2 = rz; // 1,2,3,4,5
    //   pout=rz; print_zahl_s("Zehn hoch x :");
   rx=nenner; ry = rz; multiply(); nenner = rz; // 1,1*2, 1*2*3, 1*2*3*4
   rx=zaehler1; ry=nenner; divide();
   rx=sum1; ry=rz; add_sub("a"); sum1 = rz;
   // pout=rz; print_zahl_s("Zehn hoch x :");
   rx= rz; ry = piold; add_sub("s");
    if(rz.ex > lnstellen) break ;
    // pout = rz; print_zahl_s("diff;");
    piold = sum1;
  }
   printf("10^x: Zahl der Iterationen =%d , Zahl der Stellen =%d\n" ,
       i, lnstellen);
   // Wenn Betrag < 1, dann keine Korrektur
   if(kleiner1flag==1) {
      pout = sum1; print_zahl_s("10^x kleiner 1");
      rz=sum1; return;
   }
   // korrigiere Exponent : addiere abgespaltenen Wert dazu
   if(zahl1d18.sm==1) {
     // wenn Zahl neg, dann reziprok
     rx=null0; rx.ma[1]=1; ry=sum1; divide(); sum1=rz;
     sum1.ex = sum1.ex + exsum;
     pout = sum1; print_zahl_s("10^x korrigiert reziprok");
   } else {
     sum1.ex = sum1.ex + exsum;
     pout = sum1; print_zahl_s("10^x korrigiert normal");
   }
   rz = sum1;
 }
/**********************************************/
/* Berechnung den natÅrlichen Logarithmus ln10 einer Reihe
  u = 9/11 ; ln(10)=2(u + u^3/3 + u^5/5 ....)
  1/ln(10) = lg(e) ,,wird benoetigt fuer die Umrechnung
  konvergiert zu langsam, berechne ln(2*2*2*1.25)
   = 3 * ln(2) + ln(1.25)
  ln10 steht in rz */
 void ln10()
{
 int i;
 int ln10stellen;
 ln10stellen = 100;
 printf("Berechne den Natuerlichen Logarithmus 10:\n");
 eins = null0; eins.ma[1]=1;
 zwei = null0; zwei.ma[1]=2;
 rx = null0; rx.ma[1] = 1; // 1
 ry = null0; ry.ma[1] = 3; // 3
 divide(); xxx= rz;        // 9/11
 rx = rz; ry = rz; multiply(); xx2 = rz;    // (9/11)^2

 // pout= rz; print_zahl_s("uv quad:");
 // return;
 xx1 = eins;
 piold = null0;
 // for (i=1; i<=30 ; i++) {
 i=0;
  for(;;) {
    i++;
    if(i==1) {sum1 = xxx; continue;}           // uv
    rx=xx1; ry = zwei; add_sub("a"); xx1=rz; // 3,5,7,
    // pout=rz; print_zahl_s("3,5,7");
    rx = xxx; ry = xx2; multiply(); xxx = rz; // uv^3, uv^5,..
    // pout=rz; print_zahl_s("uv3,uv5,uv7");
    rx=rz; ry=xx1; divide();
    rx = sum1; ry=rz; add_sub("a"); sum1 = rz;
    // pout=sum1; print_zahl_s("xxx ln:");
    rx= rz; ry = piold; add_sub("s");
    if(rz.ex > ln10stellen) break ;
    // pout = rz; print_zahl_s("diff;");
    piold = sum1;
 }
 rx=zwei; ry = sum1; multiply(); sum1 = rz;
 printf("ln2: Zahl der Iterationen =%d , Zahl der Stellen =%d\n",
        i,ln10stellen);
 // pout=rz; print_zahl_s("ln 2:");
 // ln (1.25)
 rx = null0; rx.ma[1] = 2; rx.ma[2] = 5; rx.ex=1; rx.se = 1; //  0.25
 ry = null0; ry.ma[1] = 2; ry.ma[2] = 2; ry.ma[3] = 5; //  2.25
 divide(); xxx= rz;        // 0.25/2.25
 rx = rz; ry = rz; multiply(); xx2 = rz;    // ()^2
 xx1 = eins;
 piold = null0;
 i=0;
  for(;;) {
    i++;
    if(i==1) {sum2 = xxx; continue;}           // uv
    rx=xx1; ry = zwei; add_sub("a"); xx1=rz; // 3,5,7,
    // pout=rz; print_zahl_s("3,5,7");
    rx = xxx; ry = xx2; multiply(); xxx = rz; // uv^3, uv^5,..
    // pout=rz; print_zahl_s("uv3,uv5,uv7");
    rx=rz; ry=xx1; divide();
    rx = sum2; ry=rz; add_sub("a"); sum2 = rz;
    // pout=sum1; print_zahl_s("xxx ln:");
    rx= rz; ry = piold; add_sub("s");
    if(rz.ex > ln10stellen) break ;
    // pout = rz; print_zahl_s("diff;");
    piold = sum2;
 }
 rx=zwei; ry = sum2; multiply(); sum2 = rz;
 printf("ln1.25 : Zahl der Iterationen =%d , Zahl der Stellen =%d\n",i,ln10stellen);
 // pout=rz; print_zahl_s("ln 1.25:");
 rx= sum1; ry = sum1; add_sub("a"); rx=rz; ry=sum1; add_sub("a");
 rx = rz; ry=sum2; add_sub("a");
 // pout=rz; print_zahl_s("ln 10 :");
}
/**********************************************/
/* berechne y^x = 1 + (x*ln(y))/1! + (x*ln(y))^2/2! + (x*ln(y^))^3/3!+ .. */
/* Folgendes ist zu beachten : y^0 = 1 , 0^0 = error
   y^1 = y , 1^x = 1,
   fuer y = 10 : gehe zu 10^x   y < 0 = error
   Rechenzeit : 2^100 ca 350 Iterationen   100 Stellen
                3^100    479
                4^100    564
                5^100    628
                9^100    794
   umso langsamer, je groesser x*ln(y) ist
      */
void ypsilonhochx()
{
  int i, yhochxstellen, wurzelflag;
  yhochxstellen = 100;
  // wurzelflag = 0 : y^x   =1 : xroot(y)
  wurzelflag=0;
  if(wurzelflag==0) {
     printf("Berechne y hoch x :\n");
  } else {
     printf("Berechne xroot(y) :\n");
  }
  // Beispielzahl
  ry= null0; ry.ma[1]=2; ry.ex=0; // rx.ma[2]=0; rx.ma[3]=0 ;
  rx= null0; rx.ma[1]=1; rx.ma[2]= 0; rx.ex=2;  rx.se=0; // rx.sm=1;// rx.ma[2]=0; rx.ma[3]=0 ;
  zahl1d239 = rx;
  zahl1d239_2 = ry;
  pout=rx; print_zahl_s("Eingabe x :");
  pout=ry; print_zahl_s("Eingabe y :");
    // Ende Beispielzahl

  // Pruefe auf 0 : y^0 = 1
  if( rx.ma[1]==0){
    if(wurzelflag==0){
     rz=null0; rz.ma[1]=1;
     pout=rz; print_zahl_s("y^0 = 1 :");
    } else {
     printf("0 wurzel(y) nicht definiert ");
    }
    return;
  }
  // pruefe auf 0^0
  if(rx.ma[1]==0 && ry.ma[1]==0) {
    printf("0^0 ist nicht definiert "); return;
  }
  if(ry.sm==1) {
    printf("negatives y nicht erlaubt"); return;
  }
  // das noch einbauen
  // pruefe auf 10^x , rufe andere Funktion auf
  // if(wurzelflag==0){
  //  ry=null0; ry.ma[1]=1; ry.ex=1; add_sub("s");
  //  if(rz.ma[1]== 0){
  //   rx=zahl1d239; zehnhochx(); return;
  //  }
  // }
  // pruefe auf y^1 , Sonderfall
    ry=null0; ry.ma[1]=1; ry.ex=0; add_sub("s");
   if(rz.ma[1]== 0){
     rz=zahl1d239_2;
     pout=rz; print_zahl_s("y^1 = y :");
     return;
   }
    ry=zahl1d239_2; rx=null0; rx.ma[1]=1; rx.ex=0; add_sub("s");
   if(rz.ma[1]== 0){
     rz=zahl1d239_2;
     pout=rz; print_zahl_s("1^x = 1 :");
     return;
   }
  // berechne Reihe
  if(wurzelflag==1){
    rx=null0; rx.ma[1]=1;
    // rx=eins;
    ry= zahl1d239; divide(); zahl1d239=rz; // 1/x
    //  pout=rz; print_zahl_s("xxln(y)= ");  return;
  }
  rx= zahl1d239_2;
  // berechne ln(y), flag fuer ln setzen nicht vergessen
  logarithmusn();
  pout=rz; print_zahl_s("ln(y)= ");
  rx= zahl1d239; ry=rz; multiply(); x0 = rz;  // x*ln(y)
  sum1 = eins;  xx1= eins; xx2=null0;
  nenner=eins; zaehler1 = eins;
  piold = null0;
  i=0;  int zz;
  for(;;) {
   i++;
   printf(" %d %d|",i,zz);
   rx = zaehler1; ry = x0; multiply(); zaehler1 = rz;
   // pout=rz; print_zahl_s("(x*ln(y))= ");

   rx= xx2; ry=eins; add_sub("a"); xx2 = rz; // 1,2,3,4,5
   rx=nenner; ry = rz; multiply(); nenner = rz; // 1,1*2, 1*2*3, 1*2*3*4
   rx=zaehler1; ry=nenner; divide();
   pout=rz; print_zahl_s("z/n= ");

   rx=sum1; ry=rz; add_sub("a"); sum1 = rz;

   rx= rz; ry = piold; add_sub("s");
   zz=rz.ex;
      // pout=rz; print_zahl_s("(x*ln(y))= ");

   if(rz.ex > yhochxstellen) break ;
   piold = sum1;
  }
   printf("y^x: Zahl der Iterationen =%d , Zahl der Stellen =%d\n" ,
      i, yhochxstellen);
   rz=sum1;
   pout = sum1; print_zahl_s("y^x:");
}
/***********************************************/
/* berechne PI nach gauss _AGM :
 */
 /*wortform zwei, eins, zahl1d18, zahl1d57, zahl1d239,
sum1, sum2, sum3, zahl1d57_2, zahl1d18_2, zahl1d239_2,
nenner, zaehler1, zaehler2, zaehler3, x0,
xx1, xx2, xx3, xxx, piold ; */

void pigaussagm ()
{
 int i;
 printf("\n Berechne Pi nach AGM \n");
 printf(" definiere Anfangsbedingungen\n");
 // definiere Anfangsbedingung
 eins=null0; zwei=null0;
 eins.ma[1]=1; zwei.ma[1]=2;
 zahl1d18=eins; // a =1
 rx=zwei; wurzelk1(); rx=eins; ry=rz; divide(); zahl1d57 = rz; //b =1/sqrt(2)
 // pout=rz; print_zahl_s("b= ");

 zahl1d239.ma[1]=5; zahl1d239.ex=1; zahl1d239.se=1; // s = 0.5
 // pout=zahl1d239; print_zahl_s("b= ");

 zahl1d239_2= null0; // t =0
 xx1=eins; xx2=zwei; piold = null0;
 // Iteration
 for (i=1; i<=10; i++) {
  rx=xx1; ry=xx2; multiply(); xx1=rz;  // 2,4,8,16,..
  printf(" %d",i);
  sum1= zahl1d18; // y = a
  rx=zahl1d18; ry=zahl1d57; add_sub("a"); ry=zwei; // a=(a+b)/2
  rx=rz; divide(); zahl1d18=rz;
  // pout=zahl1d18; print_zahl_s("(a+b)/2= ");
  rx=zahl1d57; ry= sum1; multiply(); rx=rz; wurzelk(); zahl1d57= rz; // b = sqrt(b*y)
  // ca. 0.847 pout=zahl1d57; print_zahl_s("sqrt(b*y)= ");
  rx= zahl1d18; ry=sum1; add_sub("s"); rx=rz; ry=rz;
  multiply(); rx=rz; ry=xx1; multiply(); zahl1d239_2 = rz; // t = (a-y)(a-y)
  // t=t *2 exp i
  // pout=zahl1d239_2; print_zahl_s("a-y*a-y= ");
  rx=zahl1d239 ; ry= zahl1d239_2;add_sub("s"); zahl1d239= rz; // s=s-t
  rx=zahl1d18; ry=zahl1d57; add_sub("a"); rx=rz; ry=rz; multiply();
  xxx=rz; rx=zwei; ry=zahl1d239; multiply();
  rx=xxx; ry=rz; divide();sum2=rz;
  rx= rz; ry = piold; add_sub("s");
  pout=rz; print_zahl_s("diff ");
  //  if(rz.ex > 450) break ;
  piold = sum2;
  }
  pout=sum2; print_zahl_s("pi_agm= ");
}
 // xxx
 void wurzelk()
 {
 int i;
 // rm = sieben8;
 // 0.8 gibt bei 10 Iterationen 780
 rm=null0;
 rm.ma[1]=8;rm.ma[2]=5; rm.ex=1; rm.se=1;
 rn = rx;
 // konvergiert quadratisch
 nenner=null0;
 for(i=1; i<=11; i++)
  {
   printf(" wu%d ",i);
   ry=rm; rx=rn; divide();
   //printf(" nd");
   // pout=rz; print_zahl_s("nach divide");
   ry=rz; rx=rm; add_sub("a");
   //printf(" nas");
   // pout=rz; print_zahl_s("nach add_sub");
   rx=rz; ry=null5; multiply(); rm=rz;
   //printf(" nm");
   //pout=rz; print_zahl_s("nach multiply");
   // sum3=rz;
   // rx=rz; ry=nenner; add_sub("s");
   // pout=rz; print_zahl_s("wurzeldiff= ");
   // nenner=sum3;
  }
  // rz=sum3;
 }
 void wurzelk1()
 {
 int i;
 // rm = sieben8;
 // 1.4 gibt bei 10 Iterationen 1100
 rm=null0;
 rm.ma[1]=1;rm.ma[2]=4; rm.ex=0; rm.se=0;
 rn = rx;
 // konvergiert quadratisch
 nenner=null0;
 for(i=1; i<=10; i++)
  {
   printf(" wu%d ",i);
   ry=rm; rx=rn; divide();
   // pout=rz; print_zahl_s("nach divide");
   ry=rz; rx=rm; add_sub("a");
   // pout=rz; print_zahl_s("nach add_sub");
   rx=rz; ry=null5; multiply(); rm=rz;
   //pout=rz; print_zahl_s("nach multiply");
   ///rx=rz; ry=nenner; add_sub("s");
   //pout=rz; print_zahl_s("wurzeldiff= ");
   //nenner=sum3;
  }
  //rz=sum3;
 }

/***********************************************/
/* berechne PI nach ramanujan :
                       unendlich
  1/pi = sqrt(8)/ 9801 * SUMME ( (4n)!/(n!)^4 ) * ( (1103+26390 * n)/ 396^4n)
                           0
 */
 /*wortform zwei, eins, zahl1d18, zahl1d57, zahl1d239,
sum1, sum2, sum3, zahl1d57_2, zahl1d18_2, zahl1d239_2,
nenner, zaehler1, zaehler2, zaehler3, x0,
xx1, xx2, xx3, xxx, piold ; */

void piramanujan ()
{
 int i,j;
 /* 1.) sqrt(8)/9801 */
 rx = null0; rx.ma[1]=8; wurzel();
 ry=null0; ry.ma[1]=9; ry.ma[2]=8; ry.ma[4]=1; ry.ex=3;
 rx=rz; divide(); xxx=rz;
 // pout=rz; print_zahl_s("xxx= ");
 /* 2.) 396^4  */
 rx=null0; rx.ma[1]=3; rx.ma[2]=9; rx.ma[3]=6; rx.ex=2;
 ry=rx; multiply(); rx=rz; ry=rz; multiply(); x0=rz;
 // pout=x0; print_zahl_s("396^4= ");
 /* 1103, 26390 */
 rx=null0; rx.ma[1]=1; rx.ma[2]=1; rx.ma[3]=0;rx.ma[4]=3; rx.ex=3;
 zahl1d57 = rx; // 1103
 rx=null0; rx.ma[1]=2; rx.ma[2]=6; rx.ma[3]=3;rx.ma[4]=9;
 rx.ma[5]=0; rx.ex=4;
 zahl1d18 = rx; // 26390
 rx=null0; rx.ma[1]=4; zahl1d239=rx; // 4

  eulerz = eins;
  xx1= null0; xx1.ma[1]=1;
  eins = xx1;
  sum3 = eins;
  nenner=eins;
  xx2 = null0 ; sum2=null0;
  piold = null0;
  sum1 = zahl1d57;   // fÅr n=0 ist erstezahl = 1103
  // for (i=1; i<= 3; i++) {
  i=0;
  for(;;) {
     i++;
     printf(" i=%d",i);
     // (396^4)^1 ,(396^4)^2 ...in nenner
     rx=nenner; ry=x0; multiply(); nenner=rz;
     // pout=rz; print_zahl_s("nenner:");
     // (n Fakultaet)^4 in xx1
     rx= xx2; ry = eins; add_sub("a"); xx2 = rz; // inc xx2 1,2,3
     rx= xx1; ry=xx2; multiply(); xx1=rz ;  // nFak
     rx= rz; ry=rz; multiply();
     rx= rz; ry=rz; multiply(); zahl1d57_2=rz;
     // pout=rz; print_zahl_s("zahl1d57_2:");

     // (4n) Fakultaet in sum3
       for (j=1; j<=4;j++){
          rx= sum2; ry = eins; add_sub("a"); sum2 = rz;
          rx= sum3; ry=sum2; multiply(); sum3 = rz;
       }
     // pout=rz; print_zahl_s("sum2:");
     // (1103 + 26390 * n) in zaehler1
     rx= zahl1d18; ry=xx2; multiply(); rx=zahl1d57;
     ry=rz; add_sub("a"); zaehler1=rz;
     // pout=rz; print_zahl_s("*n");

     // dividiere Fak
     rx=sum3; ry=zahl1d57_2; divide();
     rx=rz; ry= zaehler1;multiply();
     rx=rz; ry=nenner; divide();

     // pout=rz; print_zahl_s("4Teile");
     rx=sum1; ry=rz; add_sub("a"); sum1=rz;
     // pout=rz; print_zahl_s("summe");
     //rx=eins; ry=rz; divide();
     //rx=rz; ry=eulerz; add_sub("a"); eulerz = rz;
     rx=sum1; ry=xxx; multiply();
     rx=eins; ry=rz; divide(); zahl1d18_2=rz;

     rx= rz; ry = piold; add_sub("s");

     // pout=rz; print_zahl_s("diff ");

     if(rz.ex > 450) break ;
     piold = zahl1d18_2;

 }
    //rx=sum1; ry=xxx; multiply();
    //rx=eins; ry=rz; divide();
    pout=zahl1d18_2; rz=pout; print_zahl_s("pi");

}
/**********************************************/
 void ehochpiwu()
 /* berechne e^(pi*sqrt(n))  */
 {
  int i,k,found,ii;
  int sto[5000];
  for(i=1; i<5000; i++) {sto[i]=0;}
  xxx=null0; xxx.ma[1]= 1; xxx.ma[2]=6;xxx.ma[3]=1;
  xxx.ex=2; koef0 = xxx;   // 37,38,39
  printf("Berechne e^(pi*sqrt(n))\n");
  // printf("Berechne e \n");
  // ehochx();
  printf("Berechne pi \n");
  piramanujan();  koef2=rz;
  pout=rz; print_zahl_s("wurzel:");
  // vvvv
  koef1 = null0; koef1.ma[1]=1;
  for(i=162 ; i<=1000; i++)
    {
      printf("------- i= %d -------\n",i);
      rx=koef1; ry = koef0; add_sub("a"); koef0=rz; // 1,2,3,4,5
      rx=rz; wurzel();
      // pout=rz; print_zahl_s("wurzel:");
      rx=koef2; ry=rz; multiply();
      // pout=rz; print_zahl_s("i * wurzel:");
      rx=rz; ehochx();
      found=0;
      for (k=1; k<=n-10; k++) {
        if(found==0) {
          if(eulerz.ma[k]==9 && eulerz.ma[k+1]==9 && eulerz.ma[k+2]==9 && eulerz.ma[k+3]==9 ) {
           printf("9999 gefunden bei i=%d\n",i); found=1;
           sto[i]=1; break;
          }
        }
      }
      for(ii=1; ii<=4000; ii++) {
       if(sto[ii] !=0 ) printf("i=%d ",ii);
      }
    }
    printf("Ergebnis :\n");
    for(ii=1; ii<=4000; ii++) {
      if(sto[ii] == 1 ) printf("i=%d ",ii);
    }
 }
/**********************************************/
 void wilkinson()
{
  int i, inull;
/* Definition von Konstanten
  --------------------------   */
// Koeffizienten fÅr das Wilkinson Polynom  a0
  koef0.ma[1]=2; koef0.ma[2]=4; koef0.ma[3]=3; koef0.ma[4]=2;
  koef0.ma[5]=9; koef0.ma[6]=0; koef0.ma[7]=2; koef0.ma[8]=0;
  koef0.ma[9]=0; koef0.ma[10]=8; koef0.ma[11]=1; koef0.ma[12]=7;
  koef0.ma[13]=6; koef0.ma[14]=6; koef0.ma[15]=4; koef0.sm=0;
  koef0.se=0; koef0.ex=18;
// Koeffizienten fÅr das Winson Polynom  a1
  koef1.ma[1]=8; koef1.ma[2]=7; koef1.ma[3]=5; koef1.ma[4]=2;
  koef1.ma[5]=9; koef1.ma[6]=4; koef1.ma[7]=8; koef1.ma[8]=0;
  koef1.ma[9]=3; koef1.ma[10]=6; koef1.ma[11]=7; koef1.ma[12]=6;
  koef1.ma[13]=1; koef1.ma[14]=6;
  koef1.sm=1;
  koef1.se=0; koef1.ex=18;
// Koeffizienten fÅr das Winkelson Polynom  a2
  koef2.ma[1]=1; koef2.ma[2]=3; koef2.ma[3]=8; koef2.ma[4]=0;
  koef2.ma[5]=3; koef2.ma[6]=7; koef2.ma[7]=5; koef2.ma[8]=9;
  koef2.ma[9]=7; koef2.ma[10]=5; koef2.ma[11]=3; koef2.ma[12]=6;
  koef2.ma[13]=4; koef2.ma[14]=0; koef2.ma[15]=7;
  koef2.ma[16]=0; koef2.ma[17]=4;
  koef2.sm=0;koef2.se=0; koef2.ex=19;
// Koeffizienten fÅr das Winkelson Polynom  a3
  koef3.ma[1]=1; koef3.ma[2]=2; koef3.ma[3]=8; koef3.ma[4]=7;
  koef3.ma[5]=0; koef3.ma[6]=9; koef3.ma[7]=3; koef3.ma[8]=1;
  koef3.ma[9]=2; koef3.ma[10]=4; koef3.ma[11]=5; koef3.ma[12]=1;
  koef3.ma[13]=5; koef3.ma[14]=0; koef3.ma[15]=9;koef3.ma[16]=8;
  koef3.ma[17]=8; koef3.ma[18]=8; koef3.ma[19]=0;koef3.ma[20]=0;
  koef3.sm=1; koef3.se=0; koef3.ex=19;
// Koeffizienten fÅr das Winkelson Polynom  a4
  koef4.ma[1]=8; koef4.ma[2]=0; koef4.ma[3]=3; koef4.ma[4]=7;
  koef4.ma[5]=8; koef4.ma[6]=1; koef4.ma[7]=1; koef4.ma[8]=8;
  koef4.ma[9]=2; koef4.ma[10]=2; koef4.ma[11]=6; koef4.ma[12]=4;
  koef4.ma[13]=5; koef4.ma[14]=0; koef4.ma[15]=5;koef4.ma[16]=1;
  koef4.ma[17]=7; koef4.ma[18]=7; koef4.ma[19]=6;koef4.ma[20]=0;
  koef4.sm=0; koef4.se=0; koef4.ex=18;
// Koeffizienten fÅr das Winkelson Polynom  a5
  koef5.ma[1]=3; koef5.ma[2]=5; koef5.ma[3]=9; koef5.ma[4]=9;
  koef5.ma[5]=9; koef5.ma[6]=7; koef5.ma[7]=9; koef5.ma[8]=5;
  koef5.ma[9]=1; koef5.ma[10]=7; koef5.ma[11]=9; koef5.ma[12]=4;
  koef5.ma[13]=7; koef5.ma[14]=6; koef5.ma[15]=0;koef5.ma[16]=7;
  koef5.ma[17]=2; koef5.ma[18]=0; koef5.ma[19]=0;koef5.ma[20]=0;
  koef5.sm=1; koef5.se=0; koef5.ex=18;
// Koeffizienten fÅr das Winkelson Polynom  a6
  koef6.ma[1]=1; koef6.ma[2]=2; koef6.ma[3]=0; koef6.ma[4]=6;
  koef6.ma[5]=6; koef6.ma[6]=4; koef6.ma[7]=7; koef6.ma[8]=8;
  koef6.ma[9]=0; koef6.ma[10]=3; koef6.ma[11]=7; koef6.ma[12]=8;
  koef6.ma[13]=0; koef6.ma[14]=3; koef6.ma[15]=7;koef6.ma[16]=3;
  koef6.ma[17]=3; koef6.ma[18]=6; koef6.ma[19]=0;koef6.ma[20]=0;
  koef6.sm=0; koef6.se=0; koef6.ex=18;
// Koeffizienten fÅr das Winkelson Polynom  a7
  koef7.ma[1]=3; koef7.ma[2]=1; koef7.ma[3]=1; koef7.ma[4]=3;
  koef7.ma[5]=3; koef7.ma[6]=3; koef7.ma[7]=6; koef7.ma[8]=4;
  koef7.ma[9]=3; koef7.ma[10]=1; koef7.ma[11]=6; koef7.ma[12]=1;
  koef7.ma[13]=3; koef7.ma[14]=9; koef7.ma[15]=0;koef7.ma[16]=6;
  koef7.ma[17]=4; koef7.ma[18]=0; koef7.ma[19]=0;koef7.ma[20]=0;
  koef7.sm=1; koef7.se=0; koef7.ex=17;
// Koeffizienten fÅr das Winkelson Polynom  a8
  koef8.ma[1]=6; koef8.ma[2]=3; koef8.ma[3]=0; koef8.ma[4]=3;
  koef8.ma[5]=0; koef8.ma[6]=8; koef8.ma[7]=1; koef8.ma[8]=2;
  koef8.ma[9]=0; koef8.ma[10]=9; koef8.ma[11]=9; koef8.ma[12]=2;
  koef8.ma[13]=9; koef8.ma[14]=4; koef8.ma[15]=8;
  koef8.ma[16]=9; koef8.ma[17]=6;
  koef8.sm=0; koef8.se=0; koef8.ex=16;
// Koeffizienten fÅr das Winkelson Polynom  a9
  koef9.ma[1]=1; koef9.ma[2]=0; koef9.ma[3]=1; koef9.ma[4]=4;
  koef9.ma[5]=2; koef9.ma[6]=2; koef9.ma[7]=9; koef9.ma[8]=9;
  koef9.ma[9]=8; koef9.ma[10]=6; koef9.ma[11]=5; koef9.ma[12]=5;
  koef9.ma[13]=1; koef9.ma[14]=1; koef9.ma[15]=4;koef9.ma[16]=5;
  koef9.sm=1; koef9.se=0; koef9.ex=16;
 // Koeffizienten fÅr das Winkelson Polynom  a10
  koef10.ma[1]=1; koef10.ma[2]=3; koef10.ma[3]=0; koef10.ma[4]=7;
  koef10.ma[5]=5; koef10.ma[6]=3; koef10.ma[7]=5; koef10.ma[8]=0;
  koef10.ma[9]=1; koef10.ma[10]=0; koef10.ma[11]=5; koef10.ma[12]=4;
  koef10.ma[13]=0; koef10.ma[14]=3; koef10.ma[15]=9; koef10.ma[16]=5;
  koef10.sm=0;koef10.se=0; koef10.ex=15;
 // Koeffizienten fÅr das Winkelson Polynom  a11
  koef11.ma[1]=1; koef11.ma[2]=3; koef11.ma[3]=5; koef11.ma[4]=5;
  koef11.ma[5]=8; koef11.ma[6]=5; koef11.ma[7]=1; koef11.ma[8]=8;
  koef11.ma[9]=2; koef11.ma[10]=8; koef11.ma[11]=9; koef11.ma[12]=9;
  koef11.ma[13]=5; koef11.ma[14]=3; koef11.ma[15]=0;
  koef11.sm=1;koef11.se=0; koef11.ex=14;
 // Koeffizienten fÅr das Winkelson Polynom  a12
  koef12.ma[1]=1; koef12.ma[2]=1; koef12.ma[3]=3; koef12.ma[4]=1;
  koef12.ma[5]=0; koef12.ma[6]=2; koef12.ma[7]=7; koef12.ma[8]=6;
  koef12.ma[9]=9; koef12.ma[10]=9; koef12.ma[11]=5; koef12.ma[12]=3;
  koef12.ma[13]=8; koef12.ma[14]=1; koef12.ma[15]=0;
  koef12.sm=0;koef12.se=0; koef12.ex=13;
 // Koeffizienten fÅr das Winkelson Polynom  a13
  koef13.ma[1]=7; koef13.ma[2]=5; koef13.ma[3]=6; koef13.ma[4]=1;
  koef13.ma[5]=1; koef13.ma[6]=1; koef13.ma[7]=1; koef13.ma[8]=8;
  koef13.ma[9]=4; koef13.ma[10]=5; koef13.ma[11]=0; koef13.ma[12]=0;
  koef13.sm=1;koef13.se=0; koef13.ex=11;
 // Koeffizienten fÅr das Winkelson Polynom  a14
  koef14.ma[1]=4; koef14.ma[2]=0; koef14.ma[3]=1; koef14.ma[4]=7;
  koef14.ma[5]=1; koef14.ma[6]=7; koef14.ma[7]=7; koef14.ma[8]=1;
  koef14.ma[9]=6; koef14.ma[10]=3; koef14.ma[11]=0;
  koef14.sm=0;koef14.se=0; koef14.ex=10;
 // Koeffizienten fÅr das Winkelson Polynom  a15
  koef15.ma[1]=1; koef15.ma[2]=6; koef15.ma[3]=7; koef15.ma[4]=2;
  koef15.ma[5]=2; koef15.ma[6]=8; koef15.ma[7]=0; koef15.ma[8]=8;
  koef15.ma[9]=2; koef15.ma[10]=0; koef15.ma[11]=0;
  koef15.sm=1;koef15.se=0; koef15.ex=9;
 // Koeffizienten fÅr das Winkelson Polynom  a16
  koef16.ma[1]=5; koef16.ma[2]=3; koef16.ma[3]=3; koef16.ma[4]=2;
  koef16.ma[5]=7; koef16.ma[6]=9; koef16.ma[7]=4; koef16.ma[8]=6;
  koef16.sm=0;koef16.se=0; koef16.ex=7;
 // Koeffizienten fÅr das Winkelson Polynom  a17
  koef17.ma[1]=1; koef17.ma[2]=2; koef17.ma[3]=5; koef17.ma[4]=6;
  koef17.ma[5]=8; koef17.ma[6]=5; koef17.ma[7]=0;
  koef17.sm=1;koef17.se=0; koef17.ex=6;
// Koeffizienten fÅr das Winkelson Polynom  a18
  koef18.ma[1]=2; koef18.ma[2]=0; koef18.ma[3]=6; koef18.ma[4]=1;
  koef18.ma[5]=5;
  koef18.sm=0;koef18.se=0; koef18.ex=4;
 // Koeffizienten fÅr das Winkelson Polynom  a19
  koef19.ma[1]=2; koef19.ma[2]=1; koef19.ma[3]=0;
  koef19.sm=1;koef19.se=0; koef19.ex=2;
 // Koeffizienten fÅr das Winkelson Polynom  a20
  koef20.ma[1]=1;
  koef20.sm=0;koef20.se=0; koef20.ex=0;


  printf("  Koeffizienten des Wilkinson Polynoms:\n");
  printf("----------------------------------------\n");
  /* pout=koef0; print_zahl_s("a0");
  pout=koef1; print_zahl_s("a1");
  pout=koef2; print_zahl_s("a2");
  pout=koef3; print_zahl_s("a3");
  pout=koef4; print_zahl_s("a4");
  pout=koef5; print_zahl_s("a5");
  pout=koef6; print_zahl_s("a6");
  pout=koef7; print_zahl_s("a7");
  pout=koef8; print_zahl_s("a8");
  pout=koef9; print_zahl_s("a9");
  pout=koef10; print_zahl_s("a10");
  pout=koef11; print_zahl_s("a11");
  pout=koef12; print_zahl_s("a12");
  pout=koef13; print_zahl_s("a13");
  pout=koef14; print_zahl_s("a14");
  pout=koef15; print_zahl_s("a15");
  pout=koef16; print_zahl_s("a16");
  pout=koef17; print_zahl_s("a17");
  pout=koef18; print_zahl_s("a18");
  pout=koef19; print_zahl_s("a19");
  pout=koef20; print_zahl_s("a20"); */

  /* Berechnung der Nullstellen nach dem Hornerschema */
  for(inull=1; inull <=20 ; inull++) {
    xxx= null0;
    if(inull < 10) xxx.ma[1] = inull;
    if(inull==10) { xxx.ma[1]=1; xxx.ex=1;}
    if(inull==20) { xxx.ma[1]=2; xxx.ex=1;}
    if(inull > 10 && inull < 20) { xxx.ma[1]=1; xxx.ma[2]=inull-10; xxx.ex=1;}
   /* for(inull=0;inull<10;inull++) {
    xxx=null0; xxx.ma[1]=1;xxx.ma[2]=inull; xxx.ex=0;
    yyy=null0; */
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef20; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef19; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef18; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef17; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef16; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef15; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef14; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef13; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef12; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef11; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef10; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef9; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef8; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef7; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef6; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef5; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef4; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef3; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef2; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef1; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef0; add_sub("a");yyy=rz;
    printf("i= %d ",inull); pout=yyy; print_zahl_s("Nullstelle");
    }
    /* Berechnung der Nullstelle fÅer x = 4 nach Newton */
    /* Startwert x=3.5 */
    printf("Berechnung einer Nullstelle nach Newton :\n");
    xxx= null0; xxx.ma[1] = 3; xxx.ma[2]=7;
    for(i=0; i < 10; i++) {
    yyy=null0;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef20; add_sub("a");yyy=rz;
//    rx = xxx; ry = yyy; multiply(); rx=rz, ry=koef20; add_sub("a");dy=rz;
    dy=koef20;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef19; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz, ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef18; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef17; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef16; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef15; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef14; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef13; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef12; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef11; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef10; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef9; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef8; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef7; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef6; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef5; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef4; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef3; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef2; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef1; add_sub("a");yyy=rz;
    rx = xxx; ry = dy; multiply(); rx=rz; ry=yyy; add_sub("a");dy=rz;

    rx = xxx; ry = yyy; multiply(); rx=rz;  ry=koef0; add_sub("a");yyy=rz;
    // pout=yyy; print_zahl_s("yyy-Iteration");
    // pout=dy; print_zahl_s("dy-Iteration");

    rx = yyy; ry = dy; divide(); ry=rz; rx=xxx; add_sub("s"); xxx= rz;
    printf("it= %d ",i);
    pout=xxx; print_zahl_s("x-Iteration");
  }
  // setze Nullstelle ins Polynom ein : gibt es genau null ??
    yyy=null0;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef20; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef19; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef18; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef17; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef16; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef15; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef14; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef13; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef12; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef11; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef10; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef9; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef8; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef7; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef6; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef5; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef4; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef3; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef2; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef1; add_sub("a");yyy=rz;
    rx = xxx; ry = yyy; multiply(); rx=rz; ry=koef0; add_sub("a");yyy=rz;
    pout=yyy; print_zahl_s("Kontrollierte Nullstelle: ");

} // end wilkinson

 main()
{
  int i,jg;
/* definition von Konstanten
  --------------------------   */
 for(i=0; i<=laenge - 1 ; i++)
  {
   sieben8.ma[i]=0; null5.ma[i]=0; null0.ma[i]=0;
  }
// 7.8   Startwert fÅr Wurzel nach Heron
  sieben8.sm=0; sieben8.se=0; sieben8.ma[1]=7; sieben8.ma[2]=8;
  sieben8.ex=0;
//  0.5  fÅr Wurzel
  null5.sm=0; null5.se=1; null5.ma[1]=5; null5.ex=1;
// 0.0
  null0.sm=0; null0.se=0; null0.ex = 0;

  printf("Rechnen mit Zahlen grosser Genauigkeit \n");
  printf("====================================== \n");
  // print_zahl_s("test:");
  rx.ma[1] = 7; rx.sm = 0; rx.se = 1 ; rx.ex = 1;
  rx.ma[2] = 0;
  rx=null0;
  pout = rx;
  print_zahl_s("testrx:");
   ry.ma[1] = 9; ry.sm = 0; ry.se = 1; ry.ex = 11;
//  ry = null0;
  pout = ry;
  print_zahl_s("testry:");
  add_sub("a");
  pout = rz;
  print_zahl_s("summe:");
  printf("----------");
  ehochpiwu();
  // test
  /*rx=null0; ry=null0;
  rx.ma[1]=2;
  ry.ma[1]=9;
  countplus=0; countminus=0;
  wurzel();
  printf("countplus= %d countminus= %d\n", countplus, countminus);
  countplus=0; countminus=0;
  rx=rz; ry=rz;
   multiply();
  pout=rz;
  print_zahl_s("mult");
  printf("countplus= %d countminus= %d\n", countplus, countminus); */

  // ypsilonhochx();
  // ehochx();
  // piramanujan();
  // pigaussagm();
  // cosinus();
  // sinus();
  // arcussinus();
  // eulerzahl();
  // logarithmusn();
  // zehnhochx();
  // arcustangens();
  // ln10();
  //  getch();
  // Berechne Wilkinson Polynom Nullstellen
 goto skip1;
  wilkinson();
 // skip1:
 // Pi nach Gauss
 //goto skip2;
 // pigauss();
 //skip2:
 // Wurzel
 	jg=2;
   printf("WURZEL %d\n",jg);
   rx=null0; rx.ma[1]=jg; rx.ex=0;
   wurzel();  /* 1-100 */
   pout=rz; print_zahl_s("wurzel:");
   //   getch();
 skip1:
 goto skip3;
 for (jg=1; jg<=10; jg++)
 {
  if (jg < 10) {
   printf("WURZEL %d\n",jg);
   rx=null0; rx.ma[1]=jg; rx.ex=0;
   wurzel();  /* 1-100 */
   pout=rz; print_zahl_s("wurzel:");
   rx=rz; ry=rz;
   multiply();
   pout=rz;
   print_zahl_s("wurzel* wurzel: ");
  }
 }
 skip3:
;
 /*  add_sub("a"); */
/* divide();  */
//  multiply();
//  pout = rz;
//  print_zahl_s("mult:");
//  exit(0);

 /*  if (jg >=90 && jg <=95 ) {
         printf("WURZEL %d\n", jg);
         rx=null0; rx.ma[1]=9; rx.ma[2]=jg-90; rx.ex=1;
	 wurzel();   1-100 */
	   /*            pout=rz;
             print_zahl_s("wurzel= ");
             rx=rz;
             ry=rz;
             multiply();
             pout=rz;
             print_zahl_s("wurzel* wurzel: ");
   }
 }
          rx=null0; rx.ma[1]=1; rx.ma[2]=0; rx.ex=0; rx.se=0; rx.ma[50]=1;
          pout=rx;
             print_zahl_s("zahl = ");
             wurzel();   1-100 */
 /*            pout=rz;
             print_zahl_s("wurzel=  ");
             rx=rz;
             ry=rz;
             multiply();
             pout=rz;
             print_zahl_s("wurzel* wurzel: ");
//  divide();
//  pout = rz;
//  printf("j=%d\n",j);
   cout << "countplus = " << countplus << "   countminus = " << countminus << endl;
//   print_zahl_s("wurzel: ");
//  }*/
/*  rx=rz; ry=rz;
   multiply();
   pout=rz;
   print_zahl_s("wurzel2 * wurzel2: ");
/*
  cout << "Hello World";
  cin >> eing;
*/
/*
  Blaise Pascal (1623 - 1660)
  ----------------------------
  Franz˜sischer Mathematiker, Physiker und Philosoph.
  1642 erfand er eine Rechenmaschine, die addieren konnte (zehn-
  stufige ZahnrÑder, ZehnerÅbertrag Åber Klauen, in Paris vorgefÅhrt).
  Arbeiten Åber Zahlenkombinatorik (Pascalsches Dreieck).

  Archimedes und die Kreiszahl PI :
  ---------------------------------
   PI ist der 16. Buchstabe des Griechischen Alphabets und Symbol
   fÅr den Umfang des Kreises mit Durchmesser 1.
   Pi ist ein unendlicher nicht periodischer Dezimalbruch .
   Archimedes war ein bedeutender Mathematiker und Physiker der Antike,
   lebte in Syrakus, Studium in Alexandria. (287 v. Chr. bis 212 v. Chr).
   Er hat ein iteratives  Verfahren zur Berechnung von PI ca 260 v. Chr.
   gefunden.
   Im 16. Jahrhundert war PI bereits auf 35 Stellen bekannt ("Ludolfsche
   Zahl").
   Ludolf van Ceulen (1540 bis 1610), Mathematiker.
   1961 100000 Stellen, 1985 8 Millionen Stellen berechnet.

  Carl Friedrich Gauss  (1777 bis 1855):
  --------------------------------------
  Mathematiker, Physiker und Astronom, studierte in G˜ttingen,
  Professor fÅr Astronomie, moderne Zahlentheorie, lieferte
  Bahnbrechende BeitrÑge zu fast allen Gebieten der Mathematik.
  Algorithmus zur schnellen Berechnung von PI .

  Heron von Alexandria (2. HÑlfte des 1. Jahrhunderts v. Chr.)
  ---------------------
  Verfasser geometrischer Schriften

  Leonhard Euler (1707 bis 1783)
  ------------------------------
  Schweizer Mathematiker, arbeitet auf fast allen Gebieten der
  Mathematik. Die Zahl "e" ist nach ihm benannt (Eulersche Zahl).
  "e" ist ein irrationale Zahl, Grundzahl der natÅrlichen Logarithmen.
  1748 Ver˜ffentlichungen von Euler zu dieser Zahl.

  Isaac Newton (1643 - 1727) und Gottfried Wilhelm Leibnitz(1646 - 1716)
  ----------------------------------------------------------------------
  Erfanden die Differentialrechnung

  Brook Taylor (1685 bis 1731)
  -----------------------------
  Englische Mathematiker, Arbeiten Åber unendliche Reihen.

  Baron John Neper (1550 - 1617)
  ------------------------------
  Schottischer Adeliger, 1614 erste Logarithmentafel erstellt.
  Hat die Bezeichnung Logarithmus eingefÅhrt.

  Briggs Henry (1556 bis 1630)
  ----------------------------
  Londoner  Mathematikprofessor, entwickelte dekadische Logarithmen.
  Hat 1617 14 stellige Logarithmen von 1 bis 1000 ver˜ffentlicht.
  1624 "Arithmetica Logarithmica" enthielt 14 stellige Logarithmen
  von 1 bis 20000.   */
}

