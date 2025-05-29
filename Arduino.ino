#include <WiFiNINA.h>   // Uključivanje WiFiNINA biblioteke 

struct Sensor {   // Konstruiramo strukturu "Sensor"
  boolean status;   // Boolean varijabla s kojom označavmo jeli senzor uključen ili isključen
  int parametar1;   // Integer parametar1 koji se naknadno može koristit za podešavanje senzora
  int parametar2;   // Integer parametar2 koji se naknadno može koristit za podešavanje senzora
  int parametar3;   // Integer parametar3 koji se naknadno može koristit za podešavanje senzora
  int parametar4;   // Integer parametar4 koji se naknadno može koristit za podešavanje senzora
  int parametar5;   // Integer parametar5 koji se naknadno može koristit za podešavanje senzora
  int value;   // Integer "value" gdje ćemo zapisivati naša očitanja dotičnog senzora
};

char ssid[] = "Wi-FiMreza";   // Naziv Wi-Fi Mreže
char pass[] = "Lozink";   // Zaporka Wi-Fi Mreže

int status = WL_IDLE_STATUS;   // Status spajanja na Wi-Fi mrežu
char server[] = "192.168.18.29";   // IP Adresa servera
int port = 4253;   // Port servera

WiFiClient client;  //Instanciramo WiFiClient s kojim ćemo se spajati na server, Wi-Fi mrežu, slati i primati podatke
String receivedMessage;   // Instaciramo String koji ćemo koristiti za prihvat poruka
String responseMessage = "Pocetak";   // Inicijalna poruka
const int NUM_SENSORS = 10;   // Broj senzora
Sensor sensors[NUM_SENSORS];   // Kreiramo instance strukture Sensor 

void setup() {
  Serial.begin(9600);   // Standardna linija Arduino koda na početku setup() s kojom se inicijalizira serijska komunikacija na brzinu od 9600 bita u sekundi
  
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensors[i].status = false;   // Početno stavljamo sve senzore da su isključeni
  }

  sensors[0].status = true;   // Kao primjer ćemo postaviti nulti senzor da radi
  sensors[0].parametar1 = 5;   // Kao primjer ćemo postaviti parametar1 nultog senzora na vrijednost 5
  sensors[0].parametar2 = 3;   // Kao primjer ćemo postaviti parametar2 nultog senzora na vrijednost 3
  sensors[0].parametar3 = 10;  // Kao primjer ćemo postaviti parametar3 nultog senzora na vrijednost 10
  sensors[0].parametar4 = 20;  // Kao primjer ćemo postaviti parametar4 nultog senzora na vrijednost 20
  sensors[0].parametar5 = 30;  // Kao primjer ćemo postaviti parametar5 nultog senzora na vrijednost 30
  
  while (status != WL_CONNECTED) {   // Provjera da vidimo jesmo li spojeni na Wi-Fi mrežu. Ukoliko nismo, ponovno ćemo se probati spojiti
    Serial.print("Povezivanje na SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);   // Pokušaj spajanja na Wi-Fi mrežu. Vrijednost koju dobijemo spremamo u varijablu status koji kasnije provjeravamo jeli WL_CONNECTED

    delay(10000);   // Ukoliko se nismo spojili na Wi-Fi mrežu, čekamo 10 sekundi prije ponovnog pokušaja spajanja
  }
  Serial.println("Povezan na Wi-Fi mrežu");

  while (!client.connect(server, port)) {   // Pokušaj spajanja na server. Ukoliko nismo uspjeli, pokušat ćemo se ponovno spojiti
    Serial.println("Ne mogu se povezati na server. Pokušavam ponovo...");
    delay(10000);   // Ukoliko se nismo spojili na server, čekamo 10 sekundi prije ponovnog pokušaja spajanja
  }
  Serial.println("Povezan na server");

}

