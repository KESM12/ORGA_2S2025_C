// Pines de control
const int pinL1 = 24;
const int pinL2 = 25;
const int pinLDR = A0;
const int pinCLK = 26;
const int pinResetTime = 32;  // Añadido para resetear el contador

bool enPausa = false;
const int umbralLuz = 700;

void setup() {
  pinMode(pinL1, OUTPUT);
  pinMode(pinL2, OUTPUT);
  pinMode(pinCLK, OUTPUT);
  pinMode(pinResetTime, OUTPUT);

  digitalWrite(pinL1, LOW);
  digitalWrite(pinL2, LOW);
  digitalWrite(pinCLK, LOW);
  digitalWrite(pinResetTime, HIGH);

  Serial.begin(9600);
  Serial.println("Sistema iniciado.");
}

void loop() {
  int valorLDR = analogRead(pinLDR);

  // Detectar que alguien se cayo
  if (valorLDR < umbralLuz && !enPausa) {
    activarPausa();
  } else if (valorLDR > umbralLuz - 100 && enPausa) {
    enPausa = false;
    Serial.println("Reiniciando sistema.");
  }

  if (enPausa) {
    // TODO: Validar contraseña desde comparador y flip-flops
    // Ej: if (digitalRead(pinValidacion) == HIGH) { enPausa = false; }
    return;
  }

  // === FASE 1: L1 (15 segundos sincronizados con clock) ===
  Serial.println("=== FASE 1: Conteo Ascendente 0-15 ===");
  resetContador();            // Reiniciar contador
  digitalWrite(pinL1, HIGH);  // Encender motor L1
  Serial.println("movimiento L1");

  for (int i = 0; i < 14; i++) {
    if (verificarPausa()) return;  // Verificar pausa en cada pulso
    generarPulsoCLK();
  }

  digitalWrite(pinL1, LOW);  // Apagar motor L1
  Serial.println("fin L1");

  // === FASE 2: L2 (10 segundos sincronizados con clock) ===
  Serial.println("=== FASE 2: Conteo Descendente 10-0 ===");
  resetContador();            // Reiniciar contador
  digitalWrite(pinL2, HIGH);  // Encender motor L2
  Serial.println("movimiento L2");

  for (int i = 0; i < 10; i++) {
    if (verificarPausa()) return;  // Verificar pausa en cada pulso
    generarPulsoCLK();
  }

  digitalWrite(pinL2, LOW);  // Apagar motor L2
  Serial.println("fin L2");

  // Pausa antes de repetir
  delay(1000);
}

// Genera un pulso de 1Hz (1 segundo)
void generarPulsoCLK() {
  digitalWrite(pinCLK, HIGH);
  delayMicroseconds(100);  // Pulso corto
  digitalWrite(pinCLK, LOW);
  delay(1000);  // Espera 1 segundo
}

// Reinicia el contador a 0
void resetContador() {
  digitalWrite(pinResetTime, HIGH);  // Activar reset
  delay(10);                         // Tiempo suficiente
  digitalWrite(pinResetTime, LOW);   // Desactivar reset
}

// Verifica si se activó la pausa durante el conteo
bool verificarPausa() {
  int valorLDR = analogRead(pinLDR);
  if (valorLDR < umbralLuz && !enPausa) {
    activarPausa();
    return true;
  }
  return false;
}

void activarPausa() {
  enPausa = true;
  digitalWrite(pinL1, LOW);
  digitalWrite(pinL2, LOW);
  digitalWrite(pinResetTime, HIGH);  // Se pone a HIGH para que el reseteo del contador se active
  Serial.println("Sistema de seguridad activado.");
}