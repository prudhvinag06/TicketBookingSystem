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
static int train_id = 1;
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
    int occupied_seats;
    char src[20];
    char dest[20];
    int flag;
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
void searchUser(int nsd);
void unlock(int fd, struct flock lock);
void addTrains(int nsd);
void viewTrains(int nsd);
void deleteTrain(int nsd);
void searchTrain(int nsd);
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
    // db.user_id = id;    
    strcpy(db.user_name, username);
    strcpy(db.password, password);
    db.flag = 1;
    struct flock lock;
	lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    if(strcmp(typeofuser, "customer") == 0){ //customer
        
        int fp_user;
        db.user_type = 1;
        char path_userdb[100] = path; 
        strcat(path_userdb, "user_db.txt");
        int user_fd = open(path_userdb, O_CREAT | O_RDWR, 00777);
        if(user_fd == -1){
            perror("ERROR OPEN FD!");
        }
        printf("Before entering into the CS \n");
        
        fcntl(user_fd, F_SETLKW, &lock);
        int fp = lseek(user_fd, 0, SEEK_END);
        if(fp == 0){
            id = 1;
            printf("Opened File Successfully!!\n"); 
            lseek(user_fd, (id - 1) * sizeof(db), SEEK_SET);
            db.user_id = id;
            int write_status = write(user_fd, &db, sizeof(db));
            if(write_status == -1){
            perror("ERROR WRITE TO FILE");
            }
            else{
                printf("Updated Customer Database!!\n");
                write(nsd, &id, sizeof(id));
            }
            
        }
        else{
            struct user dummy;
            fp = lseek(user_fd, -1 * sizeof(db), SEEK_END);
            read(user_fd, &dummy, sizeof(dummy));
            id = dummy.user_id;
            id++;
            db.user_id = id;
            int write_status = write(user_fd, &db, sizeof(db));
            if(write_status == -1){
            perror("ERROR WRITE TO FILE");
            }
            else{
            printf("Updated Customer Database!!\n");
            write(nsd, &id, sizeof(id));
            }  
        }
            unlock(user_fd, lock);
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
        printf("Before entering into the CS \n");
        fcntl(agent_fd, F_SETLKW, &lock);
        int fp = lseek(agent_fd, 0, SEEK_END);
        if(fp == 0){
            id = 1;
            printf("Opened File Successfully!!\n"); 
            lseek(agent_fd, (id - 1) * sizeof(db), SEEK_SET);
            db.user_id = id;
            int write_status = write(agent_fd, &db, sizeof(db));
            if(write_status == -1){
            perror("ERROR WRITE TO FILE");
            }
            else{
                printf("Updated Agent Database!!\n");
                write(nsd, &id, sizeof(id));
            }    
        }
        else{
            struct user dummy;
            fp = lseek(agent_fd, -1 * sizeof(db), SEEK_END);
            read(agent_fd, &dummy, sizeof(dummy));
            id = dummy.user_id;
            id++;
            db.user_id = id;
            int write_status = write(agent_fd, &db, sizeof(db));
            if(write_status == -1){
            perror("ERROR WRITE TO FILE");
            }
            else{
            printf("Updated Agent Database!!\n");
            write(nsd, &id, sizeof(id));
            }  
        } 
           unlock(agent_fd, lock);
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
        printf("Before entering into the CS \n");
        fcntl(admin_fd, F_SETLKW, &lock);
        int fp = lseek(admin_fd, 0, SEEK_END);
        if(fp == 0){
            id = 1;
            printf("Opened File Successfully!!\n"); 
            lseek(admin_fd, (id - 1) * sizeof(db), SEEK_SET);
            db.user_id = id;
            int write_status = write(admin_fd, &db, sizeof(db));
            if(write_status == -1){
            perror("ERROR WRITE TO FILE");
            }
            else{
                printf("Updated Admin Database!!\n");
                write(nsd, &id, sizeof(id));
            }    
        }
        else{
            struct user dummy;
            fp = lseek(admin_fd, -1 * sizeof(db), SEEK_END);
            read(admin_fd, &dummy, sizeof(dummy));
            id = dummy.user_id;
            id++;
            db.user_id = id;
            int write_status = write(admin_fd, &db, sizeof(db));
            if(write_status == -1){
            perror("ERROR WRITE TO FILE");
            }
            else{
            printf("Updated Admin Database!!\n");
            write(nsd, &id, sizeof(id));
            }  
        } 
        unlock(admin_fd, lock);
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
        else if(choice == 5){
            searchUser(nsd);
        }
    }
    else if(choice == 2){ //trains list
        printf("Trains List was selected\n");
        read(nsd, &choice, sizeof(choice));
        printf("Admin Train choice was : %d\n", choice);
        if(choice == 1){
            viewTrains(nsd);
            //view trains
        }
        else if(choice == 2){
            addTrains(nsd);
            //add trains
        }
        else if(choice == 3){
            //delete trains
        }
        else if(choice == 5){
            //search train
        }
    }
}

