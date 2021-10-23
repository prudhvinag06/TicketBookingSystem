#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include <arpa/inet.h> 
#include<netinet/in.h>
#include <string.h>
#define maxUsers 5

void client_handler(struct sockaddr_in cli, int sd);
void admin_handler();
void customer_handler();
void agent_handler();
int main(){
    struct sockaddr_in serv, cli;
    int sd, sz, nsd;
    char buf[80];
    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    serv.sin_family = AF_UNIX;
    serv.sin_addr.s_addr = INADDR_ANY; //specify ip address in double quotes. Else if its same machine we can specify like this.
    serv.sin_port = htons(5555);

    while(1){ 
        int client_number;
        bind(sd, (void *)(&serv), sizeof(serv));
        printf("Server listening.......\n");    
        listen(sd, maxUsers); 
        client_handler(cli, sd);
    }
    close(sd);
}

void client_handler(struct sockaddr_in cli, int sd){
    int sz, client_number, nsd;
    char password[20], username[20], typeofuser[20];
    sz = sizeof(cli);
    nsd = accept(sd, (void *)(&cli), &sz);
    read(nsd, &client_number, sizeof(client_number));
    printf("Client Connected : %d \n", client_number);
    int x = write(nsd, "1.ENTER LOGIN DETAILS\n", 25);
    //if user exists fine
    // else create new user
    read(nsd, &username, sizeof(username));
    read(nsd, &password, sizeof(password));
    read(nsd, &typeofuser, sizeof(typeofuser));
    printf("%s\n", typeofuser);
    /*
    Pending task here : Authenticate Password
    */
    //check type of user
    if(strcmp(typeofuser, "customer") == 0){
        customer_handler();
    }
    else if(strcmp(typeofuser, "agent") == 0){
        agent_handler();
    }   
    else if(strcmp(typeofuser, "admin") == 0){
        admin_handler();
    }
}

void customer_handler(){
    printf("cust was selected\n");
}

void agent_handler(){
    printf("agent was selected\n");

}
void admin_handler(){
    printf("admin was selected\n");
}