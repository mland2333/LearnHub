extern void uart_init(void);
extern void uart_puts(char *s);
extern void uart_input();
void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, RVOS!\n");

    uart_input();
	while (1) {}; // stop here!
}