int handleReceivedMessage(String message, int currentSampleRate) {   // Funkcija za obradu dobivene poruke
  int newSampleRate = currentSampleRate;  // Koristimo trenutni sample rate kao početnu vrijednost
  
  if (receivedMessage.startsWith("MS - ")) {   // Provjeravamo počinje li primljena poruka sa "MS - "
      String sampleRateNew = message.substring(4);   // Primljenu poruku spremamo u novi String ali samo sadržaj od 5 znaka pa na dalje uključujući 5 znak
      sampleRateNew.trim();   // Čistimo String od bilo kakvih praznina
      int sampleRateNewInt = sampleRateNew.toInt();   // Budući da smo pročitanu poruku spremili u String, prebacujemo je u Integer
      if (sampleRateNewInt > 0) {   // Provjeravamo jeli novi sampleRate veći od 0 pošto nema smisla da je sampleRate negativan
        newSampleRate = sampleRateNewInt;   // Zapisujemo novi sampleRate
        responseMessage = "Postavljam sample rate na " + String(newSampleRate);   // Konstruiramo poruku koju šaljemo na server
        Serial.println(responseMessage);
      } else{
        responseMessage = "Neispravan sample rate";
      }
    } else {
      int sensorIndex = receivedMessage.indexOf('-');   // Instanciramo novi Integer u kojeg ćemo zapisati na kojem se mjestu nalazi znak '-'
      if (sensorIndex != -1) {   // Ako ne postoji znak '-' u našoj prihvaćenoj poruci, unutar Integera sensorIndex će pisati "-1" te onda tu provjeravamo šta je zapisano unutar nejga
        String sensorCommand = receivedMessage.substring(0, sensorIndex - 1);   // Unutar posve novog Stringa zapisujemo o kojoj je komandi riječ (SensorOn, SensorOff, SensorConfig)
                                                                                // Nju dobivamo tako što dobivenu poruku odrežemo od prvog do dijela u porucu kada dolazi znak '-'
        int sensorNumber = receivedMessage.substring(sensorIndex + 1).toInt();   // U posve novi Integer zapisujemo o kojem senzoru je riječ odnosno njegov ID

        if (sensorCommand == "SensorOn") {   // Provjeravamo komandu
          if (sensorNumber < NUM_SENSORS) {   // Provjeravamo jeli broj odnosno ID senzora manji od sveukupnog broja senzora
            sensors[sensorNumber].status = true;   // Postavljamo statusnu varijablu senzora na true čime signaliziramo da je senzor uključen
            responseMessage = "Sensor " + String(sensorNumber) + " je uključen";   // Konstruiramo poruku koju ćemo poslat na server
          } else {
            responseMessage = "Neispravan broj senzora";
          }
        } 
        
        else if (sensorCommand == "SensorOff") {   // Provjeravamo komandu
          if (sensorNumber < NUM_SENSORS) {   // Provjeravamo jeli broj odnosno ID senzora manji od sveukupnog broja senzora
            sensors[sensorNumber].status = false;   // Postavljamo statusnu varijablu senzora na false čime signaliziramo da je senzor isključen
            responseMessage = "Sensor " + String(sensorNumber) + " je isključen";   // Konstruiramo poruku koju ćemo poslat na server
          } else {
            responseMessage = "Neispravan broj senzora";
          }
        } 
        
        else if (sensorCommand == "SensorConf") {   // Provjeravamo komandu
          int startIndex = receivedMessage.indexOf(";");   // Iz dobivene poruke servera gledamo na kojoj poziciji se prvi puta nalazi znak ';'
          String numbersText = receivedMessage.substring(startIndex + 1);   // U novi String zapisujemo cijelu poruku od prvog pojavljivanja znaka ';'

          int firstNumberIndex = numbersText.indexOf(";");   // Ponovno gledamo na kojoj se poziciji prvi puta nalazi znak ';' ali ovaj put ne gledamo orginalnu poruku sa servera već poruku koju smo izrezali liniju iznad
          String number1Str = numbersText.substring(0, firstNumberIndex);   // Unutar Stringa zapisujemo broj koji se nalazi između dva znaka ';' (npr. ;2;)
          String restOfNumbers = numbersText.substring(firstNumberIndex + 1);   // Unutar Stringa zapisujemo sve nakon prvog broja i prvog znaka ';'

          int secondNumberIndex = restOfNumbers.indexOf(";"); // Pronalazimo drugi znak ';' u preostalim brojevima
          String number2Str = restOfNumbers.substring(0, secondNumberIndex); // Drugi broj
          restOfNumbers = restOfNumbers.substring(secondNumberIndex + 1); // Preostali brojevi

          int thirdNumberIndex = restOfNumbers.indexOf(";"); // Pronalazimo treći znak ';'
          String number3Str = restOfNumbers.substring(0, thirdNumberIndex); // Treći broj
          restOfNumbers = restOfNumbers.substring(thirdNumberIndex + 1); // Preostali brojevi

          int fourthNumberIndex = restOfNumbers.indexOf(";"); // Pronalazimo četvrti znak ';'
          String number4Str = restOfNumbers.substring(0, fourthNumberIndex); // Četvrti broj
          String number5Str = restOfNumbers.substring(fourthNumberIndex + 1); // Peti broj

          int number1 = number1Str.toInt();   // Dobiveni broj smo ranije zapisali u String no nama treba Integer pa ga prebacujemo u Integer
          int number2 = number2Str.toInt();   // Dobiveni broj smo ranije zapisali u String no nama treba Integer pa ga prebacujemo u Integer
          int number3 = number3Str.toInt();   // Dobiveni broj smo ranije zapisali u String no nama treba Integer pa ga prebacujemo u Integer
          int number4 = number4Str.toInt();   // Dobiveni broj smo ranije zapisali u String no nama treba Integer pa ga prebacujemo u Integer
          int number5 = number5Str.toInt();   // Dobiveni broj smo ranije zapisali u String no nama treba Integer pa ga prebacujemo u Integer

          if (sensorNumber < NUM_SENSORS) {   // Provjeravamo jeli broj odnosno ID senzora manji od sveukupnog broja senzora
            sensors[sensorNumber].status = true;   // Postavljamo statusnu varijablu senzora na true čime signaliziramo da je senzor uključen 
            sensors[sensorNumber].parametar1 = number1;   // Postavljamo paramater 1 na vrijednost koju smo ranije dobili iz poruke sa servera
            sensors[sensorNumber].parametar2 = number2;   // Postavljamo paramater 2 na vrijednost koju smo ranije dobili iz poruke sa servera
            sensors[sensorNumber].parametar3 = number3;   // Postavljamo paramater 3 na vrijednost koju smo ranije dobili iz poruke sa servera
            sensors[sensorNumber].parametar4 = number4;   // Postavljamo paramater 4 na vrijednost koju smo ranije dobili iz poruke sa servera
            sensors[sensorNumber].parametar5 = number5;   // Postavljamo paramater 5 na vrijednost koju smo ranije dobili iz poruke sa servera
            responseMessage = "Sensor " + String(sensorNumber) + " je konfiguriran s parametrima: " + String(number1) + ", " + String(number2) + ", " + String(number3) + ", " + String(number4) + ", " + String(number5);
            // Konstruiramo poruku koju ćemo poslati na server
          } else {
            responseMessage = "Neispravan broj senzora";
          }

        

        } else {
          responseMessage = "Nepoznata naredba";
        }

      } else {
        responseMessage = "Nedostaje broj senzora";
      }
    }
	
  return newSampleRate;
}