void addTrains(int nsd){
    int x = 5, total_seats = 0;
    char train_name[30], train_source[30], train_dest[30];
    struct train train_db;

    read(nsd, &train_name, sizeof(train_name));
    read(nsd, &total_seats, sizeof(total_seats));
    read(nsd, &train_source, sizeof(train_source));
    read(nsd, &train_dest, sizeof(train_dest));
    printf("train name : %s\n", train_name);
    train_db.total_seats = total_seats;
    strcpy(train_db.src, train_source);
    strcpy(train_db.dest, train_dest);
    strcpy(train_db.train_name, train_name);
    train_db.flag = 1;
    train_db.occupied_seats = 0;

    char path_traindb[100] = path; 
    strcat(path_traindb, "train_db.txt");
    int train_fd = open(path_traindb, O_CREAT | O_RDWR, 00777);
    if(train_fd == -1){
        perror("ERROR OPEN FD!");
    }

    int fp = lseek(train_fd, 0, SEEK_END);
    if(fp == 0){
        train_id = 1;
        printf("Opened File Successfully!!\n"); 
        lseek(train_fd, (train_id - 1) * sizeof(train_db), SEEK_SET);
        train_db.train_id = train_id;
        int write_status = write(train_fd, &train_db, sizeof(train_db));
        if(write_status == -1){
            perror("ERROR WRITE TO FILE");
        }
        else{
            printf("Updated Customer Database!!\n");
            write(nsd, &train_id, sizeof(train_id));
        } 
    }
    else{
        struct train dummy;
        fp = lseek(train_fd, -1 * sizeof(train_db), SEEK_END);
        read(train_fd, &dummy, sizeof(dummy));
        train_id = dummy.train_id;
        train_id++;
        train_db.train_id = train_id;
        int write_status = write(train_fd, &train_db, sizeof(train_db));
        if(write_status == -1){
            perror("ERROR WRITE TO FILE");
        }
        else{
            printf("Updated Customer Database!!\n");
            write(nsd, &train_id, sizeof(train_id));
            }  
        }

}

void viewTrains(int nsd){
    printf("<----------------------viewTrains()-------------------------->\n");
    struct train train_db; 
    int count = 0;
    char path_db[100] = path; 
    strcat(path_db, "train_db.txt");
    int fd_train = open(path_db, O_RDWR, 00777);
    printf("<-----------------------FILE LOCKED------------------------->\n");
    //fcntl(fd_cust, F_SETLKW, &lock); 
    lseek(fd_train, 0 * sizeof(train_db), SEEK_SET);
    int fp = lseek(fd_train, 0, SEEK_END);
    //count = fp / sizeof(db);
    lseek(fd_train, 0 * sizeof(train_db), SEEK_SET);
    while(read(fd_train, &train_db, sizeof(train_db))){
        if(train_db.flag == 1)
        count++;
        /*todo: Add seats */
        printf("count in loop : %d\n", count);
        printf("flag value : %d\n", train_db.flag);
    }
    printf("count : %d", count);
    if(count > 0){
        write(nsd, &count, sizeof(count));
        lseek(fd_train, 0 * sizeof(train_db), SEEK_SET);
        while(count-- > 0){
            read(fd_train, &train_db, sizeof(train_db));
            int seats_left = train_db.total_seats - train_db.occupied_seats;
            if(train_db.flag == 1){
                sleep(1);
                write(nsd, &train_db.train_name, sizeof(train_db.train_name));
                sleep(1);
                write(nsd, &train_db.train_id, sizeof(train_db.train_id));
                sleep(1);
                write(nsd, &train_db.src, sizeof(train_db.src));
                sleep(1);
                write(nsd, &train_db.dest, sizeof(train_db.dest));
                sleep(1);
                write(nsd, &seats_left, sizeof(seats_left));
            }
        }
    }
    else{
        count = -1;
        write(nsd, &count, sizeof(count));
        printf("<--------NO TRAINS IN DB------->\n");  
    }
}
/*-------------------------------------------------Authentication----------------------------------------------*/
int isAuthenticated(int nsd, int userid, char password[30], int type){
    struct flock lock;
	lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
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
    fcntl(fd, F_SETLKW, &lock); 
    lseek(fd, (userid - 1) * sizeof(db), SEEK_SET);
    read(fd, &db, sizeof(db));
    unlock(fd, lock);
    if(strcmp(password, db.password) == 0)
    return 1;
    else
    return 0;
}

