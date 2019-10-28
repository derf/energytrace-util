#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>
#include "MSP430.h"
#include "MSP430_EnergyTrace.h"
#include "MSP430_Debug.h"

typedef struct __attribute__((packed))  {
	uint8_t id;
	uint64_t timestamp:56;
	uint64_t state;
	uint32_t current;
	uint16_t voltage;
	uint32_t energy;
} event_t;

void push_cb(void* pContext, const uint8_t* pBuffer, uint32_t nBufferSize) {
	assert(sizeof(event_t)==26);
	assert(nBufferSize%sizeof(event_t)==0);
	uint32_t n=nBufferSize/sizeof(event_t);
	event_t *ev = (void*)pBuffer;
	uint32_t i = 0;
	while(i < n) {
		if(ev->id == 7) {
			printf("%016x %.9e %.9e %.9e %.9e\n", ev->state, ev->timestamp/1e6, ev->current/1e9, ev->voltage/1e3, ev->energy/1e7);
		}
		ev++;
		i++;
	}
}

void error_cb(void* pContext, const char* pszErrorText) {
	printf("error %s\n", pszErrorText);
}

void usage(char *a0) {
	printf("usage: %s <measurement duration in seconds>\n", a0);
}

void check_msp430_error(STATUS_T status) {
	if (status == STATUS_ERROR) {
		int32_t error_number = MSP430_Error_Number();
		char *error_str = MSP430_Error_String(error_number);
		printf("#MSP430 Error %d: %s\n", error_number, error_str);
	}
}

int main(int argc, char *argv[]) {
	if(argc<2) {
		usage(argv[0]);
		return 1;
	}
	unsigned int duration = strtod(argv[1], 0);
	if(duration == 0) {
		usage(argv[0]);
		return 1;
	}
	
	
	STATUS_T status = STATUS_OK;
	int32_t cpu_state = 0, cpu_cycles = 0;
	char* portNumber;
	int  version;
	long  vcc = 3300;
	union DEVICE_T device;
	portNumber = "TIUSB";
	
	printf("#Initializing the interface: ");
	status = MSP430_Initialize(portNumber, &version);
	printf("#MSP430_Initialize(portNumber=%s, version=%d) returns %d\n", portNumber, version, status);
	check_msp430_error(status);
	if(status != STATUS_OK) {
		return 1;
	}

	// 2. Set the device Vcc.
	printf("#Setting the device Vcc: ");
	status = MSP430_VCC(vcc);
	printf("#MSP430_VCC(%d) returns %d\n", vcc, status);
	check_msp430_error(status);

	status = MSP430_Configure(ET_CURRENTDRIVE_FINE, 1);
	printf("#MSP430_Configure(ET_CURRENTDRIVE_FINE, 1) =%d\n", status);
	check_msp430_error(status);

	status = MSP430_Configure(DEBUG_LPM_X, 1);
	printf("#MSP430_Configure(DEBUG_LPM_X, 1) =%d\n", status);
	check_msp430_error(status);


	// 3. Open the device.
	MSP430_LoadDeviceDb(NULL); //Required in more recent versions of tilib.
	printf("#Opening the device: ");
	status = MSP430_OpenDevice("DEVICE_UNKNOWN", "", 0, 0, DEVICE_UNKNOWN);
	printf("#MSP430_OpenDevice() returns %d\n", status);
	check_msp430_error(status);
	if(status != STATUS_OK) {
		return 1;
	}

	// 4. Get device information
	status = MSP430_GetFoundDevice((char*)&device, sizeof(device.buffer));
	printf("#MSP430_GetFoundDevice() returns %d\n", status);
	check_msp430_error(status);
	printf("# device.id: %d\n", device.id);
	printf("# device.string: %s\n", device.string);
	printf("# device.mainStart: 0x%04x\n", device.mainStart);
	printf("# device.infoStart: 0x%04x\n", device.infoStart);
	printf("# device.ramEnd: 0x%04x\n", device.ramEnd);
	printf("# device.nBreakpoints: %d\n", device.nBreakpoints);
	printf("# device.emulation: %d\n", device.emulation);
	printf("# device.clockControl: %d\n", device.clockControl);
	printf("# device.lcdStart: 0x%04x\n", device.lcdStart);
	printf("# device.lcdEnd: 0x%04x\n", device.lcdEnd);
	printf("# device.vccMinOp: %d\n", device.vccMinOp);
	printf("# device.vccMaxOp: %d\n", device.vccMaxOp);
	printf("# device.hasTestVpp: %d\n", device.hasTestVpp);
	
	
	EnergyTraceSetup ets = {  ET_PROFILING_ANALOG_DSTATE,                // Gives callbacks of with eventID 7
                      ET_PROFILING_10K,                   // N/A
                      ET_ALL,                             // Gives all JSTATE information
                      ET_EVENT_WINDOW_100,                // N/A
                      ET_CALLBACKS_ONLY_DURING_RUN };           // Callbacks are continuously
	EnergyTraceHandle ha;
	EnergyTraceCallbacks cbs = {
		.pContext = 0,
		.pPushDataFn = push_cb,
		.pErrorOccurredFn = error_cb
	};
	status = MSP430_EnableEnergyTrace(&ets, &cbs, &ha);
	printf("#MSP430_EnableEnergyTrace=%d\n", status);
	check_msp430_error(status);
	
	status = MSP430_ResetEnergyTrace(ha);
	printf("#MSP430_ResetEnergyTrace=%d\n", status);
	check_msp430_error(status);

	MSP430_Run(FREE_RUN, 0);
	printf("#MSP430_Run=%d\n", status);
	check_msp430_error(status);
	
	sleep(duration);

	status = MSP430_State(&cpu_state, 1, &cpu_cycles);
	printf("#MSP430_State=%d\n", status);
	check_msp430_error(status);
	printf("# CPU is %d, ran for %d cycles\n", cpu_state, cpu_cycles);
	
	status = MSP430_DisableEnergyTrace(ha);
	printf("#MSP430_DisableEnergyTrace=%d\n", status);
	check_msp430_error(status);
	
	printf("#Closing the interface: ");
	status = MSP430_Close(0);
	printf("#MSP430_Close(FALSE) returns %d\n", status);
	check_msp430_error(status);

	return 0;
}
