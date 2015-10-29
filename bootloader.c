#include <avr/io.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "usart.h"
#include "config.h"

#define XMODEM_PAYLOAD_BYTES 128

// Custom runtime assert statement.
#undef assert
#ifdef NDEBUG
#define assert(expr)
#else
void handle_error(int err_num, char *fname, int line_num, const char *foo) {};
#define assert(expr) (void)((expr) || (handle_error(0, __FILE__, __LINE__, __func__), 0))
#endif

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
// Parameter: 'char payload_buffer[XMODEM_PAYLOAD_BYTES]' is an output.
// Return: 1 - EOF was received, 0 - normal packet, <0 - error
// See: https://en.wikipedia.org/wiki/XMODEM
// See: http://www.atmel.com/Images/doc1472.pdf
error_t receive_xmodem_packet(char payload_buffer[])
{
    char first_byte = usart_receive();
    switch(first_byte)
    {
        case ASCII_EOT:
            return 1;
        case ASCII_SOH:
            break;        // Execute body of the function.
        default:
            return ERROR_PROTOCOL_FIRST_CHARACTER;
    }

    uint8_t packet_number = usart_receive();
    uint8_t packet_number_inverted = usart_receive();
    static unsigned packet_counter = 1;                      // First packet is number 1, not 0.
    if((uint8_t)(~packet_number_inverted) != packet_number)  // Cast because of integer promotion.
    {
        return ERROR_PROTOCOL_PACKET_NUMBER_INVERSION;
    }
    if(packet_counter != packet_number)           // TODO: send ACK and ignore duplicate packet
    {
        return ERROR_PROTOCOL_PACKET_NUMBER_ORDER;
    }

    for(unsigned i = 0; i < XMODEM_PAYLOAD_BYTES; ++i)
    {
        payload_buffer[i] = usart_receive();
    }

    const uint8_t expected_checksum = usart_receive();  // 1-byte if initial request was NACK, 2-byte if initial request was 'C'.
    uint8_t checksum = 0;
    for(int i = 0; i < XMODEM_PAYLOAD_BYTES; ++i)
    {
        checksum += payload_buffer[i];                     // Unsigned overflow is deterministic and safe.
    }

    if(expected_checksum != checksum)
    {
        return ERROR_PROTOCOL_CRC;
    }

    ++packet_counter;
    return 0;                                           // Packet received successfully.
}


// We are supposed to arrive here after a jump from the application section.
// Next we anticipate XMODEM transmission of new application data.
// In the end we state success of failure via the serial conenction and wait for a cold reset.
void main(void)
{
    char xmodem_payload[XMODEM_PAYLOAD_BYTES];
    //char page_buffer[SPM_PAGESIZE];               // 64 bytes for an atmega8
    //unsigned page_number = 0;

    cli();
    usart_init();
    usart_transmit(ASCII_NACK);                   // Request file transfer.

    while(1)
    {
        int packet_type = receive_xmodem_packet(xmodem_payload);
        switch(packet_type)
        {
            case 1:                               // This is the last packet. It does not contain data.
                usart_transmit(ASCII_ACK);
                goto FINISHED;
            case 0:                               // Packet accepted correctly, proceed to next one.
                usart_transmit(ASCII_ACK);
                //program_flash_page(page_number++, page_buffer);
                break;
            case -1:                              // Error in transmission, request resend of packet.
                usart_transmit(ASCII_NACK);
                break;
        }
    }

FINISHED:
    while(1);
}