void viewRecords(int nsd){
    struct flock lock;
	lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    printf("<----------------------viewRecords()-------------------------->\n");
    struct user db; 
    int count = 0;
    char path_db[100] = path; 
    strcat(path_db, "user_db.txt");
    int fd_cust = open(path_db, O_RDWR, 00777);
    printf("<-----------------------FILE LOCKED------------------------->\n");
    fcntl(fd_cust, F_SETLKW, &lock); 
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
    unlock(fd_cust, lock);
    
}

void signup_admin(int nsd){
    char username[30], password[30], typeofuser[30];
    printf("reached signup_admin page\n");
    read(nsd, &username, sizeof(username));
    printf("reached signup_admin page\n");
    read(nsd, &password, sizeof(password));
    read(nsd, &typeofuser, sizeof(typeofuser));
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
    struct flock lock;
	lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    
    int fd = open(path_db, O_RDWR, 00777); 
    printf("Entering Lock !\n");  
    fcntl(fd, F_SETLKW, &lock); 
    lseek(fd, (id - 1) * sizeof(db), SEEK_SET);
    read(fd, &db, sizeof(db));
    unlock(fd, lock);
    
    if(db.flag == 0)
    return 0;
    else
    return 1;
}

void deleteRecord(int nsd){
    struct flock lock;
	lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    int user_id;
    struct user db;
    int count = 0;
    char path_db[100] = path; 
    strcat(path_db, "user_db.txt"); 
    int fd_cust = open(path_db, O_RDWR, 00777);
    printf("LOCKED \n");
    fcntl(fd_cust, F_SETLKW, &lock);
    read(nsd, &user_id, sizeof(user_id));
    printf("user id from client : %d\n", user_id);
    lseek(fd_cust, (user_id - 1) * sizeof(db), SEEK_SET);
    read(fd_cust,&db, sizeof(db));
    printf("name : %s\n", db.user_name);
    db.flag = 0;
    lseek(fd_cust, (user_id - 1) * sizeof(db), SEEK_SET);
    write(fd_cust, &db, sizeof(db));
    unlock(fd_cust, lock);
}

void searchUser(int nsd){
    struct flock lock;
	lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    int user_id;
    struct user db;
    int count = 0;
    char path_db[100] = path; 
    strcat(path_db, "user_db.txt"); 
    int fd_cust = open(path_db, O_RDWR, 00777);

    printf("LOCKED \n");
    fcntl(fd_cust, F_SETLKW, &lock);
    read(nsd, &user_id, sizeof(user_id));
    lseek(fd_cust, (user_id - 1) * sizeof(db), SEEK_SET);
    read(fd_cust,&db, sizeof(db));
    printf("name : %s\n", db.user_name);
    printf("id : %d\n", db.user_id);
    printf("type : %d\n", db.user_type);
    unlock(fd_cust, lock);
    write(nsd, &db.user_name, sizeof(db.user_name));
    write(nsd, &db.user_id, sizeof(db.user_id));
    write(nsd, &db.user_type, sizeof(db.user_type));
}

void unlock(int fd, struct flock lock){
    printf("Press Enter to unlock \n");
    getchar();
    lock.l_type = F_ULOCK;
    printf("Unlocked \n");   
    fcntl(fd, F_SETLK, &lock);
}



/*

struct flock lock;
	lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    fcntl(fd_cust, F_SETLKW, &lock);

*/