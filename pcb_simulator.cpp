#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>
#include <set>

// Define the PCB structure
struct PCB {
    int pid;           // Process ID
    std::string state; // Process state: "Ready", "Running", "Terminated"
    int pc;            // Program counter
    int total_work;    // Total work units needed

    // Constructor
    PCB(int process_id, int work_units)
        : pid(process_id), state("Ready"), pc(0), total_work(work_units) {}
};

// Print states of all processes, sorted by PID (PROVIDED - DO NOT MODIFY)
void printProcessStates(const std::vector<PCB>& pcbs, int timeSlice) {
    std::cout << "Interrupt " << timeSlice << ":" << std::endl;
    std::vector<PCB> sorted_pcbs = pcbs;
    std::sort(sorted_pcbs.begin(), sorted_pcbs.end(),
              [](const PCB& a, const PCB& b) { return a.pid < b.pid; });
    for (const PCB& pcb : sorted_pcbs) {
        std::cout << "PID " << pcb.pid << ": " << pcb.state  << ", at pc " << pcb.pc << std::endl;
    }
    std::cout << std::flush;
}

// Helper function to check if all processes are terminated
bool allProcessesTerminated(const std::vector<PCB>& pcbs) {
    for (const PCB& pcb : pcbs) {
        if (pcb.state != "Terminated") {
            return false;
        }
    }
    return true;
}

// Kernel simulator implementing round-robin scheduling
void kernelSimulator(std::vector<PCB>& pcbs, int timeQuantum) {
    // Initialize ready queue with all processes (by index in pcbs vector)
    std::queue<int> readyQueue;
    for (int i = 0; i < pcbs.size(); i++) {
        readyQueue.push(i);
    }

    int currentTime = 0;

    // Continue until all processes are terminated
    while (!allProcessesTerminated(pcbs)) {
        currentTime++;

        // Get the next process from ready queue if available
        if (!readyQueue.empty()) {
            int currentProcessIndex = readyQueue.front();
            readyQueue.pop();

            PCB& currentProcess = pcbs[currentProcessIndex];

            // Set current process to Running state
            currentProcess.state = "Running";

            // Execute for time quantum (2 units) or until completion
            int workDone = std::min(timeQuantum, currentProcess.total_work - currentProcess.pc);
            currentProcess.pc += workDone;

            // Check if process is completed and update state accordingly BEFORE printing
            if (currentProcess.pc >= currentProcess.total_work) {
                // Process completed - set to Terminated
                currentProcess.state = "Terminated";
            } else {
                // Process not completed - keep as Running for printing, then set to Ready
                // (state will be updated after printing)
            }

            // Print current state
            printProcessStates(pcbs, currentTime);

            // After printing, if process was not completed, set to Ready and add back to queue
            if (currentProcess.pc < currentProcess.total_work) {
                currentProcess.state = "Ready";
                readyQueue.push(currentProcessIndex);
            }
        } else {
            // No processes in ready queue but some might still be running
            // This shouldn't happen in our simple CPU-bound simulation
            printProcessStates(pcbs, currentTime);
        }
    }
}

int main() {
    // Read from standard input via redirection (PROVIDED)
    int numProcesses;
    if (!(std::cin >> numProcesses)) {
        std::cerr << "Error: Invalid input for number of processes" << std::endl;
        return 1;
    }
    if (numProcesses <= 0) {
        std::cerr << "Error: Invalid number of processes" << std::endl;
        return 1;
    }

    std::vector<PCB> pcbs;
    std::set<int> pids; // Track unique PIDs
    for (int i = 0; i < numProcesses; ++i) {
        int pid, work;
        if (!(std::cin >> pid >> work)) {
            std::cerr << "Error: Invalid input format for process data" << std::endl;
            return 1;
        }
        if (work <= 0) {
            std::cerr << "Error: Invalid work units for PID " << pid << std::endl;
            return 1;
        }
        if (pids.count(pid) > 0) {
            std::cerr << "Error: Duplicate PID " << pid << " detected" << std::endl;
            return 1;
        }

        // Add PID to set to track uniqueness
        pids.insert(pid);

        // Create PCB and add to pcbs vector
        pcbs.emplace_back(pid, work);
    }

    int timeQuantum = 2;
    kernelSimulator(pcbs, timeQuantum);

    std::cout << "All processes completed." << std::endl;
    return 0;
}
