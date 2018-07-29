//
// Emulateur de prise / switch Belkin pour Amazon Alexa
// Fichiers de départ venants de https://github.com/kakopappa/arduino-esp8266-alexa-multiple-wemo-switch
// Changelog :
// Juillet 2018 : Modification Ludo Civodul pour le forum Alexa France : https://www.forumalexa.fr/
// -> Ajout d'un fichier de configuration pour le SSID et le mot de passe
// -> Ajout d'un fichier de configuration pour nommer les devices
// -> Ajout d'un "type device" pour changer le type dans l'application Alexa (interrupteur ou prise)
// -> Modification de la fonction Switch:switch pour intéger le tyoe device
// -> Ajout d'une URL et de la routine associée pour pouvoir déclencher des actions On ou Off sur un autre périphérique en réseau
// -> Ajout de la fonction Relai On/Off
// -> Affichage de toutes les requetes dans le moniteur série (brider les "serial.print()" en fonction du besoin)
// Si question, allez sur le forum https://www.forumalexa.fr/ pour en discuter

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h> // Ajout librairie pour la fonction client web (envoi d'URL)
#include <WiFiUdp.h>

#include <functional>
#include "Switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include "credentials.h"      // Fichier contenant le SSID et le mot de passe associé
#include "configuration.h"    // Fichiers de configuration des devices (noms, type et URL associés)

// prototypes
boolean connectWifi();

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *switch0 = NULL;
Switch *switch1 = NULL;
Switch *switch2 = NULL;
Switch *switch3 = NULL;

// Mise à 0 de l'état des 4 devices par défaut dans un tableau
bool isDeviceLightsOn[] = {false, false, false, false};

// Les broches ci-dessous sont pour un Wemos R1 mini
// En fonction du borad ESP8266 utilisé (Wemos / NodeMCU, etc..), il peut y avoir conflit et reboot infini au démarrage
//define NOM_PIN BROCHE_WEMOS_R1    // 
#define LEDESP    2  // A0 / ADC0   // Led Bleue ESP8266
#define RELAI3    5  // D4 / GPIO15 // Relai 3
#define RELAI2   19  // D2 / GPIO4  // Relai 2
#define RELAI1   20  // D1 / GPIO5  // Relai 1
#define BOUTON    4  // D0 / GPIO16 // Bouton

// Fonction Setup pour le démarrage
void setup()
{
  Serial.begin(115200);

  pinMode(BOUTON, INPUT_PULLUP);  // Bouton
  pinMode(LEDESP, OUTPUT);        // Led bleue
  pinMode(RELAI3, OUTPUT);        // Relai 3
  pinMode(RELAI2, OUTPUT);        // Relai 2
  pinMode(RELAI1, OUTPUT);        // Relai 1

  // Mise à zéro des broches par défaut
  digitalWrite(LEDESP, HIGH);     // Led éteinte (logique inverse)
  digitalWrite(RELAI3, LOW);      // Relai 3 éteint par défaut
  digitalWrite(RELAI2, LOW);      // Relai 2 éteint par défaut
  digitalWrite(RELAI1, LOW);      // Relai 1 éteint par défaut
     
  // Initialise la conenxion wifi
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Definition des 4 switchs
    // Ludo modif avec device_type : Format: Alexa invocation name, device_type, local port no, on callback, off callback
    switch0 = new Switch(S_Switch[0], S_device_type[0], 80, Device0LightsOn, Device0LightsOff);
    switch1 = new Switch(S_Switch[1], S_device_type[1], 81, Device1LightsOn, Device1LightsOff);
    switch2 = new Switch(S_Switch[2], S_device_type[2], 82, Device2LightsOn, Device2LightsOff);
    switch3 = new Switch(S_Switch[3], S_device_type[3], 83, Device3LightsOn, Device3LightsOff);
    
    Serial.println("Ajout des Switchs en upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*switch0);
    upnpBroadcastResponder.addDevice(*switch1);
    upnpBroadcastResponder.addDevice(*switch2);
    upnpBroadcastResponder.addDevice(*switch3);
    
  }
}

// Boucle principale
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      switch0->serverLoop();
      switch1->serverLoop();
      switch2->serverLoop();
      switch3->serverLoop();
	 }
}


