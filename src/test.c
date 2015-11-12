#include "config.h"
#include <util/delay.h>

#include "usart.h"

void send_block(unsigned bytes, char *buff)
{
    for(int i = 0; i < bytes; ++i)
    {
        usart_transmit(buff[i]);
    }
}

int main(int argc, char **argv)
{
    usart_init();

    char msg[] = "Loading bootloader in 3 seconds.";
    send_block(sizeof(msg), msg);
    _delay_ms(3000);

    typedef void (* fn_ptr_t) (void);
    fn_ptr_t my_ptr = (fn_ptr_t)0x1800;
    my_ptr();
}
