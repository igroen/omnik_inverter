#ifndef INVERTER_HPP
#define INVERTER_HPP
#include <array>
#include <string>

struct Metrics {
  std::string serial_number{};
  int power{};
  double energy_today{};
  double energy_total{};
  double input_voltage{};
  double input_current{};
  double output_voltage{};
  double output_current{};
  double output_frequency{};
  double temperature{};
};

class Inverter {
private:
  std::string m_host;
  int m_port;
  int m_serial_number;

  auto create_payload(const int serial_number) -> std::array<unsigned char, 16>;
  auto retrieve_data(const std::array<unsigned char, 16> payload)
      -> std::array<unsigned char, 256>;
  auto get_serial_number(const std::array<unsigned char, 256> &response_data)
      -> std::string;
  auto get_power(const std::array<unsigned char, 256> &response_data) -> int;
  auto get_energy_today(const std::array<unsigned char, 256> &response_data)
      -> double;
  auto get_energy_total(const std::array<unsigned char, 256> &response_data)
      -> double;
  auto get_input_voltage(const std::array<unsigned char, 256> &response_data)
      -> double;
  auto get_input_current(const std::array<unsigned char, 256> &response_data)
      -> double;
  auto get_output_voltage(const std::array<unsigned char, 256> &response_data)
      -> double;
  auto get_output_current(const std::array<unsigned char, 256> &response_data)
      -> double;
  auto get_output_frequency(const std::array<unsigned char, 256> &response_data)
      -> double;
  auto get_temperature(const std::array<unsigned char, 256> &response_data)
      -> double;

public:
  Inverter(int serial_number, std::string host, int port);
  auto get_metrics() -> Metrics;
};

#endif
