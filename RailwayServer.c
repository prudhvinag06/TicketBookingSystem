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
#define port 12322


static int id = 1;
struct user{
    int user_id;
    char user_name[30];
    char password[30];
    int user_type;
    int flag;
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
    int flag;
};
struct train{
    int train_id;
    char train_name[30];
    int total_seats;
    int available_seats;
    char src[20];
    char dest[20];
};

int isAuthenticated(int nsd, int id, char password[30], int type);
void client_handler(struct sockaddr_in cli, int sd);
void admin_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]);
void customer_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]);
void agent_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]);
void login(int nsd, int userid, char username[30], char password[30], char typeofuser[30]);
void signup(int nsd, char username[30], char password[30], char typeofuser[30]);
void signup_admin(int nsd);
void viewRecords(int nsd);
void deleteRecord(int nsd);
int isExists(int nsd, int id, char password[30], int type);
int main(){
    struct sockaddr_in serv, cli;
    int sd, sz, nsd;
    char buf[80];
    sd = socket(AF_INET, SOCK_STREAM, 0);
    serv.sin_family = AF_INET;
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
    db.flag = 1;
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
        int x = 0;
        //customer authentication
        if(isExists(nsd, userid, password, 1)){
            while(!isAuthenticated(nsd, userid, password, 1)){
                write(nsd, &x, sizeof(x));
                char pass[30];
                read(nsd, &pass, sizeof(pass));
                strcpy(password, pass);
            }
            x = 1;
            write(nsd, &x, sizeof(x));
            printf("User Login successful\n");
            customer_handler(nsd, userid, username, password, typeofuser);
        }
        else{
            x = -1;
            write(nsd, &x, sizeof(x));
            printf("<--------------User Doesn't exist------------>\n");
        }
    }
    else if(strcmp(typeofuser, "agent") == 0){
        int x = 0;
        //AGENT authentication
        while(!isAuthenticated(nsd, userid, password, 2)){
            write(nsd, &x, sizeof(x));
            char pass[30];
            read(nsd, &pass, sizeof(pass));
            strcpy(password, pass);
        }
        x = 1;
        write(nsd, &x, sizeof(x));
        printf("User Login successful\n");
        agent_handler(nsd, userid, username, password, typeofuser);
    }   
    else if(strcmp(typeofuser, "admin") == 0){
        int x = 0;
        //ADMIN authentication
        while(!isAuthenticated(nsd, userid, password, 3)){
            write(nsd, &x, sizeof(x));
            char pass[30];
            read(nsd, &pass, sizeof(pass));
            strcpy(password, pass);
        }
        x = 1;
        write(nsd, &x, sizeof(x));
        printf("User Login successful\n");
        admin_handler(nsd, userid, username, password, typeofuser);
    }
}

/*--------------------------------------------handler---------------------------------------------------------*/
void customer_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]){
    printf("<-----------------CUST HANDLER WAS RUNNING------------------->\n");
    int choice;
    read(nsd, &choice, sizeof(choice));
    printf("Choice from customer : %d\n", choice);
    struct user db; 
    db.user_id = id;    
    strcpy(db.user_name, username);
    strcpy(db.password, password);
    db.user_type = 1;
    printf("cust was selected\n");
    
    //setting file
    char path_db[100] = path; 
    strcat(path_db, "user_db.txt");
    int fd_cust = open(path_db, O_RDWR, 00777);

    lseek(fd_cust, (userid - 1) * sizeof(db), SEEK_SET);
    read(fd_cust, &db, sizeof(db));

    printf("Name : %s\n", db.user_name);
    printf("Pass : %s\n", db.password);
}
void agent_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]){
    printf("<-----------------AGENT HANDLER WAS RUNNING-------------------->\n");
    int choice;
    read(nsd, &choice, sizeof(choice));
    printf("Choice from agent : %d\n", choice);
    
    struct user db; 
    db.user_id = id;    
    strcpy(db.user_name, username);
    strcpy(db.password, password);
    db.user_type = 1;
   
    
    //setting file
    char path_db[100] = path; 
    strcat(path_db, "agent_db.txt");
    int fd_cust = open(path_db, O_RDWR, 00777);

    lseek(fd_cust, (userid - 1) * sizeof(db), SEEK_SET);
    read(fd_cust, &db, sizeof(db));

    printf("Name : %s\n", db.user_name);
    printf("Pass : %s\n", db.password);
}
void admin_handler(int nsd, int userid, char username[30], char password[30], char typeofuser[30]){
    printf("<-----------------ADMIN HANDLER WAS RUNNING------------------->\n");
    int choice;
    read(nsd, &choice, sizeof(choice));
    printf("Choice from admin : %d\n", choice);    
    struct user db; 
    db.user_id = id;    
    strcpy(db.user_name, username);
    strcpy(db.password, password);
    db.user_type = 1;
    printf("cust was selected\n");
    
    //setting file
    char path_db[100] = path; 
    strcat(path_db, "admin_db.txt");
    int fd_cust = open(path_db, O_RDWR, 00777);

    lseek(fd_cust, (userid - 1) * sizeof(db), SEEK_SET);
    read(fd_cust, &db, sizeof(db));

    printf("Name : %s\n", db.user_name);
    printf("Pass : %s\n", db.password);
    printf("Reached before choice %d\n", choice);
    if(choice == 1){  //users list
        read(nsd, &choice, sizeof(choice));
        printf("Users List was selected\n");
        printf("Choice : %d\n", choice);
        if(choice == 1){
            viewRecords(nsd);
        }
        else if(choice == 2){
            signup_admin(nsd);
        }
        else if(choice == 3){
            deleteRecord(nsd);
        }
    }
    else if(choice == 2){ //trains list
        printf("Trains List was selected\n");
    }
}

