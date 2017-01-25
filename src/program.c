#include "program.h"
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "udp.h"
#include "system/memory.h"
#include "IP_ADRESS_EDIT_THIS.h"

u32  buttons_val[22] = {VPAD_BUTTON_A, VPAD_BUTTON_B, VPAD_BUTTON_X, VPAD_BUTTON_Y, VPAD_BUTTON_LEFT, VPAD_BUTTON_RIGHT, VPAD_BUTTON_UP, VPAD_BUTTON_DOWN, VPAD_STICK_R_EMULATION_LEFT, VPAD_STICK_R_EMULATION_RIGHT, VPAD_STICK_R_EMULATION_UP, VPAD_STICK_R_EMULATION_DOWN, VPAD_STICK_L_EMULATION_LEFT, VPAD_STICK_L_EMULATION_RIGHT, VPAD_STICK_L_EMULATION_UP, VPAD_STICK_L_EMULATION_DOWN
, VPAD_BUTTON_PLUS, VPAD_BUTTON_MINUS, VPAD_BUTTON_ZL, VPAD_BUTTON_ZR, VPAD_BUTTON_L, VPAD_BUTTON_R};
char buttons_n_h[22] = {'A', 'B', 'X', 'Y', 'L', 'R', 'U', 'D', 'L', 'R', 'U', 'D', 'L', 'R', 'U', 'D', 'P', 'M', 'Q', 'W', 'E', 'T'};
char buttons_n_r[22] = {'a', 'b', 'x', 'y', 'l', 'r', 'u', 'd', 'l', 'r', 'u', 'd', 'l', 'r', 'u', 'd', 'p', 'm', 'q', 'w', 'e', 't'};

int _entryPoint()
{
	InitOSFunctionPointers();
	InitSocketFunctionPointers();
	InitVPadFunctionPointers();

	udp_init(IP_ADRESS);

	memoryInitialize();

	// Init screen and screen buffers
	OSScreenInit();
	u8 *ScreenBuffer0 = MEM1_alloc(OSScreenGetBufferSizeEx(0), 0x40);
	u8 *ScreenBuffer1 = MEM1_alloc(OSScreenGetBufferSizeEx(1), 0x40);
	OSScreenSetBufferEx(0, ScreenBuffer0);
	OSScreenSetBufferEx(1, ScreenBuffer1);
	OSScreenEnableEx(0, 1);
	OSScreenEnableEx(1, 1);

	// Clear screens
	OSScreenClearBufferEx(0, 0);
	OSScreenClearBufferEx(1, 0);

	// print to TV
	OSScreenPutFontEx(0, 0, 0, "== RemotePad ==");
	OSScreenPutFontEx(0, 0, 1, "Press HOME-Button to exit.");

	// print to DRC
	OSScreenPutFontEx(1, 0, 0, "== RemotePad ==");
	OSScreenPutFontEx(1, 0, 1, "Press HOME-Button to exit.");

	// Flip buffers
	OSScreenFlipBuffersEx(0);
	OSScreenFlipBuffersEx(1);

	char msg_data[23];
	msg_data[22] = 0;

	int error;
	VPADData vpad_data;

	for(;;) {
		VPADRead(0, &vpad_data, 1, &error);
		DCFlushRange(&vpad_data, sizeof(VPADData));
		//Get the status of the gamepad
		for(int i=0; i<22; i++)
			msg_data[i] = (vpad_data.btns_h & buttons_val[i]) ? buttons_n_h[i] : buttons_n_r[i];

		udp_printf("%s", msg_data);

		usleep(10000); // I guess it should be enough? Make this value smaller for faster refreshing
		if (vpad_data.btns_h & VPAD_BUTTON_HOME)
			break;
	}

	MEM1_free(ScreenBuffer0);
	MEM1_free(ScreenBuffer1);
	ScreenBuffer0 = NULL;
	ScreenBuffer1 = NULL;

	return 0;
}
