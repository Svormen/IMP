/*
 * predmet: IMP projekt svetelna tabula
 * autor: Slavomir Svorada (xsvora02)
 * datum: 14.12.2021
 */
#include "MK60D10.h"

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1)<<(x & GPIO_PIN_MASK)))

// button UP(SW5) on FitKit3
#define BTN_SW5 0x4000000 // Port E, bit 26

// values for delay function
#define delayText1	222
#define delayText2	25

// define 'V' letter
#define v_letter1	(rows[0] | rows[1] | rows[2] | rows[3] | rows[4] | rows[5] | rows[6])
#define v_letter2	(rows[7])
#define v_letter3	(rows[0] | rows[1] | rows[2] | rows[3] | rows[4] | rows[5] | rows[6])
// define 'U' letter
#define u_letter1	(rows[0] | rows[1] | rows[2] | rows[3] | rows[4] | rows[5] | rows[6] | rows[7])
#define u_letter2	(rows[7])
#define u_letter3	(rows[0] | rows[1] | rows[2] | rows[3] | rows[4] | rows[5] | rows[6] | rows[7])
// define 'T' letter
#define t_letter1	(rows[0])
#define t_letter2	(rows[0] | rows[1] | rows[2] | rows[3] | rows[4] | rows[5] | rows[6] | rows[7])
#define t_letter3	(rows[0])
// define 'F' letter
#define f_letter1	(rows[0] | rows[1] | rows[2] | rows[3] | rows[4] | rows[5] | rows[6] | rows[7])
#define f_letter2	(rows[0] | rows[3])
#define f_letter3	(rows[0])
// define 'I' letter
#define i_letter1	(rows[0] | rows[7])
#define i_letter2	(rows[0] | rows[1] | rows[2] | rows[3] | rows[4] | rows[5] | rows[6] | rows[7])
#define i_letter3	(rows[0] | rows[7])


// initialize rows 1-8
int rows[9] = {
		0x4000000,	//R0
		0x1000000,	//R1
		0x200,		//R2
		0x2000000,	//R3
		0x10000000,	//R4
		0x80,		//R5
		0x8000000,	//R6
		0x20000000,	//R7
		0x0			//clear row
};

void PortsInit(void)
{
    /* Turn on all port clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTA_MASK;

    // Set PTA pins for GPIO functionality (column activators of 74HC154)
    PORTA->PCR[8] = ( 0|PORT_PCR_MUX(0x01) ); // A0
    PORTA->PCR[10] = ( 0|PORT_PCR_MUX(0x01) ); // A1
    PORTA->PCR[6] = ( 0|PORT_PCR_MUX(0x01) ); // A2
    PORTA->PCR[11] = ( 0|PORT_PCR_MUX(0x01) ); // A3

    // set PTA pins for GPIO functionality (rows selectors of 74HC154)
    PORTA->PCR[26] = ( 0|PORT_PCR_MUX(0x01) ); // R0
    PORTA->PCR[24] = ( 0|PORT_PCR_MUX(0x01) ); // R1
    PORTA->PCR[9] = ( 0|PORT_PCR_MUX(0x01) ); // R2
    PORTA->PCR[25] = ( 0|PORT_PCR_MUX(0x01) ); // R3
    PORTA->PCR[28] = ( 0|PORT_PCR_MUX(0x01) ); // R4
    PORTA->PCR[7] = ( 0|PORT_PCR_MUX(0x01) ); // R5
    PORTA->PCR[27] = ( 0|PORT_PCR_MUX(0x01) ); // R6
    PORTA->PCR[29] = ( 0|PORT_PCR_MUX(0x01) ); // R7

    // buttons
    PORTE->PCR[10] = PORT_PCR_MUX(0x01); // SW2
    PORTE->PCR[12] = PORT_PCR_MUX(0x01); // SW3
    PORTE->PCR[27] = PORT_PCR_MUX(0x01); // SW4
    PORTE->PCR[26] = PORT_PCR_MUX(0x01); // SW5
    PORTE->PCR[11] = PORT_PCR_MUX(0x01); // SW6

    /* Set corresponding PTE pins (output enable of 74HC154) for GPIO functionality */
    PORTE->PCR[28] = ( 0|PORT_PCR_MUX(0x01) ); // #EN

    /* Change corresponding PTA port pins as outputs */
    PTA->PDDR = GPIO_PDDR_PDD(0x3F000FC0);

    /* Change corresponding PTE port pins as outputs */
    PTE->PDDR = GPIO_PDDR_PDD( GPIO_PIN(28) );

    NVIC_ClearPendingIRQ(PORTE_IRQn);
    NVIC_EnableIRQ(PORTE_IRQn);
}

