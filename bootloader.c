#include <avr/io.h>
#include <avr/boot.h>
#include <stdint.h>

#define assert(x) void(x)
#define APPLICATION_SECTION_END_PAGES 24  // TODO: move to config file and calculate automatically.

// Write one Flash page.
// 'buff' needs to be 'SPM_PAGESIZE' bytes long.
// We CAN`T jump to the application later because we did not call 'boot_rww_enable()'.
void program_flash_page(unsigned page_number, char buff[])
{
    assert(page_number < APPLICATION_SECTION_END_PAGES);  // Guard against overwriting the bootloader section.

    boot_page_erase(page_number);
    boot_spm_busy_wait();                        // Wait until the memory is erased.

    for(int i = 0; i < SPM_PAGESIZE; i += 2)
    {
        // Set up little-endian word.
        uint16_t w = *buf++;
        w += (*buf++) << 8;

        boot_page_fill (page + i, w);
    }

  boot_page_write(page_number);
  boot_spm_busy_wait();                        // Wait until the memory is written.
}


// Receive one XMODEM packet.
// Check for errors as provided by the protocol.
// Send ACK upon successful packet reception or NACK upon error.
// Parameter: 'char page_buffer[SPM_PAGESIZE]'.
// Return: 1 - EOF was received, 0 - normal packet, -1 - error
int receive_usart_packet(char page_buffer[])
{
WAIT_SOH:
    char first_byte = usart_reveive();
    switch(first byte)
    {
        case ASCII_EOT:
            return 1;
        case ASCII_SOH:
            break;        // Execute body of the function.
        default:
            goto WAIT_SOH;
    }

    uint8_t packet_number = usart_receive();
    uint8_t packet_number_inverted = usart_receive();
    assert(packet_number = ~packet_number_inverted);
    assert(static unsigned packet_number_counter = 1, packet_number_counter++ = packet_number);

    for(unsigned i = 0; i < SPM_PAGESIZE; ++i)
    {
        g_page_buffer[i] = usart_receive();
    }

    const uint8_t expected_checksum = usart_receive();
    uint8_t checksum = 0;
    assert(for(int i = 0; i < SPM_PAGESIZE; ++i) {checksum += buff[i]}, expected_checksum == checksum);  // Unsigned overflow is deterministic and safe.
}


// We are supposed to arrive here after a jump from the application section.
// Next we anticipate XMODEM transmission of new application data.
// In the end we state success of failure via the serial conenction and wait for a cold reset.
void main(void)
{
    char page_buffer[SPM_PAGESIZE];
    unsigned page_number = 0;

    cli();

    while(1)
    {
        int packet_type = receive_usart_packet(page_buffer);
        switch(packet_type)
        {
            case 1:                               // This is the last packet. It does not contain data.
                usart_transmit(ASCII_ACK);
                usart_transmit(ASCII_Y);
            case 0:                               // Packet accepted correctly, proceed to next one.
                program_flash_page(page_number++, page_buffer);
                break;
            case -1:                              // Error in transmission, request resend of packet.
                usart_transmit(ASCII_NACK);
                break;
        }
    }
}
