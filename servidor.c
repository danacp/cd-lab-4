#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <netdb.h>
#include <string.h>


#define BACKLOG 3
//ESTRUCUTRA PARA SOCKET-THREAD
struct server_socket_thread{
    char buffer [256],buffer2 [256];
    int newsockfd,newsockfd2;
    pthread_t tid;
    pthread_attr_t attr;
        int rw,rw2;
};

//FUNCIONES QUE SIRVE DE RECEPTOR Y ENVIO
void* server_recibe_envia(void* args){
    int n;
    struct server_socket_thread *sar = (struct server_socket_thread*) args;
    if(sar->rw == 0){
      //HILO QUE ESCUCHA A CLIENTE 1 Y ENVIA A CLIENTE2
      while(1){
        bzero(sar->buffer, 256);
        n = read(sar->newsockfd, sar->buffer, 255);
        if (n < 0){
            error("ERROR reading from socket");
        }
        write(sar->newsockfd2, sar->buffer, strlen((char *) &(sar->buffer)));
        }
    } else {
      //HILO QUE ESCUCHA AL CLIENTE 2 Y ENVIA A CLIENTE1
        while(1){
          bzero(sar->buffer, 256);
          n = read(sar->newsockfd2, sar->buffer, 255);
          if (n < 0){
            error("ERROR reading from socket");
          }
          write(sar->newsockfd, sar->buffer, strlen((char *) &(sar->buffer)));
        }
    }
}

int main(int argc, char *argv[]){

    int port;
    sscanf(argv[1], "%d", &port);
    int id, new_id, new_id2; // descriptores servidor, cliente
    int tam;
    char buffer[500];
    char buffer2[500];
    struct sockaddr_in server , client;
    struct server_socket_thread server_write_t, server_read_t;

    //estructura server del SERVIDOR

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port); // el puerto que usare
    bzero(&(server.sin_zero), 8);

    id = socket(AF_INET, SOCK_STREAM, 0);
    if( id == -1){
        printf("No se ha podido crear el socket\n");
    }

    if( bind(id, (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("Bind fallo. Error\n");
        return 1;
    }

    puts("Esperando por conexiones...\n");
    tam = sizeof(struct sockaddr_in);

    listen(id, 3); // comienzo a escuchar

    server_write_t.newsockfd = accept(id, (struct sockaddr *)&client, (socklen_t*)&tam); // si llega algo lo acepto
    printf("Se obtuvo una conexion desde %s\n", inet_ntoa(client.sin_addr)); // Asi se obtiene informacion de red del cliente, podrian aplicar otra cosa

    server_write_t.newsockfd2= accept(id, (struct sockaddr *)&client, (socklen_t*)&tam); // si llega algo lo acepto
    printf("Se obtuvo una conexion desde %s\n", inet_ntoa(client.sin_addr));

        memset(buffer, 0, 500);
        memset(buffer2, 0, 500);
        //RECIBE NOMBRES
        recv(server_write_t.newsockfd, buffer, (sizeof(buffer)-1), 0);
        printf ("Se conecta %s\n", buffer);
        recv(server_write_t.newsockfd2, buffer2, (sizeof(buffer)-1), 0);
        printf ("Se conecta %s\n", buffer2);

        memset(buffer, 0, 500);
        memset(buffer2, 0, 500);

        //INICIO HILOS
        pthread_attr_init(&server_write_t.attr);
        pthread_attr_init(&server_read_t.attr);
        server_write_t.rw = 1;
        server_read_t.rw = 0;
        bcopy(&server_write_t.newsockfd, &server_read_t.newsockfd, sizeof(server_write_t.newsockfd));
        bcopy(&server_write_t.newsockfd2, &server_read_t.newsockfd2, sizeof(server_write_t.newsockfd2));
        //ESCUCHA AL CLIENTE 1 Y ENVIA AL CLIENTE 2:
        pthread_create(&server_write_t.tid, &server_write_t.attr, server_recibe_envia, &server_write_t);
        //ESCUCHA AL CLIENTE 2 Y ENVIA AL CLIENTE 1:
        pthread_create(&server_read_t.tid, &server_read_t.attr, server_recibe_envia, &server_read_t);

        pthread_join(server_write_t.tid, NULL);
        pthread_join(server_read_t.tid, NULL);



        return 0;



}
