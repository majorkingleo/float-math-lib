#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <conio.h>
#include "confunc.h"

char ergbuffer[1000];
#define WURZEL 1

/* maximale Werte füer n = 1030, Läenge = 1050
   Länge sollte immer um ca 20 grösser sein   */
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
  /* Variable für PI */
wortform zwei, eins, zahl1d18, zahl1d57, zahl1d239,
sum1, sum2, sum3, zahl1d57_2, zahl1d18_2, zahl1d239_2,
nenner, zaehler1, zaehler2, zaehler3, x0,
xx1, xx2, xx3, xxx, piold ;

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
 for(i=laenge; i>=0; i--)
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
 for(i=laenge; i>=0; i--)
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
  ra.ma[i] = ra.ma[i] + rb.ma[i];
  ra.ma[i] = ra.ma[i] + carry;
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
/* right shift rb cnt digits, links 0 einfügen
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
   zu ra addiert , Ergebnis in ra, schiebe dann
   ra weiter */
void mult_rb_rq()
{ int i,jj ;
 for(i=n-1; i>=1; i--) {
    cnt = rq.ma[i];
      for (jj=1; jj<= cnt; jj++) {
        // printf("cnt,jj,i %d %d %d \n",cnt,jj, i);
        ra_plus_rb();
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
    ra_plus_rb(); /* Divisor addieren für positiv ra */
    leftshift_ra();  /* nächste Stelle */
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
                1.5e-2 ist grösser als 1.5e-13
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
  x_zero_test;
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
} /* ende add_sub */
/**********************************************/
/* Korrektur für mult und div
   Korrigiere das Ergebnis und den Exponent
   bestimme Vorzeichen Exponent und addiere Exponent  */
void mult4()
{ 
 if ((rx.se ^ ry.se) == 0 ) {
 /* gleiches Vorzeichen Exponent */
    if ( (rx.ex + ry.ex) > maxexp ){
    /* overflow */
      if( (rx.ex + ry.ex) != 0 ){
       /* zu klein oder zu groß */
       rz.se = rx.se ; rz.sm = rx.sm ;
         if(rz.se == 1) { clear_rz();} else { infin();}
       goto fertig;
      }
      if(multdiv == true) {  /* für div */
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
 /* übernimm richtiges Vorzeichen vom Exponent, wenn Differenz 0,
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
     oder x < y. Dann muß der Exponent noch extra korrigiert werden */
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
  rightshift_ra(); /* Übertrag, schiebe rechts */
  if(rz.se == 1) {
  /* sign Exponent negativ */
   rz.ex--;
   /* springe noch einmal zurück und setze Exp 0 */
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
  /* wann kommt die nächste Zeile vor */
  if(ra.ma[1] == 0){
    leftshift_ra();
  }
  /* Korrigiere Exponent, multdiv flag steuert mult/div */
  mult4();
  /* setze original Exp y wieder */
  ry.se = ry.se ^ 1;
  fertig:
}
/**********************************************/
/* Berechne Wurzel nach der Heron-Methode
   m0 = 7.8 , m1 =(zahl/m0 + m0) * 0.5,
   m2=(zahl/m1 + m1) * 0.5,
   rz = sqrt(rx)
   Gültiger Bereich für rx ist 1 -100
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
 /* 15 ist ausreichend für 100 Stellen, bei groesserer
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
/* Berechnung von PI nach GAUSS  */
/* PI = 48 arctan(1/18) + 32 arctan(1/57) - 20 arctan(1/239)
   arctan= x - x^3/3 + x^5/5 -+...(-1)^(k-1)/(2k-1) + (-1)^k/(2k+1) */
// mit laenge=750, n=730 , maxexp=200 ,i=300 stimmt PI auf 700
// Stellen
 void pigauss()
 {
  int i ;
 printf("Berechne PI nach arctan Reihe Gauss (1-400)\n");
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
  for(i=1; i<=300; i++)
  {
    printf(" %d",i);
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
 //  rx= sum1; ry=sum2; add_sub("a"); rx = rz; ry = sum3; add_sub("s");
 pout = rz; printf("\n"); print_zahl_s("PI :"); xxx= rz;
 rx=rz; ry=piold; add_sub("s"); pout= rz; print_zahl_s("Unterschied");
 piold = xxx;
 }
 }


/**********************************************/

int floatfunc(int artflag)
{
  int i,jg;
/* definition von Konstanten
  --------------------------   */
 for(i=0; i<=laenge; i++)
  {
   sieben8.ma[i]=0; null5.ma[i]=0; null0.ma[i]=0;
  }
// 7.8   Startwert für Wurzel nach Heron
  sieben8.sm=0; sieben8.se=0; sieben8.ma[1]=7; sieben8.ma[2]=8;
  sieben8.ex=0;
//  0.5  für Wurzel
  null5.sm=0; null5.se=1; null5.ma[1]=5; null5.ex=1;
// 0.0
  null0.sm=0; null0.se=0; null0.ex = 0;

//  printf("Rechnen mit Zahlen grosser Genauigkeit \n");
//  printf("====================================== \n");
 /* print_zahl_s("test:");   */
 /*  rx.ma[1] = 7; rx.sm = 1; rx.se = 0 ; rx.ex = 9;
  rx.ma[2] = 0;
  pout = rx;
  print_zahl_s("testrx:");
  ry.ma[1] = 9; ry.sm = 1; ry.se = 0; ry.ex = 0;
  pout = ry;
  print_zahl_s("testry:");  */
 // Berechne Wilkinson Polynom Nullstellen
/* goto skip1;
  wilkinson();
 skip1:
*/ // Pi nach Gauss
 //goto skip2;
//  pigauss();
 //skip2:
 // Wurzel
/* goto skip3;
 for (jg=1; jg<=10; jg++)
 {
  if (jg < 10) {
   printf("WURZEL %d\n",jg); */
   /* format Zahl  outbuffer
//   +-x.xxxxxxxxE+-yyyy

   1.konvertiere Exponent
   */
   rx=null0; rz=null0;
   int buflen = strlen(outbuffer)-1;
   int esum = 0;
   int stelle = 1;
   for(int i=buflen; i>=0; i--)
   {
    if(outbuffer[i]=='+' || outbuffer[i]=='-')  {
     if(outbuffer[i]=='+')
     {
      	rx.se = 0;
      	break;
     }else{
      	rx.se = 1;
      	break;
     }
    }
    rx.ex = rx.ex + (outbuffer[i]-48)*stelle;
    stelle = stelle*10;
	}
// 2.konvertiere Mantisse //
  if(outbuffer[0]=='+')
   {
    rx.sm = 0;
   }else{
    rx.sm = 1;
   }
   rx.ma[1]=outbuffer[1]-48;
   for(int i=3; i<buflen; i++)
   {
    if(outbuffer[i]=='E') break;
    rx.ma[i-1] = outbuffer[i]-48;
   }
   switch(artflag)
   {
    case WURZEL:
    		if(rx.sm==1){sprintf(errorbuffer,"Negative Wurzel nicht erlaubt!"); return(-1);}
    		wurzel();
      	break;
   }
   // konvertiere zurück in ergbuffer format: +-x.xxxxxxxxE+-yyyy
   memset(&ergbuffer,32,sizeof(ergbuffer));
   if(rz.sm==0)
   {
    	ergbuffer[0]='+';
   }else{
      ergbuffer[0]='-';
   }
   ergbuffer[1]=rz.ma[1]+48;
   ergbuffer[2]='.';
   for(int i=2; i<n; i++)
   {
    ergbuffer[i+1]=rz.ma[i]+48;
   }
   ergbuffer[n+3]='E';
   if(rz.se==0)
     ergbuffer[n+4]='+';
   else
   	ergbuffer[n+4]='-';

char sexp[20];
	 if(rz.se==0)
    	 sprintf(sexp,"%0.4d",rz.ex);
    else
    	 sprintf(sexp,"%0.4d",rz.ex);
    ergbuffer[n+5]='\0';
    strcat(ergbuffer,sexp);

//   rz.se=0;

//   rx.ex=0;
 /*  rx=null0; rx.ma[1]=jg; rx.ex=0;
   wurzel();  /* 1-100 */
 /*  pout=rz; print_zahl_s("wurzel:");
   rx=rz; ry=rz;
   multiply();
   pout=rz;
   print_zahl_s("wurzel* wurzel: ");
//  }
 //}
// skip3:
 /*  add_sub("a"); */
/* divide();  */
//  multiply();
//  pout = rz;
//  print_zahl_s("mult:");
//  exit(0);

 /*  if (jg >=90 && jg <=95 ) {
         printf("WURZEL %d\n", jg);
         rx=null0; rx.ma[1]=9; rx.ma[2]=jg-90; rx.ex=1;
           wurzel();  /* 1-100 */
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
             wurzel();  /* 1-100 */
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
//  }
/*  rx=rz; ry=rz;
   multiply();
   pout=rz;
   print_zahl_s("wurzel2 * wurzel2: ");
/*
  cout << "Hello World";
  cin >> eing;
*/
}

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
    Mantisse            : array  0..Länge, Feld 0 bleibt frei
                           an jeder Position steht die Zahl 0-9
    Exponent            : Integer Zahl, nur positiv
   Der (fiktive) Dezimalpunkt steht immer nach der 1. Stelle, das Ergebnis
   aller Operationen wird immer in dieser Darstellung präsentiert.
   Die Eingabe jeder Zahl in die Register muss immer in diesem
   Format erfolgen !!
   Beispiel : 3.88888 e-77  , 1.0 e0,  -9.999 e33
   Beispiel für Eingabe : Zahl 1.9e-33
   rx.ma[1]=1; rx.ma[2]=9 ; rx.se = 1 ; rx.ex = 33; rx.sm = 0;

   Beachte, daß addsub die Register rx, ry so vertauscht, dass
   in rx immer der groesste Absolutbetrag steht.
   alle Register müssen vom Typ 'wortform' sein.
 */
