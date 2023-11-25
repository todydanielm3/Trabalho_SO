#include <stdbool.h> // Inclui definição do tipo bool para valores booleanos
#include <stdio.h>   // Inclui a biblioteca padrão de entrada/saída
#include <stdlib.h> // Inclui a biblioteca padrão para funções como malloc e rand
#include <string.h> // Inclui funções para manipulação de strings
#include <sys/types.h> // Inclui definições de tipos de dados usados em chamadas de sistema
#include <sys/wait.h> // Permite o uso de waitpid para processos
#include <time.h>     // Inclui funções para manipulação de tempo
#include <unistd.h>   // Fornece acesso a chamadas de sistema POSIX como fork

    // Define uma estrutura para armazenar informações sobre processos
    struct Process {
  int id;           // Identificador único do processo
  int pid;          // PID do processo
  char path[100];   // Caminho para o executável do processo
  clock_t begin;    // Tempo de início do processo
  double timeInSec; // Duração do processo em segundos
  int priority;     // Prioridade do processo
  int status;       // Status do processo
  bool isCompleted; // Indica se o processo foi concluído
};

// Declaração de funções utilizadas no código
bool hasPID(struct Process[], int, struct Process);
struct Process lotteryScheduler(struct Process[], int);
bool hasAllProcessesCompleted(struct Process[], int);
int getProcessIndex(struct Process[], int, struct Process);
void printAllProcessesTimes(struct Process[], int);
int getProcessIndexWithPID(struct Process[], int, int);

// Função principal do programa
int main() {
  FILE *file;             // Ponteiro para o arquivo de entrada
  char line[100];         // Buffer para armazenar linhas lidas do arquivo
  char path[100];         // Buffer para armazenar o caminho do executável
  int seconds = 0;        // Contador de segundos
  clock_t begin;          // Variável para armazenar o tempo de início
  double timeSpent = 0.0; // Variável para calcular o tempo gasto
  struct Process processes[100]; // Array para armazenar os processos
  int processIndex = 0;          // Índice para o array de processos
  int currentlyRunning;          // PID do processo atualmente em execução
  struct Process chosenProcess;  // Processo escolhido pelo escalonador

  file = fopen("input.txt", "r"); // Abre o arquivo de entrada para leitura

  if (file == NULL) // Verifica se o arquivo foi aberto com sucesso
    return 1;       // Retorna 1 se houver erro na abertura do arquivo

  // Lê a primeira linha do arquivo e inicializa o primeiro processo
  fgets(line, sizeof(line), file);
  sscanf(line, "%s %d", processes[processIndex].path,
         &processes[processIndex].priority);
  processes[processIndex].id = processIndex + 1;
  processes[processIndex].pid = fork();   // Cria um processo filho
  if (processes[processIndex].pid == 0) { // Se for o processo filho
    execl(processes[processIndex].path, processes[processIndex].path,
          NULL);                                // Executa o programa
  } else if (processes[processIndex].pid > 0) { // Se for o processo pai
    processes[processIndex].begin = clock();    // Armazena o tempo de início
    currentlyRunning =
        processes[processIndex].pid; // Atualiza o PID do processo em execução
    processIndex++;
    processes[processIndex].isCompleted =
        false; // Marca o processo como não concluído
  }

  // Loop principal do escalonador
  while (1) {
    begin = clock();            // Armazena o tempo de início do loop
    bool checkedOneSec = false; // Flag para verificar se já passou um segundo
    int i = getProcessIndexWithPID(
        processes, processIndex,
        currentlyRunning); // Obtém o índice do processo atual

    // Loop para controlar o tempo de execução do processo atual
    do {
      timeSpent =
          (double)(clock() - begin) /
          CLOCKS_PER_SEC; // Calcula o tempo gasto desde o início do loop

      // Verifica se já passou um segundo e atualiza o status do processo
      if (timeSpent >= 1.0 && !checkedOneSec) {
        if (waitpid(currentlyRunning,
                    &processes[i].

                     status,
                    WNOHANG) != 0) {
          processes[i].isCompleted = true;
          processes[i].timeInSec =
              (double)(clock() - processes[i].begin) / CLOCKS_PER_SEC;
        }

        checkedOneSec = true;
      }

      // Verifica se já passou dois segundos e atualiza o status do processo
      if (timeSpent >= 2.0 &&
          waitpid(currentlyRunning, &processes[i].status, WNOHANG) != 0) {
        processes[i].isCompleted = true;
        processes[i].timeInSec =
            (double)(clock() - processes[i].begin) / CLOCKS_PER_SEC;
      }
    } while (timeSpent <
             2.0); // Continua no loop enquanto não passar dois segundos

    seconds += 2; // Incrementa o contador de segundos
    // Verifica se todos os processos foram concluídos
    if (hasAllProcessesCompleted(processes, processIndex))
      break; // Sai do loop se todos os processos estiverem concluídos

    // Lê e inicia novos processos do arquivo se não estiver no final do arquivo
    if (!feof(file)) {
      fgets(line, sizeof(line), file);
      sscanf(line, "%s %d", processes[processIndex].path,
             &processes[processIndex].priority);
      processes[processIndex].begin = clock();
      processes[processIndex].id = processIndex + 1;
      processIndex++;
    }

    // A cada 6 segundos, altera o processo em execução
    if (seconds % 6 == 0) {
      int index =
          getProcessIndexWithPID(processes, processIndex, currentlyRunning);
      if (waitpid(currentlyRunning, &processes[index].status, WNOHANG) != 0) {
        processes[index].isCompleted = true;
        processes[index].timeInSec =
            (double)(clock() - processes[index].begin) / CLOCKS_PER_SEC;
      }

      kill(currentlyRunning, SIGTSTP); // Suspende o processo atual
      chosenProcess = lotteryScheduler(
          processes, processIndex); // Escolhe um novo processo para executar
      index = getProcessIndex(
          processes, processIndex,
          chosenProcess); // Obtém o índice do processo escolhido
      // Verifica se o processo escolhido já possui um PID
      if (hasPID(processes, processIndex, chosenProcess)) {
        kill(processes[index].pid,
             SIGCONT); // Continua a execução do processo escolhido
        currentlyRunning =
            processes[index].pid; // Atualiza o PID do processo em execução
      } else {
        processes[index].pid = fork();   // Cria um novo processo filho
        if (processes[index].pid == 0) { // Se for o processo filho
          execl(processes[index].path, processes[index].path,
                NULL);                         // Executa o programa
        } else if (processes[index].pid > 0) { // Se for o processo pai
          currentlyRunning =
              processes[index].pid; // Atualiza o PID do processo em execução
        }
      }
    }
  }

  // Imprime o tempo total necessário para concluir todos os processos
  printf("MakeSpan: %d seconds\n", seconds);
  printAllProcessesTimes(
      processes, processIndex); // Imprime o tempo de execução de cada processo

  fclose(file); // Fecha o arquivo

  return 0; // Finaliza o programa
}