// function for delay text
void delayText(int del1, int del2, int num)
{
	int i, j;

	for(i=0; i<del1; i++) {
		for(j=0; j<del2; j++) {
			// turning the pixels of a particular row ON
			PTA->PDOR |= GPIO_PDOR_PDO(num);
		}
	}
	// clear before another function call
	PTA->PDOR &= GPIO_PDOR_PDO(rows[8]);
}

// function for word 'VUT'
void text(int numberColumn) {
	// letter 'V'
	column_select(numberColumn);
	delayText(delayText2, delayText1, v_letter1);
	column_select(numberColumn + 1);
	delayText(delayText2, delayText1, v_letter2);
	column_select(numberColumn + 2);
	delayText(delayText2, delayText1, v_letter3);

	// letter 'U'
	column_select(numberColumn + 4);
	delayText(delayText2, delayText1, u_letter1);
	column_select(numberColumn + 5);
	delayText(delayText2, delayText1, u_letter2);
	column_select(numberColumn + 6);
	delayText(delayText2, delayText1, u_letter3);

	// letter 'T'
	column_select(numberColumn + 8);
	delayText(delayText2, delayText1, t_letter1);
	column_select(numberColumn + 9);
	delayText(delayText2, delayText1, t_letter2);
	column_select(numberColumn + 10);
	delayText(delayText2, delayText1, t_letter3);
}

//function for word 'FIT'
void text2(int numberColumn) {
	// letter 'F'
	column_select(numberColumn);
	delayText(delayText2, delayText1, f_letter1);
	column_select(numberColumn + 1);
	delayText(delayText2, delayText1, f_letter2);
	column_select(numberColumn + 2);
	delayText(delayText2, delayText1, f_letter3);

	// letter 'I'
	column_select(numberColumn + 4);
	delayText(delayText2, delayText1, i_letter1);
	column_select(numberColumn + 5);
	delayText(delayText2, delayText1, i_letter2);
	column_select(numberColumn + 6);
	delayText(delayText2, delayText1, i_letter3);

	// letter 'T'
	column_select(numberColumn + 8);
	delayText(delayText2, delayText1, t_letter1);
	column_select(numberColumn + 9);
	delayText(delayText2, delayText1, t_letter2);
	column_select(numberColumn + 10);
	delayText(delayText2, delayText1, t_letter3);
}



/* Conversion of requested column number into the 4-to-16 decoder control.  */
// inspired from IMP_projekt-had_tabule_test.zip from: https://wis.fit.vutbr.cz/FIT/st/cfs.php.cs?file=%2Fcourse%2FIMP-IT%2Fprojects&cid=14662
void column_select(unsigned int col_num)
{
	unsigned i, result, col_sel[4];

	for (i =0; i<4; i++) {
		result = col_num / 2;	  // Whole-number division of the input number
		col_sel[i] = col_num % 2;
		col_num = result;

		switch(i) {

			// Selection signal A0
		    case 0:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(8))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(8)));
				break;

			// Selection signal A1
			case 1:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(10))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(10)));
				break;

			// Selection signal A2
			case 2:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(6))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(6)));
				break;

			// Selection signal A3
			case 3:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(11))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(11)));
				break;

			// Otherwise nothing to do...
			default:
				break;
		}
	}
}

// main function
int main(void)
{
    PortsInit();

    while (1) {
    	// move from right to left
    	for (int i=15; i>=0; i--) {

    		// firstly write 'VUT' on display and after holding button 'UP (SW5)' write 'FIT' on display
    		if (!(GPIOE_PDIR & BTN_SW5)) {
    			text2(i);
    		} else if (GPIOE_PDIR & BTN_SW5){
    			text(i);
    		}

    	}
    }

    return 0;
}
