#include <MKL25Z4.H>
#include "MMA8451.h"
#include <math.h>

#include "WAIT1.h"
#include "MMA1.h"

/* mjdargen */
#define M_PI_2 (M_PI/2.0)
#define M_PI_4 (M_PI/4.0)

#if 0 // used for atan2 test function
volatile float acc, app, error;
volatile float x, y;
#endif

int16_t acc_X=0, acc_Y=0, acc_Z=0;
float roll=0.0, pitch=0.0;

//mma data ready
extern uint32_t DATA_READY;

/*
 Initializes mma8451 sensor. I2C has to already be enabled.
 */
int init_mma() {
	  //check for device
		/*if(i2c_read_byte(MMA_ADDR, REG_WHOAMI) == WHOAMI)	{
			
		  //Delay(100);
		  WAIT1_Waitms(100);
		  //turn on data ready irq; defaults to int2 (PTA15)
		  i2c_write_byte(MMA_ADDR, REG_CTRL4, 0x01);
		  WAIT1_Waitms(100);
		  //set active 14bit mode and 100Hz (0x19)
		  i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x01);
				
		  //enable the irq in the NVIC
		  //NVIC_EnableIRQ(PORTA_IRQn);
		  return 1;
		}*/

	uint8_t res=ERR_OK;
	bool isEnabled;

	res = MMA1_Init();
	if (res!=ERR_OK) {
		for(;;) {} /* error? */
	}
	WAIT1_Waitms(10);
	res = MMA1_SetFastMode(FALSE);
	if (res!=ERR_OK) {
		for(;;) {} /* error? */
	}
	res = MMA1_isEnabled(&isEnabled);
	if(!isEnabled) {
		for(;;) {} /* error? */
	}

	//else error
	return 0;
	
}

/* 
  Reads full 16-bit X, Y, Z accelerations.
*/
void read_full_xyz()
{
	
	/*int i;
	uint8_t data[6];
	
	i2c_start();
	i2c_read_setup(MMA_ADDR , REG_XHI);
	
	for( i=0;i<6;i++)	{
		if(i==5)
			data[i] = i2c_repeated_read(1);
		else
			data[i] = i2c_repeated_read(0);
	}
	
	acc_X = (((int16_t) data[0])<<8) | data[1];
	acc_Y = (((int16_t) data[2])<<8) | data[3];
	acc_Z = (((int16_t) data[4])<<8) | data[5];*/

	acc_X = MMA1_GetX();
	acc_Y = MMA1_GetY();
	acc_Z = MMA1_GetZ();
}

// new implementation of approx_sqrtf uses union
float approx_sqrtf(float z) { // from Wikipedia
	union float_int val_int;
	const int a = 0x4c000;

	val_int.f = z; /* interpret float as int */
	val_int.i -= 1 << 23; /* Subtract 2^m. */
	val_int.i >>= 1; /* Divide by 2. */
	val_int.i += 1 << 29; /* Add ((b + 1) / 2) * 2^m. */
	val_int.i += a;
	// val_int.i = (1 << 29) + (val_int.i >> 1) - (1 << 22) + a;
	return val_int.f; /* Interpret again as float */
}

float approx_atan2f(float y, float x) {
	float a, abs_a, approx, adj=0.0;
	char negate = 0;
	
	if (x == 0) { // special cases
		if (y == 0.0)
			return 0.0; // undefined, but return 0 by convention
		else if (y < 0.0)
			return -M_PI_2;
		else
			return M_PI_2;
	}	else {
	// Version 7
#if 1		
		a = y/x;
		if (a>1) {
			a = x/y;
			adj = M_PI_2;
			negate = 1;
		} else if (a<-1) {
			a = x/y;
			adj = -M_PI_2;
			negate = 1;
		}
#endif

		abs_a = (a < 0)? -a : a;
		approx = M_PI_4*a - a*(abs_a - 1)*(0.2447+0.0663*abs_a);
		if (negate) {
			approx = adj - approx;
		}
		
		if (x > 0)
			return approx;
		else if (y >= 0)
			return approx + M_PI;
		else
			return approx - M_PI;
	}		
}

void convert_xyz_to_roll_pitch(void) {
/*
	float ax = acc_X/COUNTS_PER_G,
				ay = acc_Y/COUNTS_PER_G,
				az = acc_Z/COUNTS_PER_G;
*/
	
	roll = approx_atan2f(acc_Y, acc_Z)*(180/M_PI);
	pitch = approx_atan2f(acc_X, approx_sqrtf(acc_Y*acc_Y + acc_Z*acc_Z))*(180/M_PI);
#if 0	
	roll = atan2(ay, az)*180/M_PI;
	pitch = atan2(ax, sqrt(ay*ay + az*az))*180/M_PI;
#endif
}

