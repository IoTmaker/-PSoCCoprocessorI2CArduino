# THIS FILE IS AUTOMATICALLY GENERATED
# Project: C:\Users\Zeus\Documents\PSoC Creator\CE211252_Ambient_Light_Sensing\CE211252_Ambient_Light_Sensing.cydsn\CE211252_Ambient_Light_Sensing.cyprj
# Date: Sun, 08 Jan 2017 01:40:47 GMT
#set_units -time ns
create_clock -name {Clock_PWM(FFB)} -period 83.333333333333329 -waveform {0 41.6666666666667} [list [get_pins {ClockBlock/ff_div_4}]]
create_clock -name {ADC_intSarClock(FFB)} -period 83.333333333333329 -waveform {0 41.6666666666667} [list [get_pins {ClockBlock/ff_div_12}]]
create_clock -name {I2C_1_SCBCLK(FFB)} -period 625 -waveform {0 312.5} [list [get_pins {ClockBlock/ff_div_0}]]
create_clock -name {EZI2C_SCBCLK(FFB)} -period 625 -waveform {0 312.5} [list [get_pins {ClockBlock/ff_div_1}]]
create_clock -name {CyILO} -period 25000 -waveform {0 12500} [list [get_pins {ClockBlock/ilo}]]
create_clock -name {CyLFCLK} -period 25000 -waveform {0 12500} [list [get_pins {ClockBlock/lfclk}]]
create_clock -name {CyIMO} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyHFCLK} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/hfclk}]]
create_clock -name {CySYSCLK} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/sysclk}]]
create_generated_clock -name {Clock_PWM} -source [get_pins {ClockBlock/hfclk}] -edges {1 3 5} [list]
create_generated_clock -name {ADC_intSarClock} -source [get_pins {ClockBlock/hfclk}] -edges {1 3 5} [list]
create_generated_clock -name {I2C_1_SCBCLK} -source [get_pins {ClockBlock/hfclk}] -edges {1 15 31} [list]
create_generated_clock -name {EZI2C_SCBCLK} -source [get_pins {ClockBlock/hfclk}] -edges {1 15 31} [list]


# Component constraints for C:\Users\Zeus\Documents\PSoC Creator\CE211252_Ambient_Light_Sensing\CE211252_Ambient_Light_Sensing.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Users\Zeus\Documents\PSoC Creator\CE211252_Ambient_Light_Sensing\CE211252_Ambient_Light_Sensing.cydsn\CE211252_Ambient_Light_Sensing.cyprj
# Date: Sun, 08 Jan 2017 01:40:45 GMT
