#define WINVER 0x0500

#include <windows.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

void keyDown(int key) {
	INPUT ip;
	// Set up a generic keyboard event.
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; // hardware scan code for key
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	
	// Press the "A" key
	ip.ki.wVk = key; // virtual-key code for the "a" key
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));
}
 
void keyUp(int key) {
	INPUT ip;
	// Set up a generic keyboard event.
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; // hardware scan code for key
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	
	// Release the "A" key
	ip.ki.wVk = key; // virtual-key code for the "a" key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

#define SERVER_PORT 4242

bool keystates[22] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
char buttons_n_h[22] = {'A', 'B', 'X', 'Y', 'L', 'R', 'U', 'D', 'L', 'R', 'U', 'D', 'L', 'R', 'U', 'D', 'P', 'M', 'Q', 'W', 'E', 'T'};
int SCval[22] = {0x41, 0x42, 0x58, 0x59, 0x25, 0x27, 0x26, 0x28, 0x25, 0x27, 0x26, 0x28, 0x25, 0x27, 0x26, 0x28, 0xBB, 0xBD, 0x51, 0x57, 0x45, 0x54};
const char * real_names[] = {"A", "B", "X", "Y", "LEFT", "RIGHT", "UP", "DOWN", "LEFT", "RIGHT", "UP", "DOWN", "LEFT", "RIGHT", "UP", "DOWN", "+", "-", "ZL", "ZR", "L", "R"};

int main()
{
	printf("== RemotePad client ==\n");
	printf("Starting client...\n");

	int sock;
	struct sockaddr_in name;
	struct hostent *hp, *gethostbyname();
	int bytes;
	
	// Create socket from which to read
	sock = socket(AF_INET, SOCK_DGRAM, 0);	
	if (sock < 0)
		return 1;
  
	// Bind our local address so that the client can send to us */
	memset((char*)&name, 0, sizeof(name));
	name.sin_family      = AF_INET;
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	name.sin_port        = htons(SERVER_PORT);
  
	if (bind(sock, (struct sockaddr *) &name, sizeof(name)))
		return 2;
	
	if (ntohs(name.sin_port) != 4242)
		return 3;
	
	printf("Ready for connections!\n");
	
	// Allocate buffer
	char * incoming = malloc(1024);

	while ((bytes = read(sock, incoming, 1024)) > 0) 
	{
		for (int i = 0; i < 22; i++) 
		{
			if (keystates[i] == false && incoming[i] == buttons_n_h[i]) 
			{
				keyDown(SCval[i]);
				keystates[i] = true;
				printf("Key Press:   %c (%s)\n", buttons_n_h[i], real_names[i]);
			} 
			else if (keystates[i] == true && incoming[i] != buttons_n_h[i]) 
			{
				keyUp(SCval[i]);
				keystates[i] = false;
				printf("Key Release: %c (%s)\n", buttons_n_h[i], real_names[i]);
			}
		}
	}
	
	free(incoming);
	
	close(sock);
	return 0;
}
