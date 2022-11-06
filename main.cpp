#include "mbed.h"

static BufferedSerial device1(D10, D9); // tx, rx  D10:tx  D9:rx
static BufferedSerial device2(D1, D0);  // tx, rx   D1:tx   D0:rx
//static BufferedSerial serial_port(USBTX, USBRX);

Thread slave_thread;
Thread master_thread;

void master() {
    char buf[2] = {0,10};
    for (int i = 0; i < 3; i++){
        buf[0] = i;
        device1.write(&buf[0], 1);
        ThisThread::sleep_for(10ms);// wait 10ms before sending next command
        device1.write(&buf[1], 1);
        ThisThread::sleep_for(100ms);// wait for slave printf debug
        printf("[master] Written to slave: mode =%d, value = %d\n", buf[0], buf[1]);
        ThisThread::sleep_for(100ms);
    }
}

void slave(){
    char buf[5];
    char msg[2];
    int mode; 
    int value;

    while (1) {
    if (device2.size()) { // with at least 1 char
      device2.read(buf, 1);
      printf("Condition check\n");
      mode = buf[0];
      if (device2.size()) { // with at least 1 char
        device2.read(buf, 1);
        value = buf[0];
        printf("Slave Read: mode=%d, value=%d\n", mode, value);

        if (device2.writable()) {
          if (mode == 1) { // mode!=0
            value = value + 1;
            msg[0] = mode;
            msg[1] = value;
            printf("Slave Write: mode=%d, value=%d\n", mode, value);
            device2.write(msg, 2);
          } else if (mode == 2) {
            value = value + 2;
            msg[0] = mode;
            msg[1] = value;
            printf("Slave Write: mode=%d, value=%d\n", mode, value);
            device2.write(msg, 2);
          } else {
            msg[0] = mode;
            msg[1] = value;
            printf("Slave Write: mode=%d, value=%d\n", mode, value);
            device2.write(msg, 2);
          }
          ThisThread::sleep_for(100ms); //wait after write
        }
      }
    }
    else{
        printf("Failed to write to slave\n");
        break;
    }
  }
}

int main(){
     // Set desired properties (9600-8-N-1).
  device1.set_baud(9600);
  device1.set_format(
      /* bits */ 8,
      /* parity */ BufferedSerial::None,
      /* stop bit */ 1);

  // Set desired properties (9600-8-N-1).
  device2.set_baud(9600);
  device2.set_format(
      /* bits */ 8,
      /* parity */ BufferedSerial::None,
      /* stop bit */ 1);

  master_thread.start(master);
  slave_thread.start(slave);
}