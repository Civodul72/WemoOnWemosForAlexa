# WemoOnWemosForAlexa
Wemo / Bellkin emulator for Alexa on Wemos board (or ESP8266 board)\n
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