void loop() {
  static int localSampleRate = 1000;  // Inicijaliziramo statičnu varijablu
  if(client.connected()){
    if (client.available()) {    // Provjeravamo postoje li podaci za čitanje koje nam je server poslao
    receivedMessage = "";   // Instanciramo prazni String u kojem ćemo upisivati poruku koja je pristigla sa servera
    while (client.available()) {   // Provjeravamo postoje li podaci za čitanje koje nam je server poslao
      char data = client.read();   // Čitamo znak i zapisujemo ga u "data"
      if (data == '!') break;   // Čitamo znak po znak sve dok ne dođemo do znaka '!' kada prestajemo s čitanjem
      receivedMessage += data;   // Pročitani znak iz "data" zapisujemo u String "receivedMessage"
    }

    Serial.print("Received data: ");   // Ispis dobivene poruke
    Serial.println(receivedMessage);   // Ispis dobivene poruke
	// Primjeri dobivene poruke:
    // "SensorOn - 1"
    // "SensorOff - 2"
    // "SensorConf - 3;5;20;30;40;50"
    // "MS - 100"

    localSampleRate = handleReceivedMessage(receivedMessage, localSampleRate);   // Pozivamo funkciju za obradu dobivene poruke
  }

  if (responseMessage.length() > 0) {   // Provjeravamo jeli poruka koju trebamo poslati veća od 0 da ne šaljemo praznu poruku
    Serial.print("Šaljem poruku: ");   // Ispis poruke koju šaljemo
    Serial.println(responseMessage);   // Ispis poruke koju šaljemo
    client.print(responseMessage);   // Slanje poruke na server
    responseMessage = "";   // Čišćenje Stringa za slanje poruke
  }

  for(int i = 0; i < NUM_SENSORS; i++){   // Petlja u kojoj ćemo ići kroz sve moguće senzore i ispisivati njihove vrijednosti ukoliko je senzor uključen
    responseMessage = "";   // Brišemo sve iz Stringa koji koristimo za slanje poruke na server
    if(sensors[i].status == true){   // Provjeravamo jeli senzor uključen
      int randmoBroj = random(599);   // Generiramo nasumičnu vrijednost od 0 do 599 koju ćemo poslati kao "Value"
      responseMessage = "Sensor" + String(i) + ";Value-" + String(randmoBroj) + ";Parametar1-" + String(sensors[i].parametar1) + ";Parametar2-" + String(sensors[i].parametar2) + ";Parametar3-" + String(sensors[i].parametar3) + ";Parametar4-" + String(sensors[i].parametar4) + ";Parametar5-" + String(sensors[i].parametar5);
      // Konstruiramo poruku
      client.print(responseMessage);   // Šaljemo poruku na server
    }
    delay(200);   // Svaku sljedeću iteraciju unutar petlje namjerno zakašnjujemo za 200ms da se stigne poslati na server iz ranije iteracije
  }
  } else {
    Serial.println("Nisam više spojen na server");
    while (!client.connect(server, port)) {   // Pokušaj spajanja na server. Ukoliko nismo uspjeli, pokušat ćemo se ponovno spojiti
      Serial.println("Ne mogu se povezati na server. Pokušavam ponovo...");
      delay(10000);   // Ukoliko se nismo spojili na server, čekamo 10 sekundi prije ponovnog pokušaja spajanja
  }
  Serial.println("Povezan na server");
  }
  
  delay(localSampleRate);   // Naša loop petlja će se ponavljati svakih onoliko sekundi koliko je postavljen sampleRate
}