// Fonction Relai et / ou Virtuel On
bool DeviceLightsOn(int x) {
    Serial.println(*S_Switch[x] + " sur On ...");
    isDeviceLightsOn[x] = true;

    if (device_virtuel[x] == true) {
      sendHttp(x, isDeviceLightsOn[x]);
    }
    if (device_physique[x] == true) {
      Relai(x, isDeviceLightsOn[x]);
    }
    return isDeviceLightsOn[x];
}

// Fonction Relai et / ou Virtuel Off
bool DeviceLightsOff(int x) {
    Serial.println(*S_Switch[x] + " sur Off ...");
    isDeviceLightsOn[x] = false;
    
    if (device_virtuel[x] == true) {
      sendHttp(x, isDeviceLightsOn[x]);
    }
    if (device_physique[x] == true) {
      Relai(x, isDeviceLightsOn[x]);
    }
    return isDeviceLightsOn[x];
}


// CallBack Device 0 On
bool Device0LightsOn() {
  DeviceLightsOn(0);
}

// CallBack Device 0 Off
bool Device0LightsOff() {
  DeviceLightsOff(0);
}

// CallBack Device 1 On
bool Device1LightsOn() {
  DeviceLightsOn(1);
}

// CallBack Device 1 Off
bool Device1LightsOff() {
  DeviceLightsOff(1);
}

// CallBack Device 2 On
bool Device2LightsOn() {
  DeviceLightsOn(2);
}

// CallBack Device 2 Off
bool Device2LightsOff() {
  DeviceLightsOff(2);
}

// CallBack Device 3 On
bool Device3LightsOn() {
  DeviceLightsOn(3);
}

// CallBack Device 3 Off
bool Device3LightsOff() {
  DeviceLightsOff(3);
}


// Fonction basculement Relai
void Relai(int device, bool etat) {
  
  if (device_relai[device] == 1) {
    digitalWrite(RELAI1, etat);      // Relai 1
    Serial.printf("*** [RELAI] 1 : %d\n", etat);
  }
  else if (device_relai[device] == 2) {
    digitalWrite(RELAI2, etat);      // Relai 2
    Serial.printf("*** [RELAI] 2 : %d\n", etat);
  }
  else if (device_relai[device] == 3) {
    digitalWrite(RELAI3, etat);      // Relai 3
    Serial.printf("*** [RELAI] 3 : %d\n", etat);
  }
  else if (device_relai[device] == 0) {
    Serial.println("*** [RELAI] Pas de relai associe...");
  }
}

// Envoi de la requête URL vers le serveur associé
void sendHttp(int device, bool etat) {
  
  String s_etat, s_URL;

  s_etat = etat ? "On" : "Off";  // Si etat = True alors s_etat = On  / si etat = False alors s_etat = Off

  // création de L'URL à envoyer
  s_URL = URL[device] + s_etat; 

  HTTPClient http;

  // Connexion sur l'URL
  Serial.println("*** [HTTP] begin...");
  http.begin(s_URL);
        
  Serial.print("*** [HTTP] GET... ");
  Serial.println(s_URL);
        
  // Envoi du HTTP header
  int httpCode = http.GET();

  // Si httpCode est négatif c'est qu'il y a eu une erreur
  if(httpCode > 0) {
    // Réponse du serveur
    Serial.printf("*** [HTTP] GET... code: %d\n", httpCode);
  } 
  else {
    // Erreur
    Serial.printf("*** [HTTP] GET... echouee, erreur: %s\n", http.errorToString(httpCode).c_str());
  }       
  http.end();
}


// Fonction Connexion au wifi
// Retourne TRUE si cela a réussi ou sinon retourne FALSE
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("\nConnexion au WiFi en cours");

  // Attente de la connexion
  while (WiFi.status() != WL_CONNECTED) {
    delay(125);
    digitalWrite(LEDESP, LOW);         // Led allumée
    Serial.print(".");

    if (i > 150){                      // 150 essais de connexion
      state = false;
      break;
    }
    delay(125);
    digitalWrite(LEDESP, HIGH);        // Led éteinte
    i++;
  }
  
  if (state){
    digitalWrite(LEDESP, LOW);         // Led allumée
    Serial.print("\nConnexion sur ");
    Serial.println(ssid);
    Serial.print("Addresse IP : ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("\nConnexion Wifi echouee.");
    digitalWrite(LEDESP, HIGH);       // Led éteinte
  }
  
  return state;
}
