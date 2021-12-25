#include <unistd.h>

#include <iostream>
#include <string>

#include "inverter.hpp"

auto main(int argc, char *argv[]) -> int {
  int opt{};
  std::string host{};
  int port{};
  int serial_number{};
  bool influx_data{};
  while ((opt = getopt(argc, argv, ":h:p:s:i")) != EOF)
    switch (opt) {
    case 's':
      try {
        serial_number = std::stoi(optarg);
      } catch (const std::invalid_argument &) {
      }
      break;
    case 'h':
      host = optarg;
      break;
    case 'p':
      try {
        port = std::stoi(optarg);
      } catch (const std::invalid_argument &) {
      }
      break;
    case 'i':
      influx_data = true;
      break;
    }

  if (host.empty() || port == 0 || serial_number == 0) {
    std::cerr
        << "usage: omnik_inverter -s <SERIAL_NUMBER> -h <HOST> -p <PORT> [-i]\n"
        << "options:\n"
        << "-s\tinverter serial number\n"
        << "-h\tinverter host\n"
        << "-p\tinverter port\n"
        << "-i\toutput influxdb data format\n";
    exit(EXIT_FAILURE);
  }

  Inverter inverter{serial_number, host, port};
  const Metrics metrics{inverter.get_metrics()};

  if (influx_data) {
    std::cout << "energy_today=" << metrics.energy_today << ","
              << "energy_total=" << metrics.energy_total << ","
              << "input_current=" << metrics.input_current << ","
              << "input_voltage=" << metrics.input_voltage << ","
              << "output_current=" << metrics.output_current << ","
              << "output_frequency=" << metrics.output_frequency << ","
              << "output_voltage=" << metrics.output_voltage << ","
              << "power=" << metrics.power << ","
              << "temperature=" << metrics.temperature;
  } else {
    std::cout << "Serial number:\t\t" << metrics.serial_number << '\n'
              << "Power:\t\t\t" << metrics.power << '\n'
              << "Energy today:\t\t" << metrics.energy_today << '\n'
              << "Energy total:\t\t" << metrics.energy_total << '\n'
              << "Input voltage:\t\t" << metrics.input_voltage << '\n'
              << "Input current:\t\t" << metrics.input_current << '\n'
              << "Output voltage:\t\t" << metrics.output_voltage << '\n'
              << "Output current:\t\t" << metrics.output_current << '\n'
              << "Output frequency:\t" << metrics.output_frequency << '\n'
              << "Temperature:\t\t" << metrics.temperature << '\n';
  }

  exit(EXIT_SUCCESS);
}
