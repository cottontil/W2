//
// Created by Aaron Ye on 7/28/22.
//

//extern "C"
//{extern void printhello();}
#define UART_READ_REG (*(volatile unsigned int*)(0x1f800000))
#define UART_TX_BUSY_REG (*(volatile unsigned int*)(0x1f800001))
#define UART_WRITE_REG (*(volatile unsigned int*)(0x1f800002))

void uart_puts(const char* str);

void setup() {
    // put your setup code here, to run once:

}


void loop()
{
    // put your main code here, to run repeatedly:
    uart_puts("hello\n");
    while(1);
}
void uart_puts(const char* str)
{
    while(1)
    {
        if('\0' == *str)
        {
            return;
        }
        while(1 == UART_TX_BUSY_REG);
        UART_WRITE_REG = *str;
        *str++;
    }
}