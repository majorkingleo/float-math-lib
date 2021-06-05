#include "floatfunc.h"
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
} /* ende add_sub */