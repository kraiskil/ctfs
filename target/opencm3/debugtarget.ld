/* An imaginary target with insane memories.
 * This is useful, since GNU tools seem to be
 * able to print much more statistics info 
 * after linking step. 
 * With this target there is space for it all
 * and hopfully the linker statistics prints
 * give some clue on where to cut back.
 */
MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 100M
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 1M
}

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld

