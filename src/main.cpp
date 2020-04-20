#include <STRATEGIET.h>


U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 12, /* reset=*/ 8);



void drawLogo(void)
{
    u8g2.setFontMode(1);	// Transparent


    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.drawStr(0, 22, "U");

    u8g2.setFontDirection(1);

    u8g2.drawStr(14,8,"4");

    u8g2.setFontDirection(0);

    u8g2.drawStr(36,22,"g");
    u8g2.drawStr(48,22,"\xb2");

    u8g2.drawHLine(2, 25, 34);
    u8g2.drawHLine(3, 26, 34);
    u8g2.drawVLine(32, 22, 12);
    u8g2.drawVLine(33, 23, 12);



}


bool detection=true, checkMenu1=false, quitMenu1=false, quitMenu2=false, demandeRecalage=false, cote=coteJaune, start=false;
bool verif=false;

int intdeplakment[18][3],k=0,x=0;
int PinCLK = 1;
int PinDT = 7;
int PinSW = 0;
int PinCLKLast = LOW,PinSWLast=LOW;
int n=LOW,n1=LOW;
int nbClic=0, ready=2, timer;
static long encoderPos = 0;    // Au 1er démarrage, il passera à 0

float deplakment[18][3];

byte Trame[6],envoye=0,com=0;

// ---------------------------------------------------------------------------
// Un programme Arduino doit impérativement contenir la fonction "setup"
// Elle ne sera exécuter une seule fois au démarrage du microcontroleur
// Elle sert à configurer globalement les entrées sorties
// ---------------------------------------------------------------------------
void setup()
{
  pinMode(9, OUTPUT);
  digitalWrite(9, 0);	// default output in I2C mode for the SSD1306 test shield: set the i2c adr to 0
  pinMode(5, INPUT_PULLUP); // pin de la tirette

  u8g2.begin();

  deplakment[0][1]=1000;//x
  deplakment[0][2]=1000;//y
  deplakment[0][3]=0;//angle initial

  deplakment[1][1]=750;
  deplakment[1][2]=1000;
  deplakment[1][3]=PI/6;//angle voulu

  deplakment[2][1]=1000;
  deplakment[2][2]=1000;
  deplakment[2][3]=PI/6;

  deplakment[3][1]=750;
  deplakment[3][2]=750;
  deplakment[3][3]=0;

  deplakment[4][1]=1000;
  deplakment[4][2]=1000;
  deplakment[4][3]=PI/3;

  deplakment[5][1]=1250;
  deplakment[5][2]=750;
  deplakment[5][3]=0;

  deplakment[6][1]=1000;
  deplakment[6][2]=1000;
  deplakment[6][3]=0 ;

  deplakment[7][1]=1250;
  deplakment[7][2]=1250;
  deplakment[7][3]=0;

  deplakment[8][1]=1000;
  deplakment[8][2]=1000;
  deplakment[8][3]=3*PI/2 ;

  deplakment[9][1]=1000;
  deplakment[9][2]=750;
  deplakment[9][3]=PI/2;

  deplakment[10][1]=1000;
  deplakment[10][2]=1000;
  deplakment[10][3]=0;

  deplakment[11][1]=750;
  deplakment[11][2]=1250;
  deplakment[11][3]=3*PI/2;

  deplakment[12][1]=1000;
  deplakment[12][2]=1000;
  deplakment[12][3]=0;

  deplakment[13][1]=1250;
  deplakment[13][2]=1000;
  deplakment[13][3]=PI/4;

  deplakment[14][1]=1000;
  deplakment[14][2]=1000;
  deplakment[14][3]=0;

  deplakment[15][1]=1000;
  deplakment[15][2]=1250;
  deplakment[15][3]=0;

  deplakment[16][1]=1000;
  deplakment[16][2]=1000;
  deplakment[16][3]=0;

  deplakment[17][1]=1000;
  deplakment[17][2]=1000;
  deplakment[17][3]=0;

  pinMode (PinCLK,INPUT_PULLUP);
  pinMode (PinDT,INPUT_PULLUP);
  pinMode (PinSW,INPUT_PULLUP);
  // Ouvre le port série et fixe le debit de communication à 9600 bauds
  Serial.begin(9600);

  // Initialise la library Wire et se connecte au bus I2C en tant que maître
  Wire.begin();

  // Test si la Teensy fonctionne en allumant sa LED
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

  MsTimer2::set(1000,IntrerrupTimer);//tout les  seconde
  MsTimer2::start();

  //création Ecran de démarrage
  u8g2.clearBuffer();         // clear the internal memory
  drawLogoArea();
  delay(3000);

  while( x <=138){ //défiler logo
    u8g2.clearBuffer();        // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
    u8g2.setBitmapMode(0);
    u8g2.drawXBM( x, 0, logoAREA_width, logoAREA_height, logoAREA);
    u8g2.sendBuffer();
    x+=10; // défiler plus vite ( penser à mettre <=128+10 pour être sur que le logo disparait totalement à droite)
  }

  u8g2.clearBuffer();

  //Les Menus pour la Mise en place
  while(verif!=true){
    //Utilisation Menu 1 pour configurer
    algoMenu1();

    //Utilisation Menu 2 pour confirmer si tout est ok
    menu2();
    quitMenu1=false;
    quitMenu2=false;
  }
  do{
    drawLogo();
    start=digitalRead(5);
    delay(10);
    Serial.println(start);
  }while(start==0);
  timer=0;
  u8g2.clearBuffer();
}


