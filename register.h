/*
 * register.h
 *
 *  Created on: 27. Mai 2017
 *      Author: Raphael
 */

#ifndef REGISTER_H_
#define REGISTER_H_



//---- Direct commands ------------------------------------------

#define TRF79XXA_IDLE_CMD                    			0x00
#define TRF79XXA_SOFT_INIT_CMD               			0x03
#define TRF79XXA_INITIAL_RF_COLLISION_AVOID_CMD			0x04
#define TRF79XXA_PERFORM_RES_RF_COLLISION_AVOID_CMD		0x05
#define TRF79XXA_PERFORM_RES_RF_COLLISION_AVOID_N0_CMD	0x06
#define TRF79XXA_RESET_FIFO_CMD              			0x0F
#define TRF79XXA_TRANSMIT_NO_CRC_CMD         			0x10
#define TRF79XXA_TRANSMIT_CRC_CMD            			0x11
#define TRF79XXA_DELAY_TRANSMIT_NO_CRC_CMD   			0x12
#define TRF79XXA_DELAY_TRANSMIT_CRC_CMD      			0x13
#define TRF79XXA_TRANSMIT_NEXT_SLOT_CMD      			0x14
#define TRF79XXA_CLOSE_SLOT_SEQUENCE_CMD     			0x15
#define TRF79XXA_STOP_DECODERS_CMD           			0x16
#define TRF79XXA_RUN_DECODERS_CMD            			0x17
#define TRF79XXA_TEST_INTERNAL_RF_CMD        			0x18
#define TRF79XXA_TEST_EXTERNAL_RF_CMD        			0x19

//---- Registers ------------------------------------------------

#define TRF79XXA_CHIP_STATUS_CONTROL			0x00
#define TRF79XXA_ISO_CONTROL					0x01
#define TRF79XXA_ISO14443B_TX_OPTIONS			0x02
#define TRF79XXA_ISO14443A_BITRATE_OPTIONS		0x03
#define TRF79XXA_TX_TIMER_EPC_HIGH				0x04
#define TRF79XXA_TX_TIMER_EPC_LOW				0x05
#define TRF79XXA_TX_PULSE_LENGTH_CONTROL		0x06
#define TRF79XXA_RX_NO_RESPONSE_WAIT_TIME		0x07
#define TRF79XXA_RX_WAIT_TIME					0x08
#define TRF79XXA_MODULATOR_CONTROL				0x09
#define TRF79XXA_RX_SPECIAL_SETTINGS			0x0A
#define TRF79XXA_REGULATOR_CONTROL				0x0B
#define TRF79XXA_IRQ_STATUS						0x0C
#define TRF79XXA_IRQ_MASK						0x0D
#define	TRF79XXA_COLLISION_POSITION				0x0E
#define TRF79XXA_RSSI_LEVELS					0x0F
#define TRF79XXA_SPECIAL_FUNCTION_1				0x10	// Register not documented on TRF7960A Datasheet, See sloa155 Section 7.8
#define TRF79XXA_SPECIAL_FUNCTION_2				0x11
#define TRF79XXA_FIFO_IRQ_LEVELS				0x14
#define TRF79XXA_NFC_LOW_DETECTION_LEVEL		0x16
#define TRF79XXA_NFC_ID_REG                 	0x17
#define TRF79XXA_NFC_TARGET_LEVEL				0x18
#define TRF79XXA_NFC_TARGET_PROTOCOL			0x19
#define TRF79XXA_TEST_SETTINGS_1				0x1A
#define TRF79XXA_TEST_SETTINGS_2				0x1B
#define TRF79XXA_FIFO_STATUS					0x1C
#define TRF79XXA_TX_LENGTH_BYTE_1				0x1D
#define TRF79XXA_TX_LENGTH_BYTE_2				0x1E
#define TRF79XXA_FIFO							0x1F



#endif /* REGISTER_H_ */
