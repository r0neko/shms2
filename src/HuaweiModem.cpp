#include "HuaweiModem.h"

HuaweiModem::HuaweiModem(std::string cmd_port, std::string data_port) {
  this->fd = open(cmd_port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
  this->fdAudio = open(data_port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

  if(this->fd < 0) {
    this->hasModem = 0;
    return;
  }

  if(this->fdAudio < 0) {
    this->hasModem = 0;
    return;
  }

  this->set_interface_attribs(this->fd, B9600, 0);
  this->set_interface_attribs(this->fdAudio, B9600, 0);

  this->executeCommandUntil("ATE0", "OK");
  this->executeCommand("AT+CMEE=2");
}

int HuaweiModem::set_interface_attribs(int fd, int speed, int parity) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0)
  {
    printf("error %d from tcgetattr\n", errno);
    return -1;
  }
  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
  // disable IGNBRK for mismatched speed tests; otherwise received break
  // as \000 chars
  tty.c_iflag &= ~IGNBRK;         // disable break processing
  tty.c_lflag = 0;                // no signaling chars, no echo,
  // no canonical processing
  tty.c_oflag = 0;                // no remapping, no delays
  tty.c_cc[VMIN]  = 0;            // read doesn't block
  tty.c_cc[VTIME] = 0;            // 0 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
  tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
  tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr (this->fd, TCSANOW, &tty) != 0)
  {
    printf("error %d from tcsetattr\n", errno);
    return -1;
  }
  if (tcsetattr (this->fdAudio, TCSANOW, &tty) != 0)
  {
    printf("error %d from tcsetattr\n", errno);
    return -1;
  }
  return 0;
}

std::string HuaweiModem::readPCUIBlocking() {
  std::string data;

  char buf[4];
  int x = 0;
  while(x == 0) {
    x = read(this->fd, buf, 4);
  }
  data.append(buf, x);
  while((x = read(this->fd, buf, 4))) {
    data.append(buf, x);
  }

  return data;
}

std::string HuaweiModem::readPCUI() {
  std::string data;

  char buf[4];
  int x = 0;

  while((x = read(this->fd, buf, 4))) {
    data.append(buf, x);
  }

  return data;
}

std::string HuaweiModem::readPCUIUntil(std::string until) {
  std::string response;

  while(1) {
    std::string resp = this->readPCUI();
    std::istringstream response_stream(resp);

    while(std::getline(response_stream, response, '\n')) { 
      if(response.compare(0, until.length(), until) == 0) break;
    }

    if(response.compare(0, until.length(), until) == 0) break;
  }
  
  return response;
}

void HuaweiModem::flush() {
  tcflush(this->fd, TCIOFLUSH);
}

int HuaweiModem::writePCUI(std::string command) {
  return write(this->fd, command.c_str(), command.length());
}

std::string HuaweiModem::executeCommand(std::string command) {
  this->writePCUI(command);
  this->writePCUI("\r\n");
  this->flush();

  std::string resp = this->readPCUIBlocking();
  std::istringstream response_stream(resp);
  std::string response;

  while(std::getline(response_stream, response, '\n')) {
    if(response.compare("\r") != 0) break;
  }
  
  return response;
}

std::string HuaweiModem::executeCommandUntil(std::string command, std::string until) {
  this->writePCUI(command);
  this->writePCUI("\r\n");
  this->flush();

  std::string response;
  while(1) {
    std::string resp = this->readPCUI();
    std::istringstream response_stream(resp);

    while(std::getline(response_stream, response, '\n')) { 
      if(response.compare(0, until.length(), until) == 0) break;
    }

    if(response.compare(0, until.length(), until) == 0) break;
  }
  
  return response;
}

int HuaweiModem::sendSMS(std::string destination, std::string text) {
  this->writePCUI("AT+CMGF=1\r\n");
  this->flush();
  this->writePCUI("AT+CMGS=\"");
  this->writePCUI(destination);
  this->writePCUI("\"\r\n");
  this->flush();
  this->writePCUI(text);
  this->writePCUI("\x1A");
  this->flush();
  std::string rez = this->readPCUIUntil("+");
  if(rez.compare(0, 5, "+CMGS") == 0) return 0;
  return 1;
}

// call status 1 = hanged up
// call status 2 = answered
// call status 3 = started
// call status 0 = unknown

int HuaweiModem::voiceCall(std::string destination) {
  this->writePCUI("ATD");
  this->writePCUI(destination);
  this->writePCUI(";\r\n");

  int call_status = 0;
  std::string response;

  while(1) {
    std::string r = this->readPCUIUntil("^");
    std::istringstream r_stream(r);

    while(std::getline(r_stream, response, '^')) { 
      if(response.length() >= 4) break;
    }

    if(response.compare(0, 4, "ORIG") == 0) call_status = 3;
    if(response.compare(0, 4, "CEND") == 0) {
      call_status = 1;
      break;
    }
    if(response.compare(0, 4, "CONN") == 0) {
      call_status = 2;
      this->writePCUI("AT^DDSETEX=2\r\n");
      this->flush();
      break;
    }
  }

  return call_status;
}

void HuaweiModem::playFile(std::string location) {
  int bytes_read = 0, bytes_written = 0;

  FILE *file = fopen(location.c_str(), "rb");

  if (file == NULL)
  {
    return;
  }

  fseek(file, 0, SEEK_END);
  int file_length = ftell(file);
  rewind(file);

  int silent_frame = 320 - (file_length % 320);
  int data_len = file_length + silent_frame;

  char buffer[320];

  std::string dev;

  while((bytes_read = fread(buffer, 1, 320, file))) {
    dev = this->readPCUI();
    if(dev.length() > 0) {
      if(dev.compare(0, 7, "\r\n^CEND") == 0) {
        return;
      }
    }
    bytes_written = write(this->fdAudio, buffer, bytes_read);
    usleep(20000);
  }

  this->writePCUI("AT+CHUP\r\n");
  this->flush();
}