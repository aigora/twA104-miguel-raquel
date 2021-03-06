#include <stdio.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

constexpr auto N = 8;		//Cantidad de digitos que poseen las tarjetas
constexpr auto RST_PIN = 9; //Pin de reset del sensor RFID
constexpr auto SS_PIN = 10; //Pin de SS (SDA) del sensor RFID;

MFRC522 detector(SS_PIN, RST_PIN);
Servo spuerta;
const int ledverde = 4;
const int ledrojo = 2;
int contador = 0, a = 0, b = 1;

byte UIDtarjeta[N]; //Almacenar el codigo del Tag leido
byte password[N] = { 0x60, 0x66, 0x2B, 0xA5 }; //codigo de la tarjeta blanca

void setup() {

	Serial.begin(9600);
	SPI.begin();
	spuerta.attach(6);
	detector.PCD_Init();

	pinMode(ledverde, OUTPUT);
	pinMode(ledrojo, OUTPUT);
	Serial.println("Pase la tarjeta por detector...");
	spuerta.write(0);
}

void loop() {
	b = 1;
	if (detector.PICC_IsNewCardPresent()) //Detecta cuando pasamos una tarjeta por el lector 
	{

		if (detector.PICC_ReadCardSerial()) //Lee la tarjeta 
		{
			Serial.print("Card UID:"); // Envia su UID de forma serial 
			for (byte i = 0; i < detector.uid.size; i++) { // Bucle for para leer y guardar el codigo de la tarjeta
				Serial.print(detector.uid.uidByte[i] < 0x10 ? " 0" : " ");
				Serial.print(detector.uid.uidByte[i], HEX);
				UIDtarjeta[i] = detector.uid.uidByte[i];
			}
			Serial.print("     ");

			if (compare(UIDtarjeta, password)) { //comparamos los UID para determinar si es la tarjeta correcta
			  //Al devolver TRUE
				if (a == 0) {


					Serial.println("Acceso concedido..."); //Abre la puerta
					parpadeo(ledverde);
					abrir();
					parpadeo(ledverde);
					contador = 0;
					a = 1;
					b = 0;
				}
				if (a == 1 && b == 1) {						//Cierra la puerta
					parpadeo(ledverde);
					cerrar();
					parpadeo(ledrojo);
					contador = 0;
					a = 0;
				}
			}

			else {
				//Si la funcion devuelve False
				Serial.println("Acceso denegado...");
				parpadeo(ledrojo);
				parpadeo(ledrojo);
				contador++;
				if (contador == 3) {		//Si falla luego de 3 intentos la caja se bloquea 1 minuto
					digitalWrite(ledrojo, HIGH);
					delay(60000);
					digitalWrite(ledrojo, LOW);
				}
			}
			detector.PICC_HaltA(); // Termina la lectura        
		}//Fin de la lectura    
	}//Fin del detector

}//Fin del loop

boolean compare(byte tarjeta[], byte password[]) //Funcion que compara el codigo de la tarjeta con la guardada en la memoria
{
	for (int j = 0; j < 4; j++) {
		if (tarjeta[j] != password[j])return(false);
	}
	return(true);
}

void abrir() {	//Funcion que abre la puerta
	for (int pos = 0; pos <= 95; pos += 1) {
		spuerta.write(pos);
		delay(15);
	}

}

void cerrar() {	//Funcion que cierra la puerda
	for (int pos = 95; pos >= 0; pos -= 1) {
		spuerta.write(pos);
		delay(15);
	}
}

void parpadeo(int led) { //Funci�n para que el led haga un blink o parpadeo
	delay(200);
	digitalWrite(led, HIGH);
	delay(200);
	digitalWrite(led, LOW);

}