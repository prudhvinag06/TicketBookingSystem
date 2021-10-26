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
#define maxAgents 5 
#define port 5555
static int id = 1;
struct user{
    int user_id;
    char user_name[30];
    char password[30];
    int user_type;
};
struct admin{
    int user_id;
    char user_name[30];
    char password[30];
    int user_type;
};
struct agent{
    int user_id;
    char user_name[30];
    char password[30];
    int user_type;
};

void client_handler(struct sockaddr_in cli, int sd);
void admin_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]);
void customer_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]);
void agent_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]);
void login(int nsd, int userid, char username[30], char password[30], char typeofuser[30]);
void signup(int nsd, char username[30], char password[30], char typeofuser[30]);
int main(){
    struct sockaddr_in serv, cli;
    int sd, sz, nsd;
    char buf[80];
    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    serv.sin_family = AF_UNIX;
    serv.sin_addr.s_addr = INADDR_ANY; //specify ip address in double quotes. Else if its same machine we can specify like this.
    serv.sin_port = htons(port);

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
    read(nsd, &choice, sizeof(choice));
    if(choice == 1){ //login
        read(nsd, &userid, sizeof(userid));
        read(nsd, &username, sizeof(username));
        read(nsd, &password, sizeof(password));
        read(nsd, &typeofuser, sizeof(typeofuser));
        /*
        todo : Authenticate Password.
        */
        //check type of user
        login(nsd, userid, username, password, typeofuser);
    }
    else if(choice == 2){ //signup
        read(nsd, &username, sizeof(username));
        read(nsd, &password, sizeof(password));
        read(nsd, &typeofuser, sizeof(typeofuser));
        signup(nsd, username, password, typeofuser);
    }
    printf("%s\n", typeofuser);
    
}

void signup(int nsd, char username[30], char password[30], char typeofuser[30]){
    printf("Reached signup page!!\n");
    /*
    todo : user binary semaphore here to avoid multiple updates to id variable.
    */

    /*
    todo : Add file locking where ever necessary.
    */
    struct user db; 
    db.user_id = id;    
    strcpy(db.user_name, username);
    strcpy(db.password, password);
    if(strcmp(typeofuser, "customer") == 0){ //customer
        db.user_type = 1;
        char path_userdb[100] = path; 
        strcat(path_userdb, "user_db.txt");
        int user_fd = open(path_userdb, O_CREAT | O_RDWR, 00777);
        if(user_fd == -1){
            perror("ERROR OPEN FD!");
        }
        printf("Opened File Successfully!!\n"); 
        lseek(user_fd, (id - 1) * sizeof(db), SEEK_SET);
        int write_status = write(user_fd, &db, sizeof(db));
        if(write_status == -1){
            perror("ERROR WRITE TO FILE");
        }
        printf("Updated Customer Database!!\n");
        write(nsd, &id, sizeof(id));
        id++; 
    }
    else if(strcmp(typeofuser, "agent") == 0){ //agent
        db.user_type = 2;
        char path_userdb[100] = path; 
        strcat(path_userdb, "agent_db.txt");
        int agent_fd = open(path_userdb, O_CREAT | O_RDWR, 00777);
        if(agent_fd == -1){
            perror("ERROR OPEN FD!");
        }
        printf("Opened File Successfully!!\n"); 
        lseek(agent_fd, (id - 1) * sizeof(db), SEEK_SET);
        int write_status = write(agent_fd, &db, sizeof(db));
        if(write_status == -1){
            perror("ERROR WRITE TO FILE");
        }
        printf("Updated Agent Database!!\n"); 
        write(nsd, &id, sizeof(id));
        id++;
    }   
    else if(strcmp(typeofuser, "admin") == 0){ //admin
        db.user_type = 3;
        char path_userdb[100] = path; 
        strcat(path_userdb, "admin_db.txt");
        int admin_fd = open(path_userdb, O_CREAT | O_RDWR, 00777);
        if(admin_fd == -1){
            perror("ERROR OPEN FD!");
        }
        printf("Opened File Successfully!!\n"); 
        lseek(admin_fd, (id - 1) * sizeof(db), SEEK_SET);
        int write_status = write(admin_fd, &db, sizeof(db));
        if(write_status == -1){
            perror("ERROR WRITE TO FILE");
        }
        printf("Updated Admin Database!!\n");
        write(nsd, &id, sizeof(id));
        id++; 
    }
}
void login(int nsd, int userid, char username[30], char password[30], char typeofuser[30]){
    if(strcmp(typeofuser, "customer") == 0){
        customer_handler(nsd, userid, username, password, typeofuser);
    }
    else if(strcmp(typeofuser, "agent") == 0){
        agent_handler(nsd, userid, username, password, typeofuser);
    }   
    else if(strcmp(typeofuser, "admin") == 0){
        admin_handler(nsd, userid, username, password, typeofuser);
    }
}

void customer_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]){
    struct user db; 
    db.user_id = id;    
    strcpy(db.user_name, username);
    strcpy(db.password, password);
    db.user_type = 1;
    printf("cust was selected\n");
    
    //setting file
    char path_userdb[100] = path; 
    strcat(path_userdb, "user_db.txt");
    int fd_cust = open(path_userdb, O_RDWR, 00777);


    lseek(fd_cust, (userid - 1) * sizeof(db), SEEK_SET);
    read(fd_cust, &db, sizeof(db));

    printf("Name : %s\n", db.user_name);
    printf("Pass : %s\n", db.password);
}

void agent_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]){
    printf("agent was selected\n");

}
void admin_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]){
    printf("admin was selected\n");
}


