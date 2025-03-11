#pragma once

class CpuData {
public:
    unsigned long long user, nice, system, idle, iowait, irq, softirq,
        steal;
};

class CpuUsage {

private:

public:
  CpuData get_cpu_data();

  double calculate_cpu_usage(const CpuData &prev, const CpuData &curr);

};
