/**
   PINS
*/
#define CLOCK_PIN               18
#define RW_PIN                  39

#define ADDRESS_LEN             16
#define DATA_LEN                8
#define DEBOUNCE_TH             200   // ms

typedef enum {
  READ = LOW,
  WRITE = HIGH
} mem_access_t;


//                                             LSB                                                        MSB
static const byte address_pins[ADDRESS_LEN] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52};
static const byte data_pins[DATA_LEN]       = {23, 25, 27, 29, 31, 33, 35, 37};

volatile byte manual_clock = LOW;
volatile byte tick        = LOW;

void setup() {

  // CLOCK INPUT INTERRUPT
  pinMode(CLOCK_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLOCK_PIN), clock_tick_ISR, RISING);

  // MEMORY READ WRITE MODE INPUT
  pinMode(RW_PIN, INPUT);

  // INIT ADDRESS PINS
  for (int i = 0; i < ADDRESS_LEN; i++) {
    pinMode(address_pins[i], INPUT);
  }

  // INIT DATA PINS
  for (int i = 0; i < DATA_LEN; i++) {
    pinMode(data_pins[i], INPUT);
  }

  // INIT SERIAL
  Serial.begin(115200);
  while (!Serial) ;     // wait for serial port to connect. Needed for native USB port only
}


void loop() {
}


/**
   INTERRUPT HANDLERS
*/
void clock_tick_ISR() {
  uint16_t address = read_address();
  byte data = read_data();
  mem_access_t rw = read_rw();

  static char buff[20];
  sprintf(buff, "%04X   %c   %02X", address, rw == READ ? 'R' : 'W', data);
  Serial.println(buff);
}


/**
   UTIL FUNCTIONS
*/
static uint16_t read_address() {
  uint16_t res = 0x0000;
  for (int i = 0; i < ADDRESS_LEN; i++) {
    res |= digitalRead(address_pins[i]) << i;
  }

  return res;
}


static byte read_data() {
  byte res = 0x00;
  for (int i = 0; i < DATA_LEN; i++) {
    res |= digitalRead(data_pins[i]) << i;
  }

  return res;
}


static mem_access_t read_rw() {
  return digitalRead(RW_PIN) ? READ : WRITE;
}
