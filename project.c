#include <mega8.h>
#include <delay.h>

char anode_seg[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};

unsigned char minute = 0, hour = 12, second = 0, alarmH = 0, alarmM = 0;
unsigned char i;
bit setAlarm = 0;
bit alarm = 0;
void display_digit(unsigned char digit, unsigned char comm_value)
{
    PORTD = anode_seg[digit];
    PORTC = comm_value;
    delay_us(4);
    PORTC = 0x00;
}

void main(void)
{
    DDRD = 0xFF;
    PORTD = 0x00;
    
    DDRB = 0b00000011;
    PORTB = 0b00111100;    
    
    PORTC = 0x00;
    DDRC = 0xFF;
    
    ASSR = 0x08;
    TCCR2 = 0x05;
    TCNT2 = 0x00;
    OCR2 = 0x00;
    TIMSK = 0x40;
    #asm("sei")
      
    while (1)
    {   
        if(PINB.3 == 0){
            if(setAlarm==0){
                hour++;
                if (hour > 23) hour = 0;
            }else{
                alarmH++;
              if (alarmH > 23) alarmH = 0;
            }
            while(PINB.3==0);  
        }            
        
        if(PINB.4 == 0){
            if(setAlarm==0){
                minute++;
                if (minute > 59) minute = 0;
            }else{
                alarmM++;
                if(alarmM > 59) alarmM = 0;
            }             
            while(PINB.4==0);  
        }   
            
        if (PINB.5 == 0)
        {
           setAlarm = !setAlarm;
           alarm=1;
           while(PINB.5==0); 
        }
        if (PINB.2 == 0)
        {
           PORTB.0=0;
           alarm=0;
           while(PINB.5==0); 
        }
        if(setAlarm == 0){
            for (i = 0; i < 200; i++)
            {
            display_digit(hour / 10, 0b00000001);
            display_digit(hour % 10, 0b00000010);
            display_digit(minute / 10, 0b00000100);
            display_digit(minute % 10, 0b00001000);
            }
        }else{
            for (i = 0; i < 200; i++)
            {
            display_digit(alarmH / 10, 0b00000001);
            display_digit(alarmH % 10, 0b00000010);
            display_digit(alarmM / 10, 0b00000100);
            display_digit(alarmM % 10, 0b00001000);
            }
        }
        if((minute==alarmM) && (hour==alarmH) && alarm)PORTB.0=1;      
    }
}

interrupt[TIM2_OVF] void timer2_ovf_isr(void)
{
    second++;
    if (second > 59) second = 0;
    if (second == 0)
    {
        minute++;
        if (minute > 59) minute = 0;
        if (minute == 0)
        {
            hour++;
            if (hour > 23) hour = 0;
        }
    }
    if(setAlarm){
        PORTB.1=1;
    }else{
        PORTB.1=!PORTB.1;
    }
}