#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include<arpa/inet.h>	//inet_addr
#include<stdio.h>	//printf
#include <stdlib.h>
#include<unistd.h>    //write
#include <string.h>

//#defien PORT 3005
//#define BACKLOG 2

int main (int argc, char *argv[]){
    int id; //Descriptores servidor, cliente
    char buffer[500];
    char nombre[500];
    char usuariochat[500];

    struct sockaddr_in server; //int servidor

    struct hosten *he; // lo necesitare para el nombre de host o la ip

    int port;
    sscanf(argv[2], "%d", &port);

    //estructura server del SERVIDOR

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);// el puerto que usare (el segundo argumento de la llamada)
    bzero(&(server.sin_zero),8);

    //socket del cliente
    id = socket(AF_INET, SOCK_STREAM, 0);
    if( id == -1){
        printf("No se pudo crear socket\n");
    }
    //puts("Socket creado\n");

    if (connect(id , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Fallo la conexion. Error\n");
		return 1;
	}
    //puts("Conectado\n");
    // ya en teoria la conexion al server esta hecha
    printf("Escriba su nombre de usuario:  ");
    scanf("%s", nombre);
    send(id, nombre, strlen(nombre) , 0);

  //  strcpy(usuariochat, buffer); RECIBE EL NOMBRE DEL OTRO
    int first = 1;
    while(1){

        memset(buffer, 0, 500);

        recv(id, buffer, 500, 0);
        //write(id, "", 1);

        if( strcmp(buffer, "") != 0 &&  first == 1){
            printf("%s se ha conectado\n", buffer);
            first = 0;
        }else if(strcmp(buffer, "")){
            printf("%s\n", buffer);
        }

        // scanf("%s", buffer);

        // if( send(id, buffer, strlen(buffer) , 0) < 0){
        //     puts("Envio fallido");
        //     return 1;
        // }

        // if(strcmp(buffer, "EXIT") == 0){
        //     close(id);
        //     return 1;
        // }
        memset(buffer, 0, 500);
    }
    //write(id, buffer, 100);
    close(id); //Importante cerrar la conexion

}
