#include <types.h>
#include <lib.h>

/*
 * Do a backspace in typed input.
 * We overwrite the current character with a space in case we're on
 * a terminal where backspace is nondestructive.
 */
static
void
backsp(void)
{
	putch('\b');
	putch(' ');
	putch('\b');
}

/*
 * Read a string off the console. Support a few of the more useful
 * common control characters. Do not include the terminating newline
 * in the buffer passed back.
 */
void
kgets(char *buf, size_t maxlen)
{
	size_t pos = 0;
	int ch;

	while (1) {
		ch = getch();
		if (ch=='\n' || ch=='\r') {
			putch('\n');
			break;
		}

		/* Only allow the normal 7-bit ascii */
		if (ch>=32 && ch<127 && pos < maxlen-1) {
			putch(ch);
			buf[pos++] = ch;
		}
		else if ((ch=='\b' || ch==127) && pos>0) {
			/* backspace */
			backsp();
			pos--;
		}
		else if (ch==3) {
			/* ^C - return empty string */
			putch('^');
			putch('C');
			putch('\n');
			pos = 0;
			break;
		}
		else if (ch==18) {
			/* ^R - reprint input */
			buf[pos] = 0;
			kprintf("^R\n%s", buf);
		}
		else if (ch==21) {
			/* ^U - erase line */
			while (pos > 0) {
				backsp();
				pos--;
			}
		}
		else if (ch==23) {
			/* ^W - erase word */
			while (pos > 0 && buf[pos-1]==' ') {
				backsp();
				pos--;
			}
			while (pos > 0 && buf[pos-1]!=' ') {
				backsp();
				pos--;
			}
		}
		else {
			beep();
		}
	}

	buf[pos] = 0;
}
