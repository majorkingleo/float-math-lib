/****************************************************************/
/*                        convert.cpp                           */
/* convert test buffer string for floating point numbers        */
/* Letzte Änderung 28-Nov-98                                    */
/****************************************************************/
/* Dieses Programm konvertiert eine integer oder Fliesskomma    */
/* Zahl in eine normierte Form +-x.xxxxE+-yyy                   */
/* Erlaubte Eingabeformate sind : 123, -127, +3333333,          */
/*   3.14, 62172., -.00127, +5.0E3, 2E-3, 2E+0, -1E+3, 1E0,     */
/*   32E-45, -47.E47, 0.000001e12, 10e0, 1e004                  */
/*   erlaubte Zeichen sind 0,1,2,..9,.,e,E, space,+,-           */
/****************************************************************/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
    main(/*argc,argv*/)  /*   */
// int argc;
// char *argv[];
 {
   char buffer[100];     // eingabe
   char outbuffer[100],expcharbuffer[20];
   int laenge, kk, kkk, iok, errpos, goodcount, dezppos, expfound;
   int pluscount, minuscount;
   int expsum, expsumsave, expvorz, /*expval, explen,*/
       expstart, expend, expbuffer[20],ecount;
   int manvorz, manlen, manstellen, manstellenpkt,/* manstart, manend,*/
       manbuffer[100];
   int manstflag, mannullc, mannullv, manpktflag;
   int /*ll,*/ ii, p10;
   /* int A [109][3]; */
   printf("-----------------------------------------------------\n");
   printf("                   convert.cpp \n");
   printf("-----------------------------------------------------\n");
   printf("Konvertiere eine beliebige Zahl in das \n");
   printf("Fliesskommaformat +-x.xxxxE+-yyy \n");
   printf("Gueltige Eingabeformate sind z.B.: 123, -55.444,\n");
   printf("                0.0000006400, -12e5, 65.45E+065,\n");
   printf("                33e0, 1e-32\n");
   printf("-----------------------------------------------------\n");
   loop:
   ;
   printf(" >>> Gib eine Zahl ein:\n");
   gets(buffer);
   printf("Die Zahl ist : ");  
   puts(buffer);
   laenge= strlen(buffer);
 //  printf ("Laenge des Strings: %d \n",laenge);
 /* pruefe auf gueltige Zeichen */
   iok = 0;
   dezppos = 0;
   expfound = -1;
   expsum = 0; goodcount=0 ;ecount = 0; pluscount = 0; minuscount=0;
   for( kk=0; kk<=laenge - 1 ;kk++) {
     // printf("buffer = %c\n",buffer[kk]);
     if(buffer[kk] == '0') {goodcount ++; continue;}
     if(buffer[kk] == '1') {goodcount ++; continue;}
     if(buffer[kk] == '2') {goodcount ++; continue;}
     if(buffer[kk] == '3') {goodcount ++; continue;}
     if(buffer[kk] == '4') {goodcount ++; continue;}
     if(buffer[kk] == '5') {goodcount ++; continue;}
     if(buffer[kk] == '6') {goodcount ++; continue;}
     if(buffer[kk] == '7') {goodcount ++; continue;}
     if(buffer[kk] == '8') {goodcount ++; continue;}
     if(buffer[kk] == '9') {goodcount ++; continue;}
     if(buffer[kk] == '+') {pluscount++; goodcount ++; continue;}
     if(buffer[kk] == '-') {minuscount++; goodcount ++; continue;}
     if(buffer[kk] == '.') {dezppos= kk; goodcount ++; continue;}
     if(buffer[kk] == 'e') {expfound=kk; ecount++;goodcount ++; continue;}
     if(buffer[kk] == 'E') {expfound=kk; ecount++;goodcount ++; continue;}
     iok = 1;
     errpos = kk;
     break;
   }
   if(iok==1) {
     printf(">>> Fehlerhafte Eingabe an Position : %d  Zeichen : %c \n",
     errpos+1, buffer[errpos]);
     goto loop;
     exit(1);
   }
   if(ecount > 1) {
     printf(">>> Fehlerhafter Exponent, 2 x definiert \n");
     goto loop;
     exit(1);
   }
   if(pluscount > 2 || minuscount > 2) {
     printf(">>> Zu viele Vorzeichen, illegale Eingabe \n");
     goto loop;
     exit(1);
   }

/*------------------------------------------------------------*/
/*  normalisiere Eingabe, Dezimalpunkt immer nach 1. Stelle   */
/* 1.)  Berechne Exponent, wenn er existiert */
/*------------------------------------------------------------*/
   if(expfound > -1 ) {
      // printf("Berechne Exponent\n");
      expvorz = 0 ; /* 0 = + , 1 = -, default = + */
    //  explen = 0;
      /* suche bis Beginn der exp Zahl */
      for( kk=expfound+1; kk<=laenge - 1 ;kk++) {
         if(buffer[kk] == '+') { continue; }
         if(buffer[kk] == '-') { expvorz = 1; continue; }
         // if(buffer[kk] == '0') { continue;}
         expstart = kk;
         break;
      }
      for( kk=laenge-1; kk>=0  ;kk--) {
         if(buffer[kk] == ' ') { continue; }
         expend = kk;
         break;
      }
      if( expend - expstart < 0 ) {  
       printf(">>> Illegaler Exponent \n");
       goto loop;
       exit(1);
      }
      ii=0;
      for( kk=expstart; kk<=expend  ;kk++) {
           if(buffer[kk] == '.' || buffer[kk] =='+' ||
              buffer[kk] == '-' ) {
             printf(">>> Illegaler Exponent, illegal character \n");
             goto loop;
             exit(1);
           }
         if(buffer[kk] == ' ') { continue; }
         // printf("buffer = %c\n",buffer[kk]);
         ii++;
         expbuffer[ii] = buffer[kk];
      }
      /* konvertiere auf integer Zahl */
      p10 = 1;
      expsum = 0;
      for( kk=ii; kk>=1  ;kk--) {
         expsum = expsum + (expbuffer[kk]- 48) * p10;
         p10 = p10 * 10;
      }
      // printf("Exponent = %d\n", expsum);
   }
   expsumsave = expsum;
/* ------------------------------------------------- */
/* 2. Berechne Mantisse  */
/* ------------------------------------------------- */
 /* berechne Stellen bis zum Dezimal Punkt oder bis zum Ende (integer) */
   if(expfound == -1 ) { manlen = laenge;} else { manlen=expfound;}
   // printf("Laenge der Mantisse = %d\n",manlen);
   manvorz = 0;
   manstellen = 0; manstellenpkt = 0;
   //ll=0;
   manpktflag = 0;
   manstflag=0;
   mannullc=0;
   mannullv=0;
   for( kk=0; kk<=manlen ;kk++) {
    /*  printf("buffer = %c\n",buffer[kk]); */
     if(buffer[kk] == '0' && manstflag == 0) {mannullc++;}
     /* Zaehle vorlaufende NullStellen nach Dez Pkt */
     if(buffer[kk] == '0' && manpktflag == 1 && manstellen == 0)
        {mannullv++; continue;}
     if(buffer[kk] == '0' && manstellen > 0 )
        { manstellen++; manbuffer[manstellen] = buffer[kk];}
     if(buffer[kk] == '1'){
      manstflag=1; manstellen++; manbuffer[manstellen] = buffer[kk];continue;
     }
     if(buffer[kk] == '2') {
      manstflag=1; manstellen++; manbuffer[manstellen] = buffer[kk];continue;
     }
     if(buffer[kk] == '3') {
      manstflag=1; manstellen++; manbuffer[manstellen] = buffer[kk];continue;
     }
     if(buffer[kk] == '4') {
      manstflag=1; manstellen++; manbuffer[manstellen] = buffer[kk];continue;
     }
     if(buffer[kk] == '5') {
      manstflag=1; manstellen++; manbuffer[manstellen] = buffer[kk];continue;
     }
     if(buffer[kk] == '6') {
      manstflag=1; manstellen++; manbuffer[manstellen] = buffer[kk];continue;
     }
     if(buffer[kk] == '7') {
      manstflag=1; manstellen++; manbuffer[manstellen] = buffer[kk];continue;
     }
     if(buffer[kk] == '8') {
      manstflag=1; manstellen++; manbuffer[manstellen] = buffer[kk];continue;
     }
     if(buffer[kk] == '9') {
      manstflag=1; manstellen++; manbuffer[manstellen] = buffer[kk];continue;
     }
     if(buffer[kk] == '+' && kk == 0) { continue; }
     if(buffer[kk] == '-' && kk == 0) { manvorz = 1; continue;}
     if(buffer[kk] == '+' && kk > 0) {
        printf(">>> Illegales + an Position :%d\n", kk);
        goto loop;
        exit(1);
     }
     if(buffer[kk] == '-' && kk > 0) {
        printf(">>> Illegales - an Position :%d\n", kk);
        goto loop;
        exit(1);
     }
     if(buffer[kk] == '.') {
       manstellenpkt = manstellen;
       manpktflag = 1;
     }
     if(buffer[kk] == ' ') {
        printf(">>> Illegales space an Position :%d\n", kk);
        goto loop;
        exit(1);
     }
   }
  // printf("GesamtZahl der signifikanten Stellen = %d\n",manstellen);
  // printf("Zahl der Stellen bis Dezimal Punkt= %d\n",manstellenpkt);
  // printf("Dezimalpunkt gefunden = %d\n",manpktflag);
  // printf("Zahl der vorlaufenden Nullstellen vom Dezimalpkt weg = %d\n",
  //         mannullv);
  // printf("GesamtZahl der vorlaufenden Nullen = %d\n",mannullc);
   if(manstellen == 0 && buffer[0] == '0'){
    manstellen = 1; manbuffer[1] = '0';
   }
   if(manstellen == 0 ) {
     printf(">>> Keine Mantisse gefunden\n");
     goto loop;
     exit(1);
   }
  // for(kk=0; kk<=manstellen; kk++) {
   // printf("%c",manbuffer[kk]);
   // }
 /*----------------------------------------------------------*/
 /* 3. gib neue Zahl aus */
 /* Die Zahl wird immer im Format +-x.xxxxeE+-yyy ausgegeben */
 /*----------------------------------------------------------*/
   printf("Die neue konvertierte Zahl ist : \n");
    if(manvorz == 0) {
       printf("+"); outbuffer[0] = '+';
   } else {
       printf("-"); outbuffer[0] = '-';
   }
   kkk=1;
   for(kk=1; kk<=manstellen; kk++) {
     printf("%c",manbuffer[kk]);
     outbuffer[kkk] = manbuffer[kk]; kkk++;
     if(kk==1) {printf("."); outbuffer[kkk]='.'; kkk++;}
   }
   outbuffer[kkk]='\0';
   if(manpktflag == 0) {
   /* kein dp gefunden */
       if(expvorz == 0) {
         expsum = expsum + manstellen - 1 ;
       } else {
         expsum = -expsum + manstellen - 1 ;
       }
       printf(" E %3d\n", expsum);
   } else {
   /* dp gefunden */
      if(expvorz == 0) {
         expsum = expsum + manstellenpkt - 1 ;
       } else {
         expsum = -expsum + manstellenpkt - 1 ;
       }
       if(mannullv > 0 ) expsum = expsumsave - mannullv - 1;
       printf(" E %3d\n", expsum); 
   }
   if(expsum >= 0 ) {
     strcat(outbuffer,"E+");
   } else {
     strcat(outbuffer,"E");
   }
   sprintf(expcharbuffer,"%0.4d",expsum);
//   printf("buffer : %s\n", outbuffer);
//   printf("Zeichen :%s\n",expcharbuffer);
     strcat(outbuffer, expcharbuffer);
     printf("String : %s\n", outbuffer);
     
/* nächste Zahl */
    goto loop ;
    printf(" ");
    gets(buffer);
} /* Ende Function */
