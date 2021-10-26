#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include <arpa/inet.h> 
#include<netinet/in.h>
#include <string.h>
#define port 5555
int isAuthenticated(int client_number, char client_username[], char client_type[]);
/*---------------------------------------------Client-------------------------------------------*/
int main(int argc, char * argv[]){
    struct sockaddr_in serv;
    char client_name[20], client_password[20], client_type[20];
    int client_id;
    int sd, connection_status, type;
    int client_number = 23;
    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    serv.sin_family = AF_UNIX;
    serv.sin_addr.s_addr = INADDR_ANY;   //inet_addr("ser ip") //in case of not a local host  
    serv.sin_port = htons(port);
    connection_status = connect(sd, (struct sockaddr *) &serv, sizeof(serv));
    if(connection_status == -1){
        printf("Error\n");
    }
/*--------------------------------------------Initialization------------------------------------*/
    int choice;
    write(sd, &client_number, sizeof(client_number));
    //read(sd, &client_number, sizeof(client_number));
    //printf("Server Says : %d\n", client_number);
    printf("Select the type : \n");
    printf("1.Login\n");
    printf("2.Sign Up\n");
    scanf("%d", &choice);
    while(choice != 1 && choice != 2){
        printf("Warning : Please select the valid option\n");
        scanf("%d", &choice);
    }
    if(choice == 1){ //Login 
        printf("Enter user id : \n");
        scanf("%d", &client_id);
        printf("Enter Name : \n");
        scanf("%s", client_name);
        printf("Enter password : \n");
        scanf("%s", client_password);
        printf("Enter type of User : \n");
        printf("1.Customer\n");
        printf("2.Admin\n");
        printf("3.Agent\n");
        scanf("%d", &type);
        if(type == 1){
            strcpy(client_type, "customer");
        }
        else if(type == 2){
            strcpy(client_type, "admin");
        }
        else{
            strcpy(client_type, "agent");
        }
        // write(sd, &client_id, sizeof(client_id));
        int val = 1;
        write(sd, &val, sizeof(val));
        write(sd, &client_id, sizeof(client_id));
        write(sd, &client_name, sizeof(client_name));
        write(sd, &client_password, sizeof(client_password));
        write(sd, &client_type, sizeof(client_type));
        /*
        Can add if user doesn't exist u can call signup function
        */
    }
    else if(choice == 2){//Signup
        //signup();
        // printf("Enter user id : \n");
        // scanf("%d", &client_id);
        int user_id;
        printf("Enter Name : \n");
        scanf("%s", client_name);
        printf("Enter password : \n");
        scanf("%s", client_password);
        printf("Select Type of user : \n");
        printf("1.Customer\n");
        printf("2.Admin\n");
        printf("3.Agent\n");
        scanf("%d", &type);
        if(type == 1){
            strcpy(client_type, "customer");
        }
        else if(type == 2){
            strcpy(client_type, "admin");
        }
        else{
            strcpy(client_type, "agent");
        }
        // write(sd, &client_id, sizeof(client_id));
        int val = 2;
        write(sd, &val, sizeof(val));
        write(sd, &client_name, sizeof(client_name));
        write(sd, &client_password, sizeof(client_password));
        write(sd, &client_type, sizeof(client_type));
        read(sd, &user_id, sizeof(user_id));
        printf("Login id of user : %d\n", user_id);
    }

    // while(!isAuthenticated(client_number, client_password, client_type)){
    //     printf("Enter password : \n");
    //     scanf("%s", client_password);
    // }
   // close(sd);
}