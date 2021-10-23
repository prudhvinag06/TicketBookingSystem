#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include <arpa/inet.h> 
#include<netinet/in.h>
#include <string.h>

int isAuthenticated(char client_number[], char client_username[], char client_type[]);

/*---------------------------------------------Client-------------------------------------------*/
int main(int argc, char * argv[]){
    struct sockaddr_in serv;
    char client_username[20], client_password[20], client_type[20];
    int sd, connection_status, type;
    int client_number = 23;
    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    serv.sin_family = AF_UNIX;
    serv.sin_addr.s_addr = INADDR_ANY;   //inet_addr("ser ip") //in case of not a local host  
    serv.sin_port = htons(5555);
    connection_status = connect(sd, (struct sockaddr *) &serv, sizeof(serv));
    if(connection_status == -1){
        printf("Error\n");
    }

/*--------------------------------------------Initialization------------------------------------*/
    write(sd, &client_number, sizeof(client_number));
    read(sd, &client_number, sizeof(client_number));
    printf("Server Says : %d\n", client_number);
    printf("Enter username : \n");
    scanf("%s", client_username);
    printf("Enter password : \n");
    scanf("%s", client_password);
    printf("Select Type of user : \n");
    printf("1.Customer\n");
    printf("2.Admin\n");
    printf("3.Agent\n");
    scanf("%d", &type);
    while(!isAuthenticated(client_number, client_password, client_type)){
        printf("Enter password : \n");
        scanf("%s", client_password);
    }
    if(type == 1){
        strcpy(client_type, "customer");
    }
    else if(type == 2){
        strcpy(client_type, "admin");
    }
    else{
        strcpy(client_type, "agent");
    }

    write(sd, &client_username, sizeof(client_username));
    write(sd, &client_password, sizeof(client_password));
    write(sd, &client_type, sizeof(client_type));

    

   // close(sd);
}