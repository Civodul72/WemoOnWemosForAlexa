// Configuration des 5 périphériques (0 à 3)

// Nom des périphériques (device_name)
const char* S_Switch[] = {"salon", "salle", "entrée", "télé"};

// Ludo : Types des périphériques (device_type)
// device_type peut être lightswitch ou controllee (il existe aussi insight ou bridge sensor mais pas reconnus en natif)
const char* S_device_type[] = {"lightswitch", "lightswitch", "lightswitch", "controllee"};

// Ludo : Modes des périphériques (device_virtuel et device_physique)
// Pour piloter soit un appareil en mode web (URL), soit un relai (broche de l'ESP) soit les deux en même temps
// device_virtuel pour les périphériques utilisant une URL
const bool device_virtuel[] = {true, true, true, true};

// device_physique pour les périphériques utilisant un relai sur une broche
const bool device_physique[] = {true, true, true, false};

// affectation des Relais 1 / 2 / 3 (0 pas de relai)
const int device_relai[] = {1, 2, 3, 0};


// URL sans les On ou Off
const char* URL[] = {
  "http://XXX.XXX.XXX.XXX/monscript.php&module1=", 
  "http://XXX.XXX.XXX.XXX/monscript.php&module2=", 
  "http://XXX.XXX.XXX.XXX/monscript.php&module3=", 
  "http://XXX.XXX.XXX.XXX/monscript.php&module4=" //, 
};

