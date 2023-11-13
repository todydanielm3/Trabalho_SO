#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

void escalonador() {
  // Implemente o escalonador de loteria aqui
  // Esta função será chamada quando necessário
  // Certifique-se de adicionar a lógica de escalonamento
  // e seleção aleatória de processos com base nos tickets
}

int main() {
  FILE *arquivo;
  char linha[100];
  int segundos = 0;
  pid_t pid;

  arquivo = fopen("arquivo_de_entrada.txt", "r"); // Abra o arquivo de entrada

  if (arquivo == NULL) {
    perror("Erro ao abrir o arquivo");
    exit(1);
  }

  while (fgets(linha, sizeof(linha), arquivo)) {
    // Leitura de informações do arquivo (nome e prioridade)
    char nome[50];
    int prioridade;

    sscanf(linha, "%s %d", nome, &prioridade);

    // Adicionar processo à fila ready

    // Contagem de tempo
    segundos += 2;

    if (segundos == 6) {
      // Chamar o escalonador e parar o processo atual
      pid = getpid();
      kill(pid, SIGTSTP);
      escalonador();
      pid = fork();

      if (pid == 0) {
        // Processo filho executa o arquivo
        execl("seu_programa", "seu_programa", (char *)NULL);
      }
    }
  }

  fclose(arquivo);

  return 0;
}

/* IMPROVING TEMPLATE
struct Process {
  int id;
  int pid;
  char path[100];
  clock_t begin;
  double timeInSec;
  int priority;
  int state;
  bool isCompleted;
}

bool hasPID(Process [], int, Process);
Process lotteryScheduler(Process [], int);
bool hasAllProcessesCompleted(Process [], int);
int getProcessIndex(Process [], int, Process);
void printAllProcessesTimes(Process [], int);

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
  int chosenProcess;
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
    path = strcat("./", processes[processIndex].path);
    execvp(path, path, NULL);
  } else if (processes[processIndex].pid > 0) {
    processes[processIndex].begin = clock();
    currentlyRunning = processes[processIndex].pid;
    processIndex++;
    processes[processIndex].isCompleted = false;
  }

  while (true) {
    if (hasAllProcessesCompleted(processes, processIndex))
      break;

    begin = clock();
    while (true) {
      bool checkedOneSec = false;
      timeSpent = (double)(clock() - begin) / CLOCKS_PER_SEC;
      if (timeSpent >= 1.0 && !checkedOneSec) {
        if (waitpid(currentlyRunning, &processes[currentlyRunning].status, WNOHANG) != 0) {
          processes[currentlyRunning].isCompleted = true;
          processes[currentlyRunning].timeInSec = (double)(clock() - processes[currentlyRunning].begin) / CLOCKS_PER_SEC;
          break;
        }

        checkedOneSec = true;
      }
      if (timeSpent >= 2.0) {
        if (waitpid(currentlyRunning, &processes[currentlyRunning].status, WNOHANG) != 0) {
          processes[currentlyRunning].isCompleted = true;
          processes[currentlyRunning].timeInSec = (double)(clock() - processes[currentlyRunning].begin) / CLOCKS_PER_SEC;
        }

        break;
      }
    }
    seconds += 2;

    if (!feof(file)) {
      fgets(line, sizeof(line), file);
      sscanf(line, "%s %d", processes[processIndex].path, &processes[processIndex].priority);
      processes[processIndex].begin = clock();
      processes[processIndex].id = processIndex + 1;
      processIndex++;
    }

    if (seconds == 6) {
      kill(currentlyRunning, SIGTSTP);
      chosenProcess = lotteryScheduler(processes, processIndex); // returns Process
      int index = getProcessIndex(processes, processIndex, chosenProcess);
      if (hasPID(processes, processIndex, chosenProcess)) {
        kill(chosenProcess.pid, SIGCONT);
      } else {
        processes[index].pid = fork();
        if (processes[index].pid == 0) {
          path = strcat("./", processes[processIndex].path);
          execvp(path, path, NULL);
        } else if (processes[index].pid > 0) {
          currentlyRunning = processes[index].pid;
        }
      }
    }
  }

  makeSpan = (double)(clock() - beginMakeSpan);
  printf("MakeSpan: %f seconds\n", makeSpan);

  printAllProcessesTimes(processes, processIndex);

  fclose(file);

  return 0;
}

void printAllProcessesTimes(Processes processes[], int qty) {
  for (int i = 0; i < qty; i++)
    printf("Process %d took %f seconds\n", processes[i].id, processes[i].timeInSec);
}

bool hasPID(Process processes[], int qty, Process chosenProcess) {
  for (int i = 0; i < qty; i++) {
    if (processes[i].id == chosenProcess.id && chosenProcess.pid > 0) {
      return true;
      break;
    }
  }

  return false;
}

Process lotteryScheduler(Process processes[], int qty) {
  int totalTickets = 0;
  for (int i = 0; i < qty; i++) {
    if (processes[i].isCompleted) continue;
    totalTickets += processes[i].priority + 1;
  }

  int ticket = (rand() % totalTickets) + 1;

  Process chosenProcess = NULL;
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

bool hasAllProcessesCompleted(Process processes[], int qty) {
  let i = 0;

  for (i = 0; i < qty; i++) 
    if (!processes[i].isCompleted)
      break;

  if (i == (qty - 1))
    return false;

  return true;
}

int getProcessIndex(Process processes[], int qty, Process process) {
  int i = 0;
  for (i = 0; i < qty; i++)
    if (processes[i].id == process.id)
      break;

  return i;
}
*/

// como parar a execução de um processo
//  kill -9 PID SIGTSTP
//  kill -9 PID SIGSTOP

/*
 * Esqueleto 2
 *
 *main() {
        abre arquivo
        fork e exec arquivo (pega pid)
        segundos = 0
        for (2sec) {
                skip primeira exec
                le linha do arquivo
                armazena info do proximo processo e prioridade
                adiciona processo na fila ready

                soma 2 em segundos
                segundos == 6 {
                        sigtstp no pid
                        roda escalonador
                        fork e exec arquivo
                }
        }
}

escalonador() {
        lottery ticket scheduler
}
 *
 *
 * */
