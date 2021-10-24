#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include <arpa/inet.h> 
#include<netinet/in.h>
#include <string.h>

#define path "/home/prudhvi/Desktop/IIITB/SS/assignments/mini_proj/Database/"
#define maxUsers 5
struct user{
    int user_id;
    char user_name[30];
    char password[30];
    int user_type;
};
void client_handler(struct sockaddr_in cli, int sd);
void admin_handler();
void customer_handler();
void agent_handler();
void login(int nsd, int userid, char username[30], char password[30], char typeofuser[30]);
void signup(int nsd, int userid, char username[30], char password[30], char typeofuser[30]);
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
    int sz, client_number, nsd, userid, choice;
    char password[20], username[20], typeofuser[20];
    sz = sizeof(cli);
    nsd = accept(sd, (void *)(&cli), &sz);
    read(nsd, &client_number, sizeof(client_number));
    printf("Client Connected : %d \n", client_number);
    read(nsd, &userid, sizeof(userid));
    read(nsd, &username, sizeof(username));
    read(nsd, &password, sizeof(password));
    read(nsd, &typeofuser, sizeof(typeofuser));
    read(nsd, &choice, sizeof(choice));
    printf("%s\n", typeofuser);
    if(choice == 1){
        //login
        login(nsd, userid, username, password, typeofuser);
    }
    else if(choice == 2){
        //signup
        signup(nsd, userid, username, password, typeofuser);
    }
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

void signup(int nsd, int userid, char username[30], char password[30], char typeofuser[30]){
    printf("Reached signup page!!\n"); /* ***** */
    struct user db; 
    db.user_id = userid;
    strcpy(db.user_name, username);
    strcpy(db.password, password);
    if(strcmp(typeofuser, "customer") == 0){
        db.user_type = 1;
    }
    else if(strcmp(typeofuser, "agent") == 0){
        db.user_type = 2;
    }   
    else if(strcmp(typeofuser, "admin") == 0){
        db.user_type = 3;
    }
    char path_userdb[100] = path; 
    strcat(path_userdb, "user_db.txt");
    int user_fd = open(path_userdb, O_CREAT | O_RDWR, 00777);
    if(user_fd == -1){
        perror("ERROR OPEN FD!");
    }
    printf("OpenedFile Successfully!!\n"); /* ***** */


}
void login(int nsd, int userid, char username[30], char password[30], char typeofuser[30]){
    printf("Reached login page!!\n");
}


