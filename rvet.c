/**
 * Template base para implementação de Relógios Vetoriais (MPI)
 * -------------------------------------------------------------
 * Implementar, tomando como base a sequência de operações da figura
 * do link: https://drive.google.com/file/d/1IOAJPpJWUoRC0kygZKr6ERe0hpIICRTP/view?usp=sharing
 *
 * Compilação: mpicc -o rvet rvet.c
 * Execução:   mpiexec -n 3 ./rvet
 */

 //Arthur Felipe Dantas Melo

#include <stdio.h>
#include <string.h>
#include <mpi.h>

/* Estrutura do relógio vetorial (3 processos). */
typedef struct Clock {
    int p[3];
} Clock;

/*
 * Imprime o estado atual do relógio vetorial.
 * Mostra: qual processo realizou a atualização e o vetor resultante.
 */
void PrintClock(const char *acao, Clock *clock) {
    int pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    printf("[Atualizado por P%d] %s -> Clock = (%d,%d,%d)\n",
           pid, acao, clock->p[0], clock->p[1], clock->p[2]);
}

/*
 * Evento interno (sem comunicação).
 * Incrementa o contador local do processo.
 */
void Event(Clock *clock) {
    int pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    clock->p[pid]++;
    
    // No final, imprime relógio atualizado
    PrintClock("Evento interno", clock);
}

/*
 * Envio de mensagem.
 * Deve:
 *   1. Obter o rank.
 *   2. Incrementar clock local (evento de envio).
 *   3. Enviar clock->p via MPI_Send.
 */
void Send(int dest, Clock *clock) {
    int pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    // TODO: incrementar clock local e enviar clock->p via MPI_Send

    //aq ele incrementa a parte do vetor q representa o relogio do proprio processo
    clock->p[pid]++;

    //faz o envio da mensagem
    MPI_Send(
        clock->p, //diz oq o processo vai enviar
        3, //diz a quantidade de elementos
        MPI_INT, //diz o tipo do valor q vai ser passado
        dest, // diz pra quem vai enviar
        0, //so uma tag
        MPI_COMM_WORLD //diz que a comunicacao ta rolando no grupo onde estao os 3 processos
    );
    
    // No final, imprime relógio atualizado:
    PrintClock("Envio de mensagem", clock);
}

/*
 * Recebimento de mensagem.
 * Deve:
 *   1. Obter o rank.
 *   2. Receber vetor remoto via MPI_Recv.
 *   3. Incrementar clock local (evento de recebimento).
 *   4. Fazer fusão (max elemento a elemento) com vetor recebido.
 */
void Receive(int src, Clock *clock) {
    int pid;
    int provisorio[3];

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    // TODO: receber vetor e atualizar clock local
    
    MPI_Recv(
        provisorio, //aq diz onde vamos guardar a informacao q vai chegar
        3, //quantos valores vao ser recebidos
        MPI_INT, //tipo dos valores
        src, // de quem vai vir
        0, // so uma tag dnv
        MPI_COMM_WORLD, //diz q a comunicacao ta acontecendo no grupo que os 3 processos estao
        MPI_STATUS_IGNORE // n precisa usar status
        
    );

    //aq ele incrementa a parte do vetor que representa o proprio processo pq o ato de receber ja conta como evento
    clock->p[pid]++;

    for(int i = 0; i < 3; i++){
        if(provisorio[i] > clock->p[i]){
            clock->p[i] = provisorio[i];
        }
    }
    
    // No final, imprime relógio atualizado
    PrintClock("Recebimento de mensagem", clock);
}

/*
 * Cada processo define sua sequência de eventos e comunicações
 * conforme o diagrama da figura de referência.
 */

void process0() {
    Clock clock = {{0,0,0}};
    PrintClock("Estado inicial", &clock);

    // TODO: Send/Receive conforme diagrama
    
    Event(&clock);
    Send(1, &clock);
    Receive(1, &clock);
    Send(2, &clock);
    Receive(2, &clock);
    Send(1, &clock);
    Event(&clock);
    
}

void process1() {
    Clock clock = {{0,0,0}};
    PrintClock("Estado inicial", &clock);
    // TODO
    
    Send(0, &clock);
    Receive(0, &clock);
    Receive(0, &clock);
    
}

void process2() {
    Clock clock = {{0,0,0}};
    PrintClock("Estado inicial", &clock);
    // TODO

    Event(&clock);
    Send(0, &clock);
    Receive(0, &clock);
}

int main(void) {
    int my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0)
        process0();
    else if (my_rank == 1)
        process1();
    else if (my_rank == 2)
        process2();

    MPI_Finalize();
    return 0;
}

