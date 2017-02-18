#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

struct Process {
  int id;
  int arrivalTime;
  int waitedSince;
  int burstTime;
  int priority;
};

struct Report {
  double avgWaitingTime;
  double avgTurnAroundTime;
};

auto firstArriveAtZero(const std::vector<Process> &P) {
  std::vector<Process> _P(P);
  int minArrivalTime =
      std::min_element(_P.begin(), _P.end(), [](const Process &a,
                                                const Process &b) {
        return a.arrivalTime < b.arrivalTime;
      })->arrivalTime;
  std::for_each(_P.begin(), _P.end(), [minArrivalTime](Process &x) {
    x.arrivalTime -= minArrivalTime;
  });
  return _P;
}

auto sortByArrivalTime(const std::vector<Process> &P) {
  std::vector<Process> _P(P);
  std::sort(_P.begin(), _P.end(), [](const Process &a, const Process &b) {
    return a.arrivalTime < b.arrivalTime;
  });
  return _P;
}

auto sortByArrivalTimeAndBurstTime(const std::vector<Process> &P) {
  std::vector<Process> _P(P);
  std::sort(_P.begin(), _P.end(), [](const Process &a, const Process &b) {
    return (a.arrivalTime < b.arrivalTime) ||
           ((a.arrivalTime == b.arrivalTime) && (a.burstTime < b.burstTime));
  });
  return _P;
}

void printProcess(const Process &p) {
  std::cout << p.id << " { " << p.arrivalTime << ", " << p.burstTime << ", "
            << p.priority << "}" << std::endl;
}

void printProcessList(const std::vector<Process> &P) {
  for (auto &p : P) {
    printProcess(p);
  }
}

Report runFCFS(const std::vector<Process> &P) {
  if (P.empty())
    return {0, 0};
  auto _P = sortByArrivalTimeAndBurstTime(P);
  int n = _P.size();
  int time = 0;
  int waitingTime = 0;
  int turnAroundTime = 0;
  for (auto &p : _P) {
    waitingTime += time - p.waitedSince;
    time += p.burstTime;
    turnAroundTime += time - p.waitedSince;
  }
  double avgWaitingTime = (double)waitingTime / (double)n;
  double avgTurnAroundTime = (double)turnAroundTime / (double)n;
  return {avgWaitingTime, avgTurnAroundTime};
}

Report runSJF(const std::vector<Process> &P) {
  if (P.empty())
    return {0, 0};
  auto _P = sortByArrivalTimeAndBurstTime(P);
  int n = _P.size();
  int time = 0;
  int waitingTime = 0;
  int turnAroundTime = 0;
  while (!_P.empty()) {
    // std::cout << "time " << time << std::endl;
    // printProcessList(_P);
    auto &p = _P.at(0);
    waitingTime += time - p.waitedSince;
    // std::cout << "waitedSince " << p.waitedSince << std::endl;
    if (_P.size() == 1) {
      time += p.burstTime;
      turnAroundTime += time - p.waitedSince;
      _P.erase(_P.begin());
    } else {
      auto &q = _P.at(1);
      int smallestArrivalDiff = q.arrivalTime - p.arrivalTime;
      int burstTime = p.burstTime;
      if (q.arrivalTime != p.arrivalTime && smallestArrivalDiff < burstTime) {
        burstTime = smallestArrivalDiff;
        p.burstTime -= burstTime;
        p.waitedSince += burstTime;
      } else {
        turnAroundTime += time - p.waitedSince;
        _P.erase(_P.begin());
      }
      std::for_each(_P.begin(), _P.end(), [burstTime](Process &x) {
        x.arrivalTime = std::max(0, x.arrivalTime - burstTime);
      });
      // std::cout << "burst " << burstTime << std::endl;
      // std::cout << "-------------------------" << std::endl;
      time += burstTime;
      _P = sortByArrivalTimeAndBurstTime(_P);
    }
  }
  double avgWaitingTime = (double)waitingTime / (double)n;
  double avgTurnAroundTime = (double)turnAroundTime / (double)n;
  return {avgWaitingTime, avgTurnAroundTime};
}

Report runRR(const std::vector<Process> &P, int quantum) {
  if (P.empty())
    return {0, 0};
  auto _P = std::vector<Process>(P);
  int n = _P.size();
  int time = 0;
  int waitingTime = 0;
  int turnAroundTime = 0;
  int prevId = -1;
  while (!_P.empty()) {
    for (auto &p : _P) {
      // std::cout << "time " << time << std::endl;
      // printProcessList(_P);
      if (prevId != p.id) {
        waitingTime += time - p.waitedSince;
      }
      int burstTime = std::min(quantum, p.burstTime);
      // std::cout << "burstTime " << burstTime << std::endl;
      time += burstTime;
      int bt = std::min(p.burstTime, burstTime);
      p.burstTime = std::max(0, p.burstTime - burstTime);
      // std::cout << "waitedSince = " << p.waitedSince << std::endl;
      // if (prevId != p.id) {
      if (p.burstTime <= 0) {
        // std::cout << "turnAroundTime += " << (bt) << std::endl;
        turnAroundTime += bt;
      } else {
        // std::cout << "turnAroundTime += " << (time - p.waitedSince)
                  // << std::endl;
        turnAroundTime += time - p.waitedSince;
      }
      if (p.burstTime > 0) {
        // p.waitedSince += burstTime;
        p.waitedSince = time;
      }
      // std::cout << "-------------------------" << std::endl;
      prevId = p.id;
    }
    _P.erase(std::remove_if(_P.begin(), _P.end(),
                            [](Process &x) { return x.burstTime <= 0; }),
             _P.end());
    // std::cout << "+++++++" << std::endl;
  }
  double avgWaitingTime = (double)waitingTime / (double)n;
  double avgTurnAroundTime = (double)turnAroundTime / (double)n;
  return {avgWaitingTime, avgTurnAroundTime};
}

void printReport(std::string head, const Report &rep) {
  std::cout << head << std::endl;
  std::cout << "avgWaitingTime = " << rep.avgWaitingTime << std::endl;
  std::cout << "avgTurnAroundTime = " << rep.avgTurnAroundTime << std::endl;
}

int main() {
  std::cout << "Enter processes" << std::endl;
  std::cout << "How many processes? ";
  int n;
  std::cin >> n;
  std::vector<Process> P;
  for (int i = 0; i < n; i++) {
    int j = i + 1;
    std::cout << "P" << j << ": " << std::endl;
    std::cout << "Arrival Time? ";
    int at;
    std::cin >> at;
    std::cout << "Burst Time? ";
    int bt;
    std::cin >> bt;
    P.push_back({j, at, at, bt, 0});
  }
  int qt;
  std::cout << "Time Quantum? ";
  std::cin >> qt;
  Report repFCFS = runFCFS(P);
  printReport("runFCFS", repFCFS);
  std::cout << "=======================" << std::endl;
  Report repSJF = runSJF(P);
  printReport("runSJF", repSJF);
  std::cout << "=======================" << std::endl;
  Report repRR = runRR(P, qt);
  printReport("runRR", repRR);
  // std::cout << "=======================" << std::endl;
  return 0;
}
