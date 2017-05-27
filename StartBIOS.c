/*
 *  ======== StartBIOS.c ========
 */
#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
//#include <ti/sysbios/knl/Mailbox.h>
//#include <ti/sysbios/knl/Clock.h>
//#include <ti/sysbios/knl/Event.h>

/* Instrumentation headers */
#include <ti/uia/runtime/LogSnapshot.h>

/* Driverlib headers */
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/ssi.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

#include <ti/drivers/SPI.h>

/* Application headers */
#include"register.h"

#define BP1_IN_USE 1
#define BP2_IN_USE 0

#define SPI_DRIVER_API_IN_USE 1
#define LEGACY_SPI_IN_USE 0

#define SPI_MSG_LENGTH 1

void init_hw(void);

void send_direct_command(uint8_t command);
void spi_send_byte(uint8_t sendval);
uint8_t read_register(uint8_t reg);
void write_register(uint8_t* buffer);

#if LEGACY_SPI_IN_USE
uint32_t spi_receive_byte(void);
#endif
#if SPI_DRIVER_API_IN_USE
void spi_receive_byte(uint8_t reg);
#endif

void setup_task(void);
void taskfunction(UArg arg0);

/* Buffer for SPI Driver API and other SPI Stuff*/
#if SPI_DRIVER_API_IN_USE
	uint8_t masterTxBuffer[1];
	uint8_t masterRxBuffer[1];
	SPI_Handle spi_hendl;
	SPI_Params spiParams;
	SPI_Transaction spi_transaction;
#endif

 int main(void){
	uint32_t ui32SysClock = Board_initGeneral(120*1000*1000);

	init_hw();

	Board_initSPI();
#if LEGACY_SPI_IN_USE
	#if BP1_IN_USE
		SSIClockSourceSet(SSI2_BASE, SSI_CLOCK_SYSTEM);
		SSIConfigSetExpClk(SSI2_BASE, ui32SysClock, SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, 2 * 1000 * 1000, 8);
		SSIEnable(SSI2_BASE);
	#endif
	#if BP2_IN_USE

		SSIClockSourceSet(SSI3_BASE, SSI_CLOCK_SYSTEM);
		SSIConfigSetExpClk(SSI3_BASE, ui32SysClock, SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, 2 * 1000 * 1000, 8);
		SSIEnable(SSI3_BASE);
	#endif
#endif
	setup_task();

	BIOS_start();
	return 0;
}

void init_hw(void){
	uint32_t ui32Strength;
	uint32_t ui32PinType;

#if BP2_IN_USE
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);

	//Slave Select Pin High - PN5 BP2 - PB2 BP1
	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_5, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_5, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_5);
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0xFF);

	SysCtlDelay(160000);

	//Turn TRF7970 on - PN4 BP2 - PB3 BP1
	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_4, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_4, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_4);
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4, 0xFF);

	SysCtlDelay(200000);

	//Turn LEDs for Tag-indication off - PP3, PQ1, PM6 BP2 - PN2, PN3, PP2 BP1
	GPIOPadConfigGet(GPIO_PORTP_BASE, GPIO_PIN_3, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTP_BASE, GPIO_PIN_3, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_3, 0x00);

	GPIOPadConfigGet(GPIO_PORTQ_BASE, GPIO_PIN_1, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTQ_BASE, GPIO_PIN_1, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, GPIO_PIN_1);
	GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_1, 0x00);

	GPIOPadConfigGet(GPIO_PORTM_BASE, GPIO_PIN_6, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_6, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_6);
	GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, 0x00);
#endif

#if BP1_IN_USE

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //SPI SlaveSel PB2, TRF Enable PB3
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); //ISO15693 PN2, ISO14443A PN3
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP); //ISO14443B PP2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //MISO PD0, MOSI PD1, SPI Clock PD3
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); //IRQ PC7

	//Slave Select Pin High - PN5 BP2 - PB2 BP1
	GPIOPadConfigGet(GPIO_PORTB_BASE, GPIO_PIN_2, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_2, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xFF);
	SysCtlDelay(160000);

	//Turn TRF7970 on - PN4 BP2 - PB3 BP1
	GPIOPadConfigGet(GPIO_PORTB_BASE, GPIO_PIN_3, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_3, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xFF);

	SysCtlDelay(200000);

		//Turn LEDs for Tag-indication off - PP3, PQ1, PM6 BP2 - PN2, PN3, PP2 BP1
	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_2, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_2, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_2, 0x00);

	GPIOPadConfigGet(GPIO_PORTN_BASE, GPIO_PIN_3, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_3, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, 0x00);

	GPIOPadConfigGet(GPIO_PORTP_BASE, GPIO_PIN_2, &ui32Strength, &ui32PinType);
	GPIOPadConfigSet(GPIO_PORTP_BASE, GPIO_PIN_2, ui32Strength, GPIO_PIN_TYPE_STD);
	GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_2, 0x00);