/*-------------------------------------------------Authentication----------------------------------------------*/
int isAuthenticated(int nsd, int userid, char password[30], int type){
    struct user db;
    char path_db[100] = path; 
    if(type == 1){
        strcat(path_db, "user_db.txt");
    }
    else if(type == 2){
        strcat(path_db, "agent_db.txt");
    }
    else {
        strcat(path_db, "admin_db.txt");
    }
    int fd = open(path_db, O_RDWR, 00777);    
    lseek(fd, (userid - 1) * sizeof(db), SEEK_SET);
    read(fd, &db, sizeof(db));
    
    if(strcmp(password, db.password) == 0)
    return 1;
    else
    return 0;
}

void viewRecords(int nsd){
    printf("<----------------------viewRecords()-------------------------->\n");
    struct user db; 
    int count = 0;
    char path_db[100] = path; 
    strcat(path_db, "user_db.txt");
    int fd_cust = open(path_db, O_RDWR, 00777);
    lseek(fd_cust, 0 * sizeof(db), SEEK_SET);
    int fp = lseek(fd_cust, 0, SEEK_END);
    //count = fp / sizeof(db);
    lseek(fd_cust, 0 * sizeof(db), SEEK_SET);
    while(read(fd_cust, &db, sizeof(db))){
        if(db.flag == 1)
        count++;
        /*todo: Add seats */
    }
    printf("count : %d", count);
    if(count > 0){
        write(nsd, &count, sizeof(count));
        lseek(fd_cust, 0 * sizeof(db), SEEK_SET);
        while(read(fd_cust, &db, sizeof(db))){
            if(db.flag == 1){
                write(nsd, &db.user_name, sizeof(db.user_name));
                write(nsd, &db.user_id, sizeof(db.user_id));
            }
        /*todo: Add seats */
        }
    }
    else{
        count = -1;
        write(nsd, &count, sizeof(count));
        printf("<--------NO USERS IN DB------->\n");  
    }
    
}

void signup_admin(int nsd){
    char username[30], password[30], typeofuser[30];
    printf("reached signup_admin page\n");
    read(nsd, &username, sizeof(username));
    printf("reached signup_admin page\n");
    read(nsd, &password, sizeof(password));
    read(nsd, &typeofuser, sizeof(typeofuser));
    printf("dasfdsf");
    signup(nsd, username, password, typeofuser);
}

int isExists(int nsd, int id, char password[30], int type){
    struct user db;
    char path_db[100] = path; 
    if(type == 1){
        strcat(path_db, "user_db.txt");
    }
    else if(type == 2){
        strcat(path_db, "agent_db.txt");
    }
    else {
        strcat(path_db, "admin_db.txt");
    }
    int fd = open(path_db, O_RDWR, 00777);    
    lseek(fd, (id - 1) * sizeof(db), SEEK_SET);
    read(fd, &db, sizeof(db));
    
    if(db.flag == 0)
    return 0;
    else
    return 1;
}

void deleteRecord(int nsd){
    int user_id;
    struct user db;
    int count = 0;
    char path_db[100] = path; 
    strcat(path_db, "user_db.txt"); 
    int fd_cust = open(path_db, O_RDWR, 00777);
    read(nsd, &user_id, sizeof(user_id));
    printf("user id from client : %d\n", user_id);
    lseek(fd_cust, (user_id - 1) * sizeof(db), SEEK_SET);
    read(fd_cust,&db, sizeof(db));
    printf("name : %s\n", db.user_name);
    db.flag = 0;
    lseek(fd_cust, (user_id - 1) * sizeof(db), SEEK_SET);
    write(fd_cust, &db, sizeof(db));


}


