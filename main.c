/******************************************************************************
* Project Name      : CE211252_Ambient_Light_Sensing
* Version           : 1.0
* Device Used       : CY8C4A45LQI-L483
* Software Used     : PSoC Creator 3.3 CP3
* Compiler Used     : ARM GCC 4.9.3 
* Related Hardware  : CY8CKIT-048 PSoC Analog Coprocessor Pioneer Kit  
*******************************************************************************
* Copyright (2016), Cypress Semiconductor Corporation.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software") is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and 
* foreign), United States copyright laws and international treaty provisions. 
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the 
* Cypress source code and derivative works for the sole purpose of creating 
* custom software in support of licensee product, such licensee product to be
* used only in conjunction with Cypress's integrated circuit as specified in the
* applicable agreement. Any reproduction, modification, translation, compilation,
* or representation of this Software except as specified above is prohibited 
* without the express written permission of Cypress.
* 
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes to the Software without notice. 
* Cypress does not assume any liability arising out of the application or use
* of Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use as critical components in any products 
* where a malfunction or failure may reasonably be expected to result in 
* significant injury or death ("ACTIVE Risk Product"). By including Cypress's 
* product in a ACTIVE Risk Product, the manufacturer of such system or application
* assumes all risk of such use and in doing so indemnifies Cypress against all
* liability. Use of this Software may be limited by and subject to the applicable
* Cypress software license agreement.
*******************************************************************************/
/*******************************************************************************
* Theory of Operation: This code example demonstrates how to implement an analog 
* front end (AFE) for an ambient light sensor (ALS) using the PSoC Analog Coprocessor.
* The measured ALS current and the calculated light illuminance are sent over I2C 
* using the Cypress Bridge Control Panel program. Also, the light illuminance value 
* is used to control the brightness of an LED, such that the brightness corresponds 
* to the ambient light illuminance.
*******************************************************************************/

/* Header File Includes */
#include <project.h>

/* IIR Filter coefficient */
/* Cut off frequency = fs/(2 * pi * iir_filter_constant).  In this project fs ~= 1 ksps.
This results in a cut-off frequency of 15.91 Hz.  We are using IIR filter as FIR requires 
more order of filter to get the same cut-off frequency*/
#define FILTER_COEFFICIENT_ALS			        (10)

/* Constants for photodiode current calculation */
/* Scale Factor = (VREF / (2048 * 220K)) * 10^9 nA = 2.6633 
   As the TIA produces a negative voltage, the scale factor is made 
   negative */
#define ALS_CURRENT_SCALE_FACTOR_NUMERATOR		(-26633)
#define ALS_CURRENT_SCALE_FACTOR_DENOMINATOR	(10000)

/* Constants for ambient light calculation */
/* Scale Factor = 10000Lx / 3000nA = 3.333 */
#define ALS_LIGHT_SCALE_FACTOR_NUMERATOR		(3333)
#define ALS_LIGHT_SCALE_FACTOR_DENOMINATOR		(1000)

#define PWM_DUTY_SCALE	         (1u)	
#define PWM_DUTY_OFFSET	         (0)
#define ADC_CHANNEL_ALS          (0u)	
#define READ_WRITE_BOUNDARY      (0u)


/* The I2C Slave address by default in a PSoC device is 8 */
#define I2C_SLAVE_ADDRESS    (8u)
/* Set the write buffer length to be 16 bits or 2 bytes */
#define WR_BUFFER_SIZE       (2u)


/* Structure that holds the ALS current and the ambient light illuminance value  */
/* Use __attribute__((packed)) for GCC and MDK compilers to pack structures      */
/* For other compilers use the corresponding directive.                          */
/* For example, for IAR use the following directive                              */
/* typedef __packed struct {..}struct_name;                                      */
typedef struct __attribute__((packed))
{
	int16 alsCurrent;			/* Ambient light sensor current output */
	uint16 illuminance; 		/* Ambient light illuminance */	
}als_sensor_data;

/* Function Prototypes */
void InitResources(void);

