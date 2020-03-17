#include <STRATEGIET.h>



bool detection=true;

int intdeplakment[18][3],k=0;

float deplakment[18][3];

byte Trame[4],envoye=0,com=0;

// ---------------------------------------------------------------------------
// Un programme Arduino doit impérativement contenir la fonction "setup"
// Elle ne sera exécuter une seule fois au démarrage du microcontroleur
// Elle sert à configurer globalement les entrées sorties
// ---------------------------------------------------------------------------
void setup()
{
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

  // Ouvre le port série et fixe le debit de communication à 9600 bauds
  Serial.begin(9600);

  // Initialise la library Wire et se connecte au bus I2C en tant que maître
  Wire.begin();

  // Test si la Teensy fonctionne en allumant sa LED
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

  delay(10000);


}

// Création d'une Fonction dont l'objectif est de :
//  - lire la valeur analogique du potentiomètre dont l'adresse est spécifiée via le
//  paramètre *potentiometerPin*
//
//  - la mettre à l'échelle, car dans cet exemple, on souhaite avoir une plage de
// valeurs entre 0 et 255
//
//  - tester si cette valeur est différente de la précédente; ce qui est possible
// avec le paramètre *previousValue* (qui est passé par référence)
//
//  - si et seulement si c'est le cas, on l'envoie sur le bus I2C, pour l'esclave
// dont l'adresse est spécifiée via *slaveAddress*
//
// La subtilité de cette méthode est le passage par adresse (plutôt que par valeur)
// du paramètre previousValue, qui permet de lire/modifier la variable en dehors
// de cette méthode. // Cela est indispensable pour pouvoir sauvegarder la valeur
// lue via le potentiomètre et pouvoir la comparer avec les valeurs suivantes.

/*
 * Creation de la fonction : readRegisterAndSendValue
 *
 */




// ---------------------------------------------------------------------------
// Le programme principal s’exécute par une boucle infinie appelée Loop ()
// ---------------------------------------------------------------------------

void loop()
{

  //lecture si le robot a fini de se déplacer
  Wire.requestFrom(I2C_SLAVE_DEPLACEMENT_ADDRESS,1);
  com=Wire.read();
  Serial.println(com);

  if (com==0){ //si il a fini...
    if(k==0)readRegisterAndSendValue();//si on est au tout début--> position initiale
    envoye=1;
    k++;
    readRegisterAndSendValue();//envoi prochaine position

  }

    delay(1000);
//  }


}


void readRegisterAndSendValue() {

  int q=0;

    // Envoie de la valeur à l'esclave I2C.
    // Principe et description :
    // 1 : Commencez une transmission vers la carte esclave I2C avec l'adresse donnée.
    // 2 : Ensuite, mettez en file d'attente les octets à transmettre avec la fonction write ()
    // 3 : transmettez-les en appelant la fonction endTransmission () .

    // débute de la communication avec un esclave (ouvre le stockage données à envoyer avec write)



    deplakment[k][3]*=100; // Pour gagner en précision --> /100 dans le programme déplacement

    intdeplakment[k][1]=deplakment[k][1];
    intdeplakment[k][2]=deplakment[k][2];
    intdeplakment[k][3]=deplakment[k][3];


    //Chargement des octets à partir du tableau de déplacement pour une transmission du composant maître vers un composant esclave
    // Rq : la trame 0 est une trame rassemblant les bits restants pour x y et turn

    Trame[0]=((intdeplakment[k][3]&=(1<<8))>>1);
    Trame[0]|=((intdeplakment[k][1]&=(0x700))>>4);
    Trame[0]|=((intdeplakment[k][2]&=(0xF00))>>8);
    Trame[1]=deplakment[k][1];
    Trame[2]=deplakment[k][2];
    Trame[3]=deplakment[k][3];

    Wire.beginTransmission(I2C_SLAVE_DEPLACEMENT_ADDRESS);
    for (int i=0; i<4; i++){
        Wire.write(Trame[i]);

        Serial.print("Trame");
        Serial.println(Trame[i]);
        q++;
        Serial.print("q");
        Serial.println(q);
      }

  Wire.endTransmission();

}