#endif
}

void taskfunction(UArg arg0){
	uint8_t reg_write_buffer[2];
#if SPI_DRIVER_API_IN_USE
    SPI_Params_init(&spiParams);
    spiParams.mode = SPI_MASTER;
    spiParams.transferMode = SPI_MODE_BLOCKING;
    spiParams.dataSize = 8;
    spiParams.frameFormat = SPI_POL0_PHA1;
	#if BP1_IN_USE
    	spi_hendl = SPI_open(Board_SPI0, &spiParams);
	#endif
	#if BP2_IN_USE
    	spi_hendl = SPI_open(Board_SPI1, &spiParams);
    #endif
    if (spi_hendl == NULL) {
    	System_abort("Error initializing SPI\n");
    }else {
    	System_printf("SPI initialized\n");
        System_flush();
    }
    spi_transaction.count = SPI_MSG_LENGTH;
    spi_transaction.txBuf = (Ptr)masterTxBuffer;
    spi_transaction.rxBuf = (Ptr)masterRxBuffer;
#endif

	send_direct_command(TRF79XXA_SOFT_INIT_CMD);
	Task_sleep(10);
	send_direct_command(TRF79XXA_IDLE_CMD);
	Task_sleep(10);
	send_direct_command(TRF79XXA_RESET_FIFO_CMD);
	Task_sleep(10);

	reg_write_buffer[0] = TRF79XXA_MODULATOR_CONTROL;
	reg_write_buffer[1] = 0x01; 			// ASK 100%, no SYS_CLK output
	write_register(reg_write_buffer);
	Task_sleep(10);

	reg_write_buffer[0] = TRF79XXA_NFC_TARGET_LEVEL;
	reg_write_buffer[1] = 0x00;			 	// For TRF7970A Errata
	write_register(reg_write_buffer);
	Task_sleep(10);

	read_register(0x00);

	while(1){
		Task_sleep(500);
	}
}

void setup_task(void){
	Task_Params task_params_nfc;
	Task_Handle task_hendl_nfc;
	Error_Block eb;

	Error_init(&eb);
	Task_Params_init(&task_params_nfc);
	task_params_nfc.stackSize = 1024;
	task_params_nfc.priority = 13;
	task_hendl_nfc = Task_create((Task_FuncPtr)taskfunction, &task_params_nfc, &eb);
	if (task_hendl_nfc == NULL) {
		System_abort("Create NFC task failed");
    }
}

void send_direct_command(uint8_t command){
#if BP1_IN_USE
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0x00); //Start SPI Mode - BP1
#endif
#if BP2_IN_USE
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0x00); // Start SPI Mode - BP2
#endif

	// set Address/Command Word Bit Distribution to command
	command = (0x80 | command);					// command
	command = (0x9f & command);					// command code

	spi_send_byte(command);
#if BP1_IN_USE
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xFF); //Start SPI Mode - BP1
#endif
#if BP2_IN_USE
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0xFF); //Stop SPI Mode - BP2
#endif
}

void write_register(uint8_t* buffer){
#if BP1_IN_USE
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0x00); //Start SPI Mode - BP1
#endif
#if BP2_IN_USE
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0x00); // Start SPI Mode - BP2
#endif

	buffer[0] = (0x1f & buffer[0]);

	spi_send_byte(buffer[0]);
	spi_send_byte(buffer[1]);

#if BP1_IN_USE
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xFF); //Start SPI Mode - BP1
#endif
#if BP2_IN_USE
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0xFF); //Stop SPI Mode - BP2
#endif
}



