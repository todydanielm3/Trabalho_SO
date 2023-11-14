#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

struct Process {
  int id;
  int pid;
  char path[100];
  clock_t begin;
  double timeInSec;
  int priority;
  int status;
  bool isCompleted;
};

bool hasPID(struct Process [], int, struct Process);
struct Process lotteryScheduler(struct Process [], int);
bool hasAllProcessesCompleted(struct Process [], int);
int getProcessIndex(struct Process [], int, struct Process);
void printAllProcessesTimes(struct Process [], int);
int getProcessIndexWithPID(struct Process [], int , int);

int main () {
  FILE *file;
  char line[100];
  char path[100];
  int seconds = 0;
  clock_t begin;
  double timeSpent = 0.0;
  struct Process processes[100];
  int processIndex = 0;
  int currentlyRunning;
  struct Process chosenProcess;

  file = fopen("input.txt", "r");

  if (file == NULL)
    return 1;

  fgets(line, sizeof(line), file);
  sscanf(line, "%s %d", processes[processIndex].path, &processes[processIndex].priority);
  processes[processIndex].id = processIndex + 1;
  processes[processIndex].pid = fork();
  if (processes[processIndex].pid == 0) {
    execl(processes[processIndex].path, processes[processIndex].path, NULL);
  } else if (processes[processIndex].pid > 0) {
    processes[processIndex].begin = clock();
    currentlyRunning = processes[processIndex].pid;
    processIndex++;
    processes[processIndex].isCompleted = false;
  }

  while (1) {
    begin = clock();
    bool checkedOneSec = false;
    int i = getProcessIndexWithPID(processes, processIndex, currentlyRunning);

    do {
      timeSpent = (double)(clock() - begin) / CLOCKS_PER_SEC;

      if (timeSpent >= 1.0 && !checkedOneSec) {
        if (waitpid(currentlyRunning, &processes[i].status, WNOHANG) != 0) {
          processes[i].isCompleted = true;
          processes[i].timeInSec = (double)(clock() - processes[i].begin) / CLOCKS_PER_SEC;
        }

        checkedOneSec = true;
      }

      if (timeSpent >= 2.0 && waitpid(currentlyRunning, &processes[i].status, WNOHANG) != 0) {
        processes[i].isCompleted = true;
        processes[i].timeInSec = (double)(clock() - processes[i].begin) / CLOCKS_PER_SEC;
      }
    } while (timeSpent < 2.0);
    
    seconds += 2;
    if (hasAllProcessesCompleted(processes, processIndex))
      break;
    
    if (!feof(file)) {
      fgets(line, sizeof(line), file);
      sscanf(line, "%s %d", processes[processIndex].path, &processes[processIndex].priority);
      processes[processIndex].begin = clock();
      processes[processIndex].id = processIndex + 1;
      processIndex++;
    }

    if (seconds % 6 == 0) {
      int index = getProcessIndexWithPID(processes, processIndex, currentlyRunning); 
      if (waitpid(currentlyRunning, &processes[index].status, WNOHANG) != 0) {
        processes[index].isCompleted = true;
        processes[index].timeInSec = (double)(clock() - processes[index].begin) / CLOCKS_PER_SEC;
      }

      kill(currentlyRunning, SIGTSTP);
      chosenProcess = lotteryScheduler(processes, processIndex);
      index = getProcessIndex(processes, processIndex, chosenProcess);
      if (hasPID(processes, processIndex, chosenProcess)) {
        kill(processes[index].pid, SIGCONT);
        currentlyRunning = processes[index].pid;
      } else {
        processes[index].pid = fork();
        if (processes[index].pid == 0) {
          execl(processes[index].path, processes[index].path, NULL);
        } else if (processes[index].pid > 0) {
          currentlyRunning = processes[index].pid;
        }
      }
    }
  }

  printf("MakeSpan: %d seconds\n", seconds);
  printAllProcessesTimes(processes, processIndex);

  fclose(file);

  return 0;
}

void printAllProcessesTimes(struct Process processes[], int qty) {
  for (int i = 0; i < qty; i++)
    printf("Process %d took %.0f\n", processes[i].id, processes[i].timeInSec);
}

bool hasPID(struct Process processes[], int qty, struct Process chosenProcess) {
  for (int i = 0; i < qty; i++) {
    if (processes[i].id == chosenProcess.id && chosenProcess.pid > 0) {
      return true;
      break;
    }
  }

  return false;
}

struct Process lotteryScheduler(struct Process processes[], int qty) {
  int totalTickets = 0;
  for (int i = 0; i < qty; i++) {
    if (processes[i].isCompleted) continue;
    totalTickets += processes[i].priority + 1;
  }

  int ticket = (rand() % totalTickets) + 1;

  struct Process chosenProcess;
  for (int i = 0; i < qty; i++) {
    if (processes[i].isCompleted) continue;

    ticket -= processes[i].priority + 1;
    if (ticket <= 0) {
      chosenProcess = processes[i];
      break;
    }
  }

  return chosenProcess;
}

bool hasAllProcessesCompleted(struct Process processes[], int qty) {
  for (int i = 0; i < qty; i++) 
    if (!processes[i].isCompleted)
      return false;

  return true;
}

int getProcessIndex(struct Process processes[], int qty, struct Process process) {
  for (int i = 0; i < qty; i++)
    if (processes[i].id == process.id)
      return i;

  return 0;
}

int getProcessIndexWithPID(struct Process processes[], int qty, int pid){
  for (int i = 0; i < qty; i++)
    if (processes[i].pid == pid)
      return i;

  return 0;
}
