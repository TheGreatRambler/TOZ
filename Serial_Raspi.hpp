#include <libserialport.h>

// https://github.com/rbultman/libserialport-example/blob/master/main.c
// Configuration
static const char portNameBase[] = "/dev/ttyS0";
static const int baudRate = 9600;
static const int bits = 8;
static const int stopBits = 1;
static const int parity = SP_PARITY_NONE;

struct sp_port* GetPort(void) {
   struct sp_port **portList;
   int retval;
   struct sp_port *port = NULL;
   int i;
   char * nameMatch; 
   char * portName;

   port = NULL;

   do {
      retval = sp_list_ports(&portList);

      if (retval == SP_OK) {
         nameMatch = NULL;
         for(i=0; portList[i] != NULL; i++) {
            portName = sp_get_port_name(portList[i]);
            nameMatch = strstr(portName, portNameBase);
            if (nameMatch != NULL) {
               break;
            }
         }
         if (nameMatch != NULL) {
            sp_copy_port(portList[i], &port);
         } else {
            puts("Waiting for a serial port to appear.");
            sleep(1);
         }
      }

      sp_free_port_list(portList);
   } while (port == NULL);

   return port;
}

int ConfigureSerialPort(struct sp_port *port) {
   int retval = 0;

   if (SP_OK != sp_set_baudrate(port, baudRate))
   {
      puts("Unable to set port baudrate.");
      retval = -1;
   } else if(SP_OK != sp_set_bits(port, bits)) {
      puts("Unable to set port width.");
      retval = -1;
   } else if (SP_OK !=  sp_set_parity(port, parity)) {
      puts("Unable to set port parity.");
      retval = -1;
   } else if (SP_OK != sp_set_stopbits(port, stopBits)) {
      puts("Unable to set port stop bits.");
      retval = -1;
   } else {
      puts("Port configured.");
   }

   return retval;
}

int OpenPort() {
   int retval;
   int error = 0;
   struct sp_port *port;

   do {
      port = GetPort();

      if (port == NULL) {
         puts("Did not find a suitable port.");
      } else {
         printf("Using %s\r\n", sp_get_port_name(port));
         retval = sp_open(port, SP_MODE_READ | SP_MODE_WRITE);
         if (retval == SP_OK) {
            puts("Serial port successfully opened.");

            if (ConfigureSerialPort(port) == 0) {
               //error = ReadFromPort(port);
               error = 0;
            }

            puts("Closing serial port.");
            retval = sp_close(port);
            if(retval == SP_OK) {
               puts("Serial port closed.");
            } else {
               puts("Unable to close serial port.");
            }

         } else {
            puts("Error opening serial port.");
         }
      }

      if (port != NULL) {
         sp_free_port(port);
      }
   } while (error != 0);

   return 0;
}
