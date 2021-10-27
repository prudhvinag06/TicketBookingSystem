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
void helper(int sd, int client_id, char client_type[30]);
void viewRecords(int sd);


/*---------------------------------------------Client-------------------------------------------*/
int main(int argc, char * argv[]){
    struct sockaddr_in serv;
    char client_name[20], client_password[20], client_type[20];
    int client_id;
    int sd, connection_status, type;
    int client_number = 23;
    sd = socket(AF_INET, SOCK_STREAM, 0);
    serv.sin_family = AF_INET;
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
    printf("1. Login\n");
    printf("2. Sign Up\n");
    scanf("%d", &choice);
    while(choice != 1 && choice != 2){
        printf("Warning : Please select the valid option\n");
        scanf("%d", &choice);
    }
    if(choice == 1){ //Login 
        printf("Enter user id : ");
        scanf("%d", &client_id);
        printf("Enter Name : ");
        scanf("%s", client_name);
        printf("Enter password : \n");
        scanf("%s", client_password);
        printf("Enter type of User : \n");
        printf("1. Customer\n");
        printf("2. Admin\n");
        printf("3. Agent\n");
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
        int auth = 0;
        //authenticate password
        read(sd, &auth, sizeof(auth));
        while(auth == 0){
            printf("Error!. Please Enter Password again \n");
            scanf("%s", client_password);
            write(sd, &client_password, sizeof(client_password));
            read(sd, &auth, sizeof(auth));
            printf("Auth value : %d\n", auth);
        }
        printf("\tLOGGED IN SUCCESSFULLY\n");
        helper(sd, client_id, client_type);
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
        printf("1. Customer\n");
        printf("2. Admin\n");
        printf("3. Agent\n");
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
}

/*--------------------------------------------helper()---------------------------------------------------------------*/
void helper(int sd, int client_id, char client_type[30]){
    int type = -1;
    if(strcmp(client_type, "customer") == 0){
        type = 1;
    }
    else if(strcmp(client_type, "agent") == 0){
        type = 2; 
    }
    else type = 3;

    if(type == 1 || type == 2){
        int choice;
        printf("Select Options Below : \n");
        printf("1. Book Ticket\n");
		printf("2. View Bookings\n");
		printf("3. Update Booking\n");
		printf("4. Cancel booking\n");
        scanf("%d", &choice);
        write(sd, &choice, sizeof(choice));

    }
    else if(type == 3){
        int choice;
        printf("Select Options You Want To Add/Update Below : \n");
        printf("1. User Account \n");
        printf("2. Train Accounts \n");
        scanf("%d", &choice);
        write(sd, &choice, sizeof(choice));
        if(choice == 1){ //users
            printf("1. View Users \n");
            printf("2. Add User\n");
            //call modified signup() function
            printf("3. Delete User\n");
            printf("4. Modify User\n");
            printf("5. Search\n");
            scanf("%d", &choice);
            write(sd, &choice, sizeof(choice));
            switch (choice)
            {
            case 1:
            viewRecords(sd);
            break;
            case 2:
            break;
            case 3:
            //deleteUser();
            break;
            case 4:
            //modifyUser();
            break;
            case 5:
            //searchUser();
            break;
            default:
            break;
            }
        }
        else if(choice == 2){ //trains
            printf("1. View Trains");
            printf("2. Add Train\n");
            printf("3. Delete Train\n");
            printf("4. Modify Train Details\n");
            printf("5. Search Train\n");
            scanf("%d", &choice);
            write(sd, &choice, sizeof(choice));
        }


    }

}
void viewRecords(int sd){
   int user_id, count;
   char user_name[30];
   read(sd, &count, sizeof(count));
   printf("Total Number of users : %d\n", count);
   while(count--){
       read(sd, &user_name, sizeof(user_name));
       read(sd, &user_id, sizeof(user_id));
       printf("USER ID: %d  USER NAME : %s\n", user_id, user_name);
   }
    
}