// Função para imprimir o tempo de execução de cada processo
void printAllProcessesTimes(struct Process processes[], int qty) {
  for (int i = 0; i < qty; i++)
    printf("Process %d took %.0f\n", processes[i].id, processes[i].timeInSec);
}

// Função para verificar se um processo específico possui um PID
bool hasPID(struct Process processes[], int qty, struct Process chosenProcess) {
  for (int i = 0; i < qty; i++) {
    if (processes[i].id == chosenProcess.id && chosenProcess.pid > 0) {
      return true;
      break;
    }
  }

  return false;
}

// Implementação do escalonador de loteria
struct Process lotteryScheduler(struct Process processes[], int qty) {
  int totalTickets = 0;
  // Calcula o número total de bilhetes
  for (int i = 0; i < qty; i++) {
    if (processes[i].isCompleted)
      continue; // Ignora processos concluídos
    totalTickets += processes[i].priority + 1;
  }

  int ticketo = (rand() % totalTickets) + 1; // Selecion
  // Gera um número aleatório entre 1 e o total de tickets.
  int ticket = (rand() % totalTickets) + 1;

  // Declara a estrutura 'chosenProcess' para armazenar o processo escolhido.
  struct Process chosenProcess;
  for (int i = 0; i < qty; i++) {
    // Se o processo atual já foi concluído, pula para o próximo.
    if (processes[i].isCompleted)
      continue;

    // Reduz o número de tickets pelo número de tickets do processo atual
    // (prioridade + 1).
    ticket -= processes[i].priority + 1;
    // Se o número de tickets for menor ou igual a zero, escolhe este processo e
    // sai do loop.
    if (ticket <= 0) {
      chosenProcess = processes[i];
      break;
    }
  }

  // Retorna o processo escolhido.
  return chosenProcess;
}

// Função que verifica se todos os processos foram concluídos.
bool hasAllProcessesCompleted(struct Process processes[], int qty) {
  for (int i = 0; i < qty; i++)
    // Se algum processo ainda não foi concluído, retorna falso.
    if (!processes[i].isCompleted)
      return false;

  // Retorna verdadeiro se todos os processos foram concluídos.
  return true;
}

// Função que retorna o índice de um processo específico na lista de processos.
int getProcessIndex(struct Process processes[], int qty,
                    struct Process process) {
  for (int i = 0; i < qty; i++)
    // Compara o ID do processo com o ID de cada processo na lista.
    if (processes[i].id == process.id)
      return i; // Retorna o índice se os IDs forem iguais.

  // Retorna 0 se o processo não for encontrado (pode ser problemático se 0 for
  // um índice válido).
  return 0;
}

// Função que retorna o índice de um processo com base no seu PID.
int getProcessIndexWithPID(struct Process processes[], int qty, int pid) {
  for (int i = 0; i < qty; i++)
    // Compara o PID passado com o PID de cada processo na lista.
    if (processes[i].pid == pid)
      return i; // Retorna o índice se os PIDs forem iguais.

  // Retorna 0 se o PID não for encontrado (mesmo problema potencial mencionado
  // acima).
  return 0;
}
