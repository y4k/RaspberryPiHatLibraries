#include "skywriter.hpp"

Skywriter::Skywriter(asio::io_context &io) : _io{io}
{

  // Set the reset pin to an output and give it the initial value of HIGH
  pinMode(SW_RESET_PIN, OUTPUT);
  digitalWrite(SW_RESET_PIN, HIGH);

  // Set the Transfer pin to an input and configure a pull up resistor
  pinMode(SW_XFR_PIN, INPUT);
  pullUpDnControl(SW_XFR_PIN, PUD_UP);

  if (!Register(SW_XFR_PIN, INT_EDGE_RISING, this))
  {
    std::cout << "Skywriter falling edge already regsistered on pin:" << SW_XFR_PIN << std::endl;
  }
  if (!Register(SW_XFR_PIN, INT_EDGE_FALLING, this))
  {
    std::cout << "Skywriter rising edge already regsistered on pin:" << SW_XFR_PIN << std::endl;
  }
  // Setup i2c communication to the Skywriter
  if ((_i2cFileHandler = wiringPiI2CSetup(SW_ADDR_1)) == -1)
  {
#ifdef DEBUG
    std::cout << "Wiring Pi i2c setup failed:" << strerror(errno) << std::endl;
#endif
    throw errno;
  }

  reset();
}

Skywriter::~Skywriter()
{
#ifdef DEBUG
  std::cout << "Skywriter destructed" << std::endl;
#endif
}

asio::io_context &Skywriter::get_io_context() { return _io; }

void Skywriter::register_callback(SensorOutputType sensor_output,
                                  const std::function<void()> callback)
{
  switch (sensor_output)
  {
  case FLICK:
    break;
  case TOUCH:
    break;
  case TAP:
    break;
  case DOUBLE:
    break;
  case AIRWHEEL:
    break;
  case POSITION:
    break;
  }
}

void Skywriter::ISR(int pin, int edge)
{
  if (pin != SW_XFR_PIN)
  {
#ifdef DEBUG
    std::cout << "Pin:" << pin << " ISR unhandled by Skywriter" << std::endl;
#endif
    return;
  }
  if (edge == INT_EDGE_FALLING)
  {
    asio::post(_io, [this]() { start_process(); });
  }
  else if (edge == INT_EDGE_RISING)
  {
#ifdef DEBUG
    std::cout << "Line pulled high by host" << std::endl;
#endif
  }
}

void Skywriter::start_process()
{
#ifdef DEBUG
  std::cout << "Starting process" << std::endl;
#endif

  read_once();
}

void Skywriter::start_transfer()
{
#ifdef DEBUG
  std::cout << "Transfer Complete" << std::endl;
#endif

  // Take control of XFR pin
  pinMode(SW_XFR_PIN, OUTPUT);
  // Assert the pin LOW
  digitalWrite(SW_XFR_PIN, LOW);
#ifdef DEBUG
  std::cout << "XFR line is asserted low by the host" << std::endl;
#endif
}

void Skywriter::complete_transfer()
{
#ifdef DEBUG
  std::cout << "XFR line is released by the host" << std::endl;
#endif

  // Release control of transfer line
  digitalWrite(SW_XFR_PIN, HIGH);
  // Wait 300 us
  usleep(300);

  // Set the Transfer pin to an input and configure a pull up resistor
  pinMode(SW_XFR_PIN, INPUT);
  pullUpDnControl(SW_XFR_PIN, PUD_UP);
#ifdef DEBUG
  std::cout << "Transfer Complete" << std::endl;
#endif
}

void Skywriter::read_once()
{
  //if (digitalRead(SW_XFR_PIN) == LOW)
  //{
  start_transfer();

  // Read mgc3130 i2c buffer
  unsigned char message[255];
  if (read(_i2cFileHandler, message, 255) != 255)
  {
#ifdef DEBUG
    std::cout << "Failed to read message correctly!" << std::endl;
#endif
    complete_transfer();
    return;
  }

  int msgSize = message[0];
  int sequence = message[2];
  int id = message[3];

  int payloadSize = int(msgSize) - 4;

  if (id > 0)
  {
    if (payloadSize > 4)
    {
      switch (id)
      {
      case SW_MSG_SYSTEM_STATUS_ID:
        // SystemStatusMessage sys_msg =
        parse_system_status(msgSize, sequence, id, message + 4);
        break;
      case SW_MSG_SENSOR_DATA_OUTPUT_ID:
        parse_sensor_data_output(msgSize, sequence, id, message + 4);
        break;
      case SW_MSG_FW_VERSION_INFO_ID:
        parse_fw_info(msgSize, sequence, id, message + 4);
        break;
      }
    }
  }
  else
  {
    std::cout << "Unknown id:" << unsigned(id) << std::endl;
  }

  complete_transfer();
  //}
  //else
  //{
  //#ifdef DEBUG
  //      std::cout << "XFR_PIN remained high. No transfer attempted" << std::endl;
  //#endif
  //  }
}

void Skywriter::reset()
{
#ifdef DEBUG
  std::cout << "Reseting..." << std::endl;
#endif

  digitalWrite(SW_RESET_PIN, LOW);
  usleep(100000);
  digitalWrite(SW_RESET_PIN, HIGH);
  usleep(500000);

#ifdef DEBUG
  std::cout << "...Reset complete" << std::endl;
#endif
}
