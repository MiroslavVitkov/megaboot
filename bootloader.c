#include <avr/io.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "usart.h"
#include "config.h"

#define NDEBUG  // TODO: move to the config file

// Custom runtime assert statement.
#undef assert
#ifdef NDEBUG
#define assert(expr)
#else
void handle_error(int err_num, char *fname, int line_num, const char *foo) {};
#define assert(expr) (void)((expr) || (handle_error(0, __FILE__, __LINE__, __func__), 0))
#endif

#define APPLICATION_SECTION_END_PAGES 24  // TODO: move to config file and calculate automatically.

// Write one Flash page.
// 'buff' needs to be 'SPM_PAGESIZE' bytes long.
// We CAN`T jump to the application later because we did not call 'boot_rww_enable()'.
void program_flash_page(unsigned page_number, char buffer[])
{
    assert(page_number < APPLICATION_SECTION_END_PAGES);  // Guard against overwriting the bootloader section.

    boot_page_erase(page_number);
    boot_spm_busy_wait();                                 // Wait until the memory is erased.

    char *buff = buffer;
    for(int i = 0; i < SPM_PAGESIZE; i += 2)
    {
        // Set up little-endian word.
        uint16_t w = *buff++;
        w += (*buff++) << 8;

        boot_page_fill (page_number + i, w);
    }

  boot_page_write(page_number);
  boot_spm_busy_wait();                                    // Wait until the memory is written.
}


// Receive one XMODEM packet.
// Check for errors as provided by the protocol.
// Parameter: 'char page_buffer[SPM_PAGESIZE]'.
// Return: 1 - EOF was received, 0 - normal packet, -1 - error
// See: https://en.wikipedia.org/wiki/XMODEM
// See: http://www.atmel.com/Images/doc1472.pdf
int receive_usart_packet(char page_buffer[])
{
    char first_byte = usart_receive();
    switch(first_byte)
    {
        case ASCII_EOT:
            return 1;
        case ASCII_SOH:
            break;        // Execute body of the function.
        default:
            return -1;
    }

    uint8_t packet_number = usart_receive();
    uint8_t packet_number_inverted = usart_receive();
    static unsigned packet_counter = 1;                   // First packet is number 1, not 0.
    if((packet_number != ~packet_number_inverted) ||
       (packet_counter != packet_number))  // TODO: send ACK and ignore packet
    {
        return -1;
    }

    for(unsigned i = 0; i < SPM_PAGESIZE; ++i)
    {
        page_buffer[i] = usart_receive();
    }

    const uint8_t expected_checksum = usart_receive();  // 1-byte if initial request was NACK, 2-byte if initial request was 'C'.
    uint8_t checksum = first_byte + packet_number + packet_number_inverted;
    for(int i = 0; i < SPM_PAGESIZE; ++i)
    {
        checksum += page_buffer[i];                     // Unsigned overflow is deterministic and safe.
    }
    if(expected_checksum != checksum)
    {
        return -1;
    }

    ++packet_counter;
    return 0;  // Packet received successfully.
}


// We are supposed to arrive here after a jump from the application section.
// Next we anticipate XMODEM transmission of new application data.
// In the end we state success of failure via the serial conenction and wait for a cold reset.
#include <util/delay.h>
void main(void)
{
    char page_buffer[SPM_PAGESIZE];
    unsigned page_number = 0;

    cli();
    usart_init();
//    usart_transmit(ASCII_NACK);                   // Request file transfer.

usart_transmit(ASCII_NACK);
while(1) {_delay_ms(300); usart_transmit(ASCII_ACK);};
    while(1)
    {
        int packet_type = receive_usart_packet(page_buffer);
        switch(packet_type)
        {
            case 1:                               // This is the last packet. It does not contain data.
                usart_transmit(ASCII_ACK);
                goto FINISHED;
            case 0:                               // Packet accepted correctly, proceed to next one.
                program_flash_page(page_number++, page_buffer);
                break;
            case -1:                              // Error in transmission, request resend of packet.
                usart_transmit(ASCII_NACK);
                break;
        }
    }

FINISHED:
    while(1);
}
