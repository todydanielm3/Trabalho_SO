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
int getProcessWithPid(struct Process [], int , int);

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
  clock_t beginMakeSpan;
  double makeSpan = 0.0;

  file = fopen("file.txt", "r");

  if (file == NULL)
    return 1;

  fgets(line, sizeof(line), file);
  sscanf(line, "%s %d", processes[processIndex].path, &processes[processIndex].priority);
  processes[processIndex].id = processIndex + 1;
  processes[processIndex].pid = fork();
  beginMakeSpan = clock();
  if (processes[processIndex].pid == 0) {
    strcpy(path, "./");
    strcat(path, processes[processIndex].path);
    char *args[]={path,NULL};
    execvp(args[0],args);
  } else if (processes[processIndex].pid > 0) {
    processes[processIndex].begin = clock();
    currentlyRunning = processes[processIndex].pid;
    processIndex++;
    processes[processIndex].isCompleted = false;
  }

  while (1) {
    begin = clock();
    bool checkedOneSec = false;
    int i = getProcessWithPid(processes, processIndex, currentlyRunning);
    while (1) {
      
      timeSpent = (double)(clock() - begin) / CLOCKS_PER_SEC;
      if (timeSpent >= 1.0 && !checkedOneSec) {
        if (waitpid(currentlyRunning, &processes[i].status, WNOHANG) != 0) {
          processes[i].isCompleted = true;
          processes[i].timeInSec = (double)(clock() - processes[i].begin) / CLOCKS_PER_SEC;
          break;
        }

        checkedOneSec = true;
      }
      if (timeSpent >= 2.0) {
        if (waitpid(currentlyRunning, &processes[i].status, WNOHANG) != 0) {
          processes[i].isCompleted = true;
          processes[i].timeInSec = (double)(clock() - processes[i].begin) / CLOCKS_PER_SEC;
        }

        break;
      }
    }
    if (hasAllProcessesCompleted(processes, processIndex)){
        break;
    }
      
    seconds += 2;
    if (!feof(file)) {
      fgets(line, sizeof(line), file);
      sscanf(line, "%s %d", processes[processIndex].path, &processes[processIndex].priority);
      processes[processIndex].begin = clock();
      processes[processIndex].id = processIndex + 1;
      processIndex++;
    }

    if (seconds % 6 == 0) {
      kill(currentlyRunning, SIGTSTP);
      chosenProcess = lotteryScheduler(processes, processIndex); // returns Process
      int index = getProcessIndex(processes, processIndex, chosenProcess);
      if (hasPID(processes, processIndex, chosenProcess)) {
        kill(chosenProcess.pid, SIGCONT);
      } else {
        processes[index].pid = fork();
        if (processes[index].pid == 0) {
          strcpy(path, "./");
          strcat(path, processes[processIndex].path);
          char *args[]={path,NULL};
          execvp(args[0],args);
        } else if (processes[index].pid > 0) {
          currentlyRunning = processes[index].pid;
        }
      }
    }
  }

  makeSpan = (double)(clock() - beginMakeSpan) / CLOCKS_PER_SEC;
  printf("MakeSpan: %f seconds\n", makeSpan);

  printAllProcessesTimes(processes, processIndex);

  fclose(file);

  return 0;
}

void printAllProcessesTimes(struct Process processes[], int qty) {
  for (int i = 0; i < qty; i++)
    printf("Process %d took %f seconds\n", processes[i].id, processes[i].timeInSec);
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
  for (int i = 0; i < totalTickets; i++) {
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
  int i = 0;

  for (i = 0; i < qty; i++) 
    if (!processes[i].isCompleted)
      break;

  if (i == (qty - 1))
    return false;

  return true;
}

int getProcessIndex(struct Process processes[], int qty, struct Process process) {
  int i = 0;
  for (i = 0; i < qty; i++)
    if (processes[i].id == process.id)
      break;

  return i;
}

int getProcessWithPid(struct Process processes[], int qty, int pid){
  int i = 0;
  for (i = 0; i < qty; i++)
    if (processes[i].pid == pid)
      break;

  return i;
}
