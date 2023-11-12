#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
