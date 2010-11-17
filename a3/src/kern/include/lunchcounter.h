
/*** Constants that define parameters of the simulation ***/

#define NSLEEP 3        /* How long main thread sleeps btwn cust creation */
#define NSEATS 6        /* number of seats available */
#define MENU_ITEMS 10   /* Number of different menu items */


/* time to prepare & eat each menu item */
static int item_time[MENU_ITEMS] = { 3, 4, 6, 2, 5, 3, 6, 2, 4, 3};

extern int lunchcounter(int nargs, char **args);

