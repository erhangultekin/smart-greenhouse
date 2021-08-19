	# include < reg51.h >

sbit M1 = P3^4;
sbit M2 = P3^5;
sbit M3 = P3^6;
sbit M4 = P3^7;
//sbit Tavan_Acik = P2^4;
sbit SU = P2^5; //Sulama motoru çalistirmak için
sbit Sulama_Button = P2^6;
//sbit Tavan_Kapali = P2^7;

sbit ADC_RD = P3^0;
sbit ADC_WR = P3^1;
sbit ADC_INT = P3^2;

sbit ADC0 = P1^0;
sbit ADC1 = P1^1;
sbit ADC2 = P1^2;
sbit ADC3 = P1^3;
sbit ADC4 = P1^4;
sbit ADC5 = P1^5;
sbit ADC6 = P1^6;
sbit ADC7 = P1^7;

unsigned int temperature_set = 25;
unsigned int temperature_measured = 25;

unsigned int su_time = 30; //sn
unsigned int fifty_ms0 = 0;
unsigned int one_sec0 = 0;

unsigned int motor_time = 10; //sn
unsigned int fifty_ms1 = 0;
unsigned int one_sec1 = 0;

unsigned int k = 0;
unsigned int l = 0;

unsigned char roof_state = 0; //kapali

void init(void);
void read_adc(void);
void Timer0_ISR(void);
void Timer1_ISR(void);
void Delay_1sec(void);
void Delay_50usec(void);

void open_roof(void);
void close_roof(void);

void main(void) {

    M1 = 0;
    M2 = 0;
    M3 = 0;
    M4 = 0;
    SU = 0;
    init();
    while (1) {
			
			if(Sulama_Button == 0){
					while(!Sulama_Button);
					SU = 1;
					TR0 = 1;
				}
      read_adc();
			
			if(temperature_measured > temperature_set + 2){
				open_roof();
			}
			else if(temperature_measured < temperature_set - 2){
				close_roof();
			}
			else{
				M1 = 0;
    M2 = 0;
    M3 = 0;
    M4 = 0;
			}
        
        
    }
}
void init(void) {
    TMOD = 0x11; // Timer1 in Mode1 and Timer0 in Mode1
    TH1 = 0x4B; // Load timer value
    TL1 = 0xFE;
	ET1 = 1;
    
    TH0 = 0x4B; // Load timer value
    TL0 = 0xFE;
    ET0 = 1; // Enable Timer0 Interrupt
    EA = 1; // Enable Global Interrupt bit
}

void open_roof(void) {
	if(roof_state == 0){
    
		M1 = 1;
		M2 = 0;
    M3 = 0;
    M4 = 1;
	TR1 = 1;
	roof_state = 1;

	}
}

void close_roof(void) {
	if(roof_state == 1){
	M1 = 0;
    M2 = 1;
    M3 = 1;
    M4 = 0;
		TR1 = 1;
	roof_state = 0;
	
	}
}


void read_adc(void){
	ADC_WR = 0;
	Delay_50usec();
	ADC_WR = 1;
	Delay_50usec();
	Delay_50usec();
	Delay_50usec();
	Delay_50usec();
	ADC_RD = 1;
	Delay_50usec();
	ADC_RD = 0;
	temperature_measured = P1*2;
}
	
void Timer0_ISR(void)interrupt 1 {
    fifty_ms0 += 1;
    if (fifty_ms0 == 20) {
        one_sec0 += 1;
        fifty_ms0 = 0;
    }

    if (one_sec0 >= su_time && SU == 1) {
      SU = 0;  
			TR0 = 0;
      one_sec0 = 0;
    }
    TH0 = 0x4B; // Load timer value
    TL0 = 0xFE;
    TF0 = 0; // Clear the interrupt flag
}

void Timer1_ISR(void)interrupt 3 {
    fifty_ms1 += 1;
    if (fifty_ms1 == 20) {
        one_sec1 += 1;
        fifty_ms1 = 0;
    }

    if (one_sec1 >= motor_time) {
    TR1 = 0;
      one_sec1 = 0;
		M1 = 0;
    M2 = 0;
    M3 = 0;
    M4 = 0;
			roof_state = ~roof_state;
    }    
    
    TH1 = 0x4B; // Load timer value
    TL1 = 0xFE;
    TF1 = 0; // Clear the interrupt flag
}

void Delay_1sec(void) {
    unsigned int k = 0;
    for (k = 0; k < 20; k++) {
        TH1 = 0x4B; // Load timer value
        TL1 = 0xFE;
        TR1 = 1; // timer0 start
        while (TF1 == 0); // check overflow condition
        TR1 = 0; // Stop Timer
        TF1 = 0; // Clear flag
    }
}

void Delay_50usec(void) {
    TH1 = 0x4B; // Load timer value
    TL1 = 0xFE;
    TR1 = 1; // timer0 start
    while (TF1 == 0); // check overflow condition
    TR1 = 0; // Stop Timer
    TF1 = 0; // Clear flag
}