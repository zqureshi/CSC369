#ifndef _MIPS_BUS_H_
#define _MIPS_BUS_H_

/*
 * Generic bus interface file.
 *
 * The only bus we support for MIPS is LAMEbus.
 * This would need to be a bit more complicated if that weren't the case.
 */

#include <machine/vm.h>		/* for MIPS_KSEG1 */
#include <lamebus/lamebus.h>	/* for LAMEbus definitions */

#define bus_write_register(bus, slot, offset, val) \
    lamebus_write_register(bus, slot, offset, val)

#define bus_read_register(bus, slot, offset) \
    lamebus_read_register(bus, slot, offset)

#define bus_map_area(bus, slot, offset) \
    lamebus_map_area(bus, slot, offset)

/*
 * Machine-dependent LAMEbus definitions
 */

/* Base address of the LAMEbus mapping area */
#define LB_BASEADDR  (MIPS_KSEG1 + 0x1fe00000)

void mips_lamebus_interrupt(void);

#endif /* _MIPS_BUS_H_ */
