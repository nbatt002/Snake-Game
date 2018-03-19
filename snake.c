/*
 * GccApplication1.c
 *
 * Created: 3/14/2018 8:08:44 PM
 *  Author: student
 */


#include <avr/io.h>
//#include <util/delay.h>
#include "bit.h"
#include "timer.h"
#include "nokia5110.h"
#include "io.h"
#include <avr/eeprom.h>

//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
    unsigned long int c;
    while(1){
        c = a%b;
        if(c==0){return b;}
        a = b;
        b = c;
    }
    return 0;
}
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
    /*Tasks should have members that include: state, period,
     a measurement of elapsed time, and a function pointer.*/
    signed char state; //Task's current state
    unsigned long int period; //Task period
    unsigned long int elapsedTime; //Time elapsed since last task tick
    int (*TickFct)(int); //Task tick function
} task;

//--------End Task scheduler data structure-----------------------------------


void ADC_init()
{
    ADMUX = (1<<REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

//Read from the ADC channel ch
uint16_t ReadADC(uint8_t ch)
{
    //Select ADC Channel
    ch &= 0x07;
    ADMUX = (ADMUX & 0xF8) | ch;
    
    //Start Single conversion
    ADCSRA |= (1 << ADSC);
    
    //Wait for conversion to complete
    while(ADCSRA & (1 << ADSC));
    
    //Clear ADIF
    ADCSRA |= (1<<ADIF);
    return (ADC);
}

int key = 0;
int16_t U_D;
int16_t L_R;
int i;
int fruit_arr [2];
int temp_snakehead[2];
//xh  yh |       |       |       |       |
int snake_arr [12] = {42, 24, 43, 24, 44, 24, 45, 24, 46, 24, 47, 24};
char ndirection;
char pdirection;
unsigned char score = 0;
unsigned char temp_score = 0;
unsigned char highscore = 0;
int temp;
int var = 0;

unsigned int cnt = 0;
unsigned char hover; //let's us know what we are displaying
unsigned char button; //our selector for the LCD
//our LCD Menu, has options: >Start Game, >Instructions, >High Score, >Reset
enum LCD_Menu{Welcome, MenuScreen, Hover1_SG, GameW, Game, Hover2_IW, Hover2_Instruct, InstructW, Instructions, Hover3_HSW, Hover3_HighScore, HighSW, HighScore, Hover4_hsRW, Hover4_Reset} state;
void TickFct_LCR_Menu()
{
    U_D = ReadADC(1);
    U_D -= 512;
    while(key == 0)
    {
        
        switch(state)//Transitions
        {
            case Welcome:
                state = MenuScreen;
                LCD_DisplayString(1, "Welcome to Snake   ~<:<<<<<>");
                hover = 1;
                break;
                
                
            case MenuScreen:
                if(hover == 1)
                {
                    if (cnt > 30)
                    {
                        state = Hover1_SG;
                        LCD_DisplayString(1, "> Start Game    > Instructions");
                        LCD_Cursor(1);
                    }
                }
                else
                {
                    state = Hover1_SG;
                    LCD_DisplayString(1, "> Start Game    > Instructions");
                    LCD_Cursor(1);
                }
                break;
                
                
            case Hover1_SG:
                hover = 0;
                if((U_D <= 250) && (U_D > -249))
                {
                    state = Hover1_SG;
                }
                else
                {
                    if(U_D > 0) //up
                    {
                        state = Hover1_SG;
                    }
                    else if(U_D < 0) //down
                    {
                        state = Hover2_IW;
                    }
                }
                break;
                
            case GameW:
                LCD_DisplayString(1, "play the goddamn thing");
                break;
                
            case Game:
                key = 1;
                /*button = GetBit(~PINA, 3);
                 
                 if (!button)
                 {
                 state = Game;
                 }
                 
                 else if (button)
                 {
                 state = MenuScreen;
                 }*/
                break;
                
            case Hover2_IW:
                state = Hover2_Instruct;
                break;
                
            case Hover2_Instruct:
                if((U_D <= 250) && (U_D > -249))
                {
                    state = Hover2_Instruct;
                }
                
                else
                {
                    if(U_D > 0) //up
                    {
                        state = MenuScreen;
                    }
                    else if(U_D < 0) //down
                    {
                        state = Hover3_HSW;
                    }
                }
                break;
                
            case InstructW:
                LCD_DisplayString(1, "Eat fruit, don'teat self or walls");
                break;
                
            case Instructions:
                button = GetBit(~PINA, 3);
                
                if (!button)
                {
                    state = Instructions;
                }
                
                else if (button)
                {
                    state = Hover2_IW;
                }
                break;
                
            case Hover3_HSW:
                state = Hover3_HighScore;
                break;
                
            case Hover3_HighScore:
                if((U_D <= 250) && (U_D > -249))
                {
                    state = Hover3_HighScore;
                }
                
                else
                {
                    if(U_D > 0) //up
                    {
                        state = Hover2_IW;
                    }
                    else if(U_D < 0) //down
                    {
                        state = Hover4_hsRW;
                    }
                }
                break;
                
            case HighSW:
                LCD_DisplayString(1, "High Score:");
                LCD_DisplayString(2, highscore);
                break;
                
            case HighScore:
                button = GetBit(~PINA, 3);
                
                if (!button)
                {
                    state = HighScore;
                }
                
                else if (button)
                {
                    state = Hover3_HSW;
                }
                break;
                
            case Hover4_hsRW:
                state = Hover4_Reset;
                
            case Hover4_Reset:
                if((U_D <= 250) && (U_D > -249))
                {
                    state = Hover4_Reset;
                }
                
                else
                {
                    if(U_D > 0) //up
                    {
                        state = Hover3_HSW;
                    }
                    else if(U_D < 0) //down
                    {
                        //state = Hover4_hsRW;
                    }
                }
                break;
                
        }
        switch(state)//Actions
        {
            case Welcome:
                break;
                
            case MenuScreen:
                cnt++;
                break;
                
            case Hover1_SG:
                button = GetBit(~PINA, 2);
                
                if (!button)
                {
                    state = Hover1_SG;
                }
                else if (button)
                {
                    state = GameW;
                }
                break;
                
            case GameW:
                state = Game;
                break;
                
            case Game:
                if (key == 1)
                {
                    break;
                }
                else if (key == 0)
                {
                    state = MenuScreen;
                }
                break;
                
            case Hover2_IW:
                LCD_DisplayString(1, ">Instructions   >High Score");
                LCD_Cursor(1);
                break;
                
            case Hover2_Instruct:
                button = GetBit(~PINA, 2);
                
                if (!button)
                {
                    state = Hover2_Instruct;
                }
                else if(button)
                {
                    state = InstructW;
                }
                break;
                
            case InstructW:
                state = Instructions;
                break;
                
            case Instructions:
                break;
                
            case Hover3_HSW:
                LCD_DisplayString(1, ">High Score     >Reset");
                LCD_Cursor(1);
                break;
                
            case Hover3_HighScore:
                button = GetBit(~PINA, 2);
                
                if (!button)
                {
                    state = Hover3_HighScore;
                }
                else if(button)
                {
                    state = HighSW;
                }
                break;
                
            case HighSW:
                state = HighScore;
                break;
                
            case HighScore:
                break;
                
            case Hover4_hsRW:
                LCD_DisplayString(1, ">Reset     ");
                LCD_Cursor(1);
                break;
                
            case Hover4_Reset:
                temp = 0;
                eeprom_write_byte((uint8_t*)23, temp);
                break;
        }
        return state;
    }
}


enum Snake {generate, breathe, no_input, input, iteration, check, dead} sstate;
void TickFct_Snake()
{
    U_D = ReadADC(0);
    U_D -= 512;
    L_R = ReadADC(1);
    L_R -= 512;
    while(key == 1)
    {
        
        switch(sstate) //transitions
        {
            case generate:
                //------------------------walls----------------------
                // left wall
                for (i = 0; i < 48; i++)
                {
                    nokia_lcd_set_pixel(0,i,1);
                }
                
                // top wall
                for (i = 0; i < 84; i++)
                {
                    nokia_lcd_set_pixel(i,0,1);
                }
                
                // right wall
                for (i = 0; i < 48; i++)
                {
                    nokia_lcd_set_pixel(83,i,1);
                }
                
                // bottom wall
                for (i = 0; i < 84; i++)
                {
                    nokia_lcd_set_pixel(i,47,1);
                }
                //------------------------walls----------------------
                
                //------------------------fruit----------------------
                //x-value of fruit location
                fruit_arr[0] = rand()%83;
                
                //y-value of fruit location
                fruit_arr[1] = rand()%47;
                
                nokia_lcd_set_pixel(fruit_arr[0], fruit_arr[1], 1);
                //------------------------fruit----------------------
                
                //------------------------snake----------------------
                nokia_lcd_set_pixel(snake_arr[0],snake_arr[1],1);
                nokia_lcd_set_pixel(snake_arr[2],snake_arr[3],1);
                nokia_lcd_set_pixel(snake_arr[4],snake_arr[5],1);
                nokia_lcd_set_pixel(snake_arr[6],snake_arr[7],1);
                nokia_lcd_set_pixel(snake_arr[8],snake_arr[9],1);
                nokia_lcd_set_pixel(snake_arr[10],snake_arr[11],1);
                
                pdirection = 'l';
                ndirection = 'l';
                //------------------------snake----------------------
                //nokia_lcd_render();
                break;
                
            case breathe:
                if(((U_D <= 250) && (U_D > -249)) && ((L_R <= 250) && (L_R > -249)))
                {
                    sstate = no_input;
                }
                else if ((U_D > 250) || (U_D <= -249))
                {
                    if (U_D < 0) // up
                    {
                        pdirection = ndirection;
                        ndirection = 'u';
                    }
                    else if(U_D > 0) // down
                    {
                        pdirection = ndirection;
                        ndirection = 'd';
                    }
                    sstate = input;
                }
                else if ((L_R > 250) || (L_R <= -249))
                {
                    if (L_R < 0) // left
                    {
                        pdirection = ndirection;
                        ndirection = 'l';
                    }
                    else if(L_R > 0) // right
                    {
                        pdirection = ndirection;
                        ndirection = 'r';
                    }
                    sstate = input;
                }
                break;
                
            case no_input:
                // if the snake is going left, let it continue to go left
                if (ndirection == 'l')
                {
                    temp_snakehead[0] = snake_arr[0] - 1;
                    temp_snakehead[1] = snake_arr[1];
                }
                // if the snake is going right, let it continue to go right
                else if (ndirection == 'r')
                {
                    temp_snakehead[0] = snake_arr[0] + 1;
                    temp_snakehead[1] = snake_arr[1];
                }
                // if the snake is going down, let it continue to go down
                else if (ndirection == 'd')
                {
                    temp_snakehead[0] = snake_arr[0];
                    temp_snakehead[1] = snake_arr[1] + 1;
                }
                // if the snake is going up, let it continue to go up
                else if (ndirection == 'u')
                {
                    temp_snakehead[0] = snake_arr[0];
                    temp_snakehead[1] = snake_arr[1] - 1;
                }
                sstate = iteration;
                break;
                
            case input:
                break;
                
            case iteration:
                nokia_lcd_set_pixel(snake_arr[10],snake_arr[11],0);
                nokia_lcd_render();
                
                //y coords
                snake_arr[11] = snake_arr[9];
                snake_arr[9] = snake_arr[7];
                snake_arr[7] = snake_arr[5];
                snake_arr[5] = snake_arr[3];
                snake_arr[3] = snake_arr[1];
                
                //x coords
                snake_arr[10] = snake_arr[8];
                snake_arr[8] = snake_arr[6];
                snake_arr[6] = snake_arr[4];
                snake_arr[4] = snake_arr[2];
                snake_arr[2] = snake_arr[0];
                
                snake_arr[1] = temp_snakehead[1];
                snake_arr[0] = temp_snakehead[0];
                
                //pixelate new snake
                nokia_lcd_set_pixel(snake_arr[10], snake_arr[11], 1);
                nokia_lcd_set_pixel(snake_arr[8], snake_arr[9], 1);
                nokia_lcd_set_pixel(snake_arr[6], snake_arr[7], 1);
                nokia_lcd_set_pixel(snake_arr[4], snake_arr[5], 1);
                nokia_lcd_set_pixel(snake_arr[2], snake_arr[3], 1);
                nokia_lcd_set_pixel(snake_arr[0], snake_arr[1], 1);
                nokia_lcd_render();
                sstate = check;
                break;
                
            case check:
                nokia_lcd_write_char('k',1);
                break;
                
            case dead:
                sstate = dead;
                break;
                
            default:
                break;
        }
        switch(sstate) //actions
        {
            case generate:
                sstate = breathe;
                break;
                
            case breathe:
                break;
                
            case no_input:
                break;
                
            case input:
                //old input = left
                if(pdirection == 'l')
                {
                    //new direction = left
                    if(pdirection == ndirection)
                    {
                        sstate = no_input;
                    }
                    //new direction = right
                    else if(ndirection == 'r')
                    {
                        sstate = no_input;
                    }
                    //new direction = down
                    else if(ndirection == 'd')
                    {
                        temp_snakehead[1] = snake_arr[1] + 1;
                        sstate = iteration;
                    }
                    //new direction = up
                    else if(ndirection == 'u')
                    {
                        temp_snakehead[1] = snake_arr[1] - 1;
                        sstate = iteration;
                    }
                }
                
                //old input = right
                else if(pdirection == 'r')
                {
                    //new direction = right
                    if(pdirection == ndirection)
                    {
                        sstate = no_input;
                    }
                    //new direction = left
                    else if(ndirection == 'l')
                    {
                        sstate = no_input;
                    }
                    //new direction = down
                    else if(ndirection == 'd')
                    {
                        temp_snakehead[1] = snake_arr[1] + 1;
                        sstate = iteration;
                    }
                    //new direction = up
                    else if(ndirection == 'u')
                    {
                        temp_snakehead[1] = snake_arr[1] - 1;
                        sstate = iteration;
                    }
                }
                
                //old input = down
                else if(pdirection == 'd')
                {
                    //new direction = down
                    if(pdirection == ndirection)
                    {
                        sstate = no_input;
                    }
                    //new direction = up
                    else if(ndirection == 'u')
                    {
                        sstate = no_input;
                    }
                    //new direction = right
                    else if(ndirection == 'r')
                    {
                        temp_snakehead[0] = snake_arr[0] + 1;
                        sstate = iteration;
                    }
                    //new direction = left
                    else if(ndirection == 'l')
                    {
                        temp_snakehead[0] = snake_arr[0] - 1;
                        sstate = iteration;
                    }
                }
                
                //old input = up
                else if(pdirection == 'u')
                {
                    //new direction = up
                    if(pdirection == ndirection)
                    {
                        sstate = no_input;
                    }
                    //new direction = down
                    else if(ndirection == 'd')
                    {
                        sstate = no_input;
                    }
                    //new direction = right
                    else if(ndirection == 'r')
                    {
                        temp_snakehead[0] = snake_arr[0] + 1;
                        sstate = iteration;
                    }
                    //new direction = left
                    else if(ndirection == 'l')
                    {
                        temp_snakehead[0] = snake_arr[0] - 1;
                        sstate = iteration;
                    }
                }
                break;
                
            case iteration:
                sstate = iteration;
                break;
                
            case check:
                if(fruit_arr[0] == temp_snakehead[0] && fruit_arr[1] == temp_snakehead[1])
                {
                    score = score + 10;
                    nokia_lcd_set_pixel(fruit_arr[0],fruit_arr[1],0);
                    nokia_lcd_render();
                    //x-value of fruit location
                    fruit_arr[0] = rand()%83;
                    
                    //y-value of fruit location
                    fruit_arr[1] = rand()%47;
                    nokia_lcd_set_pixel(fruit_arr[0],fruit_arr[1],1);
                    nokia_lcd_render();
                    
                    sstate = breathe;
                }
                else if (temp_snakehead[0] == 0 || temp_snakehead[0] == 83 || temp_snakehead[1] == 0 || temp_snakehead == 47)
                {
                    sstate = dead;
                }
                else
                {
                    for (i = 2; i < 11; i = i + 2)
                    {
                        if (temp_snakehead[0] == snake_arr[i] && temp_snakehead[1] == snake_arr[i+1])
                        {
                            var = 1;
                        }
                    }
                    if (var == 1)
                        sstate = dead;
                    else
                        sstate = breathe;
                }
                break;
                
            case dead:
                nokia_lcd_clear();
                nokia_lcd_set_cursor(0,10);
                nokia_lcd_write_string("you died fool", 1);
                temp_score = eeprom_read_byte((uint8_t*)23);
                if(score > temp_score)
                {
                    score = temp_score;
                    eeprom_write_byte((uint8_t*)23, score);
                    highscore = eeprom_read_byte((uint8_t*)23);
                }
                nokia_lcd_render();
                key = 0;
                
                sstate = 999;
                break;
                
            default:
                break;
        }
    }
}

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF;
    
    // Period for the tasks
    unsigned long int LCD_Menu_calc = 100;
    unsigned long int Snake_calc = 10;
    
    unsigned long int tmpGCD = 1;
    tmpGCD = findGCD(LCD_Menu_calc, Snake_calc);
    
    unsigned long int GCD = tmpGCD;
    
    //Recalculate GCD periods for scheduler
    unsigned long int SMTick1_period = LCD_Menu_calc/GCD;
    unsigned long int SMTick2_period = Snake_calc/GCD;
    
    static task task1, task2;
    task *tasks[] = { &task1, &task2};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    // Task 1
    task1.state = -1;//Task initial state.
    task1.period = SMTick1_period;//Task Period.
    task1.elapsedTime = SMTick1_period;//Task current elapsed time.
    task1.TickFct = &TickFct_LCR_Menu;//Function pointer for the tick.
    
    // Task 2
    task2.state = -1;//Task initial state.
    task2.period = SMTick2_period;//Task Period.
    task2.elapsedTime = SMTick2_period;//Task current elapsed time.
    task2.TickFct = &TickFct_Snake;//Function pointer for the tick.
    
    
    ADC_init();
    nokia_lcd_init();
    
    // Set the timer and turn it on
    TimerSet(GCD);
    TimerOn();
    
    
    temp_score = 0;
    eeprom_write_byte((uint8_t*)23, temp_score);
    
    state = Welcome;
    sstate = generate;
    unsigned short i; // Scheduler for-loop iterator
    while(1) {
        // Scheduler code
        for ( i = 0; i < numTasks; i++ ) {
            // Task is ready to tick
            if ( tasks[i]->elapsedTime == tasks[i]->period ) {
                // Setting next state for task
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                // Reset the elapsed time for next tick.
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 1;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 0;
}