/* Declare the i2cBuffer to exchange sensor data between Bridge Control 
Panel (BCP) and PSoC Analog Coprocessor */
als_sensor_data i2cBuffer = {0, 0};

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
* This function initializes all the resources, and in an infinite loop, performs 
* tasks to measure/calculate the ALS sensor output and to send the data over I2C
as slave to the Bridge Control Panel and as master to an Arduino Uno.
*
* Parameters:
* None
*
* Return:
* int
*
* Side Effects:
* None
*******************************************************************************/
int main()
{

	/* This variable is used to store the ADC result */
	int16 adcResult;
	
	/* These are used for firmware low pass filter input and output */
	int16 filterInput;
	int32 filterOutput = 0;
	
    /* Variable to store sensor current and light illuminance */
    int16 alsCurrent;
    uint16 illuminance;
    
	/* Variable to store the PWM Duty Cycle */
	unsigned int pwmDutyCycle;
    
    /* Variable to store the status returned by CyEnterCriticalSection()*/
    uint8 interruptState = 0;
	
	/* Enable global interrupts */
	CyGlobalIntEnable;
	
	/* Initialize hardware resources */
	InitResources();
    
    /*******************************************
    ********************************************
    *BELL addition begins
    ********************************************
    ********************************************/
    
    
    uint8 light;


    /*******************************************
    ********************************************
    *BELL addition ENDS
    ********************************************
    ********************************************/
    
	
	/* Infinite Loop */
	for(;;)
    {
		/* Check if the ADC data is ready */
		if(ADC_IsEndConversion(ADC_RETURN_STATUS))
		{			
			/* Get the ADC result */
			adcResult = ADC_GetResult16(ADC_CHANNEL_ALS);	
			
			/* Low pass filter the ADC result */
			filterInput = adcResult;
    		filterOutput = (filterInput + (FILTER_COEFFICIENT_ALS - 1)*filterOutput)/FILTER_COEFFICIENT_ALS;
    				
			/* Calculate the photodiode current */
			alsCurrent = (filterOutput * ALS_CURRENT_SCALE_FACTOR_NUMERATOR)/ALS_CURRENT_SCALE_FACTOR_DENOMINATOR; 
			
			/* If the calculated current is negative, limit it to zero */
			if(alsCurrent < 0)
			{
				alsCurrent = 0;
			}
			
			/* Calculate the light illuminance */
			illuminance = (alsCurrent * ALS_LIGHT_SCALE_FACTOR_NUMERATOR)/ALS_LIGHT_SCALE_FACTOR_DENOMINATOR;			
		
            /* Get the PWM duty cycle from the light illuminance value */
			pwmDutyCycle = ((unsigned int)illuminance*PWM_DUTY_SCALE)+PWM_DUTY_OFFSET;
	
			/* Limit the duty cycle */
			if(pwmDutyCycle>PWM_PWM_PERIOD_VALUE)
				pwmDutyCycle = PWM_PWM_PERIOD_VALUE;
			
			/* Update the PWM duty cycle */
			PWM_WriteCompare(PWM_PWM_PERIOD_VALUE-pwmDutyCycle);	
            
			/* Enter critical section to check if I2C bus is busy or not */
            interruptState = CyEnterCriticalSection();
        	if(!(EZI2C_EzI2CGetActivity() & EZI2C_EZI2C_STATUS_BUSY))
        	{
                /* Update I2C Buffer */
    			i2cBuffer.alsCurrent = alsCurrent;
    			i2cBuffer.illuminance = illuminance;
            }
            CyExitCriticalSection(interruptState);
		}
        
        
        /*******************************************
        ********************************************
        *BELL addition begins
        ********************************************
        ********************************************/
        
        /*  Set up I2C as master */
        /* Attempt to initiate communication with the slave until the function
         * completes without error.
         */
        
        do
        {
            /* automatically writes a buffer of data to a slave
             * device from start to stop.
              */
            light = I2C_1_I2CMasterWriteBuf(I2C_1_I2C_SLAVE_ADDRESS, (uint8 *)&i2cBuffer,
                                        WR_BUFFER_SIZE, I2C_1_I2C_MODE_COMPLETE_XFER);
        }
        while (light != I2C_1_I2C_MSTR_NO_ERROR);

        /* Wait for the data transfer to complete */
        while(I2C_1_I2CMasterStatus() & I2C_1_I2C_MSTAT_XFER_INP);
        
        /*******************************************
        ********************************************
        *BELL addition ENDS
        ********************************************
        ********************************************/
        
    }
}

/*******************************************************************************
* Function Name: void InitResources(void)
*****************  
* Summary:
*  This function initializes all the hardware resources
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*   None
*******************************************************************************/
void InitResources(void)
{
	/* Start the Scanning SAR ADC Component and start conversion */
	ADC_Start();
	ADC_StartConvert();

	/* Start the trans-impedance amplifier (TIA) */
	Opamp_TIA_Start();
    
    /* Start Reference buffer */
	RefBuffer_Start();
    
	/* Start Programmable Voltage Reference */
	PVref_Start();
    
    /* Enable Programmable Voltage Reference */
    PVref_Enable();
	
	/* Start PWM */
	PWM_Start();
    

	/* Start EZI2C Slave Component and initialize buffer */
	EZI2C_Start();
	EZI2C_EzI2CSetBuffer1(sizeof(i2cBuffer), READ_WRITE_BOUNDARY, (uint8*)&i2cBuffer);

    

        /*******************************************
        ********************************************
        *BELL addition begins
        ********************************************
        ********************************************/
	
    
    I2C_1_Start(); 
    
        
}


/* [] END OF FILE */