// ---------------------------------------------------------------------------
// Le programme principal s’exécute par une boucle infinie appelée Loop ()
// ---------------------------------------------------------------------------

void loop(){

  drawLogoArea();
  u8g2.drawStr(0,44,"Score");
  u8g2.sendBuffer();
  delay(200);

  if(timer==100)Serial.print("FIN");

  if (PinSW==1){
    //lecture si le robot a fini de se déplacer
    Wire.requestFrom(I2C_SLAVE_DEPLACEMENT_ADDRESS,1);
    com=Wire.read();
    Serial.println(com);

    if (com==0){ //si il a fini...
      if(k==0)readRegisterAndSendValue();//si on est au tout début--> position initiale
      envoye=1;
      k++;
      readRegisterAndSendValue();//envoi prochaine position
      com=1; //Pour ne pas retourner dans cette condition si la loop revient trop rapidement
    }

    delay(1000);
  }


}


void readRegisterAndSendValue() {

  int q=0;

    // Envoie de la valeur à l'esclave I2C.
    // Principe et description :
    // 1 : Commencez une transmission vers la carte esclave I2C avec l'adresse donnée.
    // 2 : Ensuite, mettez en file d'attente les octets à transmettre avec la fonction write ()
    // 3 : transmettez-les en appelant la fonction endTransmission () .

    // débute de la communication avec un esclave (ouvre le stockage données à envoyer avec write)

    deplakment[k][3]*=1000; // Pour gagner en précision --> /100 dans le programme déplacement

    intdeplakment[k][1]=deplakment[k][1];
    intdeplakment[k][2]=deplakment[k][2];
    intdeplakment[k][3]=deplakment[k][3];


    //Chargement des octets à partir du tableau de déplacement pour une transmission du composant maître vers un composant esclave
    // Rq : la trame 0 est une trame rassemblant les bits restants pour x y et turn

    Trame[0]=intdeplakment[k][1] >> 8;
    Trame[1]=intdeplakment[k][1] & 255;
    Trame[2]=intdeplakment[k][2] >> 8;
    Trame[3]=intdeplakment[k][2] & 255;
    Trame[4]=intdeplakment[k][3] >> 8;
    Trame[5]=intdeplakment[k][3] & 255;

    Wire.beginTransmission(I2C_SLAVE_DEPLACEMENT_ADDRESS);
    for (int i=0; i<=5; i++){
        Wire.write(Trame[i]);

        Serial.print("Trame");
        Serial.println(Trame[i]);
        q++;
        Serial.print("q");
        Serial.println(q);
      }

  Wire.endTransmission();

}

void drawLogoArea(){
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.setBitmapMode(0);
  u8g2.drawStr(0,34,"TEAM");
  u8g2.drawXBM( 33, 0, logoAREA_width, logoAREA_height, logoAREA);
  u8g2.drawStr(95,34,"AREA");
  //  u8g2.blink(); //clignoter le msg
  u8g2.sendBuffer();          // transfer internal memory to the display
}