void spi_send_byte(uint8_t sendval){
#if LEGACY_SPI_IN_USE
	#if BP1_IN_USE
		SSIDataPut(SSI2_BASE, sendval);
		while(SSIBusy(SSI2_BASE));
	#endif
	#if BP2_IN_USE
		SSIDataPut(SSI3_BASE, sendval);
		while(SSIBusy(SSI3_BASE));
	#endif
#endif
#if SPI_DRIVER_API_IN_USE
	uint8_t transmit_byte = sendval;
	spi_transaction.txBuf = (Ptr)&transmit_byte;
	if(SPI_transfer(spi_hendl, &spi_transaction));
	else{
		System_printf("SPI_Transfer failed!\n");
		System_flush();
	}
#endif
}

uint8_t read_register(uint8_t reg){
#if BP1_IN_USE
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0x00); //Start SPI Mode - BP1
#endif
#if BP2_IN_USE
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0x00); // Start SPI Mode - BP2
#endif
	uint32_t retval=0;

	// Address/Command Word Bit Distribution
	reg = (0x40 | reg); 			// address, read, single
	reg = (0x5f & reg);				// register address
#if LEGACY_SPI_IN_USE
	spi_send_byte(reg);
	retval = spi_receive_byte();
#endif

#if SPI_DRIVER_API_IN_USE
	spi_receive_byte(reg);
	System_printf("masterRxBuffer: %d, *masterRxBuffer: %d \n",masterRxBuffer,*masterRxBuffer);
	System_flush();
#endif

#if BP1_IN_USE
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xFF); //Start SPI Mode - BP1
#endif
#if BP2_IN_USE
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0xFF); //Stop SPI Mode - BP2
#endif

	System_printf("retval = %d\n", retval);
	System_flush();

	return 0;
}

#if LEGACY_SPI_IN_USE
uint32_t spi_receive_byte(void){
#endif
#if SPI_DRIVER_API_IN_USE
void spi_receive_byte(uint8_t reg){
#endif

#if LEGACY_SPI_IN_USE
	uint32_t retval;

	#if BP1_IN_USE
		SSIDataPut(SSI2_BASE, 0);
		while(SSIBusy(SSI2_BASE));

		SSIDataGet(SSI2_BASE, &retval);
		while(SSIBusy(SSI2_BASE));
	#endif
	#if BP2_IN_USE
		SSIDataPut(SSI3_BASE, 0);
		while(SSIBusy(SSI3_BASE));

		SSIDataGet(SSI3_BASE, &retval);
		while(SSIBusy(SSI3_BASE));
	#endif

	System_printf("retval in receive Byte: %d\n", retval);
	System_flush();
	return retval;
#endif

#if SPI_DRIVER_API_IN_USE

	if(SPI_transfer(spi_hendl, &spi_transaction));
	else{
		System_printf("SPI_Transfer failed!\n");
		System_flush();
	}
	System_printf("in receive_byte: masterRxBuffer: %d, *masterRxBuffer: %d \n",masterRxBuffer,*masterRxBuffer);
	System_flush();

#endif
}















/**** The code below is only for Instrumentation purposes! ****/

/* The redefinition of ti_uia_runtime_LogSnapshot_writeNameOfReference
 * is necessary due to bug UIA-23 fixed in UIA 2.00.06.52, cf.
 * http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/uia/2_00_06_52/exports/docs/uia_2_00_06_52_release_notes.html
 */
#undef ti_uia_runtime_LogSnapshot_writeNameOfReference
#define ti_uia_runtime_LogSnapshot_writeNameOfReference(refId, fmt, startAdrs, lengthInMAUs) \
( \
(ti_uia_runtime_LogSnapshot_putMemoryRange(ti_uia_events_UIASnapshot_nameOfReference, Module__MID, \
(IArg)refId,(IArg)__FILE__,(IArg)__LINE__, \
(IArg)fmt, (IArg)startAdrs, (IArg)lengthInMAUs)) \
)

/* Log the task name whenever a task is created.
 * This works around a limitation of UIA where tasks sharing a "main"
 * function do not show up separately in the execution analyzer, cf.
 * http://processors.wiki.ti.com/index.php/System_Analyzer_Tutorial_3A#Going_Further:_How_to_have_Analysis_tables_and_graphs_display_the_names_of_tasks_created_at_run_time
 */
#include <string.h>
Void tskCreateHook(Task_Handle hTask, Error_Block *eb) {
   String name = Task_Handle_name(hTask);
   LogSnapshot_writeNameOfReference(hTask, "Task_create: handle=%x", name, strlen(name)+1);
   ti_uia_runtime_LogSnapshot_writeNameOfReference(hTask, "Task_create: handle=%x", name, strlen(name)+1);
}