void algoMenu1(){ //Utilisation Menu 1
  while(quitMenu1==false){
    n1=digitalRead(PinSW);
    //Serial.println(n1);
    //Serial.println(PinSWLast);
    if (PinSWLast==LOW && n1==LOW) {   // Reset la position si on appui sur le potentiomètre
      nbClic++;
      //Serial.print(nbClic);
    }
    delay(500);
    switch (nbClic){
      case 0:
      lectureEncoder();
      cote=encoderPos%2;
      menu1(10);
      break;

      case 1:
      lectureEncoder();
      demandeRecalage=encoderPos%2;
      menu1(20);
      break;

      case 2:
      lectureEncoder();
      checkMenu1=encoderPos%2;
      menu1(40);
      break;

      case 3:
      if (checkMenu1==false) nbClic=0; // Retourne en haut de la page
      else if (demandeRecalage==true) { // Réaliser un 1er recalage bordure
        Serial.println("RECALAGE BORDURE");
        nbClic=1; //Retourne à la ligne de demande
      }
      else if (checkMenu1==true){
        nbClic=0; //Reset nombre de clics
        quitMenu1=true;
        u8g2.clearBuffer();
      }
      break;
    }
  }
}

void menu1(int blanc){
  u8g2.clearBuffer();         // clear the internal memory
  //  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.setCursor(118,64);
  u8g2.print(encoderPos);
  u8g2.setFontMode(1);  /* activate transparent font mode */
  u8g2.setDrawColor(1); /* color 1 for the box */
  u8g2.drawBox(0, blanc, 128, 10);
  u8g2.setDrawColor(2);
  u8g2.drawStr(0,9, "         Mise en place");
  u8g2.drawStr(0,19, "Quel port ?");

  if(cote==coteJaune)u8g2.drawStr(90,19,"Jaune");
  else u8g2.drawStr(100,19,"Bleu");

  if(demandeRecalage==true)u8g2.drawStr(90,29,"Oui");
  else u8g2.drawStr(100,29,"Non");

  if(checkMenu1==1)u8g2.drawStr(50,49,"LET'S GO");
  else u8g2.drawStr(20,49,"Retour en haut");


  u8g2.drawStr(0,29,"Verif auto ?");

  u8g2.drawStr(0,39,"verification -> match");

  u8g2.sendBuffer();
}

void menu2(){
  while(quitMenu2==false){
    n1=digitalRead(PinSW);
    //Serial.println(n1);
    //Serial.println(PinSWLast);
    if (PinSWLast==LOW && n1==LOW) {   // Reset la position si on appui sur le potentiomètre
      nbClic++;
      //Serial.print(nbClic);
    }
    delay(500);
    lectureEncoder();
    ready=abs(encoderPos%3);
    //Serial.println(ready);

    u8g2.clearBuffer();         // clear the internal memory
    //  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
    u8g2.setCursor(118,64);
    u8g2.print(encoderPos);
    u8g2.setFontMode(1);  /* activate transparent font mode */
    u8g2.setDrawColor(1); /* color 1 for the box */
    u8g2.drawBox(0, 35, 128, 10);
    u8g2.setDrawColor(2);
    u8g2.drawStr(0,9, "         Mise en place");
    u8g2.drawStr(0,25, "     Pare au lancement ?");

    if(ready==0) u8g2.drawStr(0,44,"    Retour accueil");
    if(ready==1) u8g2.drawStr(0,44,"   My body is ready");
    if(ready==2) u8g2.drawStr(0,44,"Attend je check encore");

    u8g2.sendBuffer();

    if (nbClic>3){
      if(ready==1){ //prêt à envoyer
        quitMenu2=true; //sortir du menu 2
        verif=true; //tout est prêt --> sortie du menu de préparation
      }
      else if(ready==0)quitMenu2=true; //retour accueil
      nbClic=0; //reset nombre de clics
      u8g2.clearBuffer();         // clear the internal memory
    }
  }
}


void lectureEncoder(){
  n = digitalRead(PinCLK);
  if ((PinCLKLast == LOW) && (n == HIGH)) {
    Serial.println("passage1");
    if (digitalRead(PinDT) != HIGH) {
      encoderPos++;
      Serial.println("passage2");
    }
    else {
      Serial.println("hello");
      encoderPos--;
    }
  }
  PinCLKLast = n;
  delay(10);
}

void IntrerrupTimer(){
  timer++;
}
