#ifndef _BIT_UTIL_H_
#define _BIT_UTIL_H_

// Read-Modify-Write operations. Prefer special set/clr/toggle regs where
// possible to reduce cycle count
#define _SET_BIT(var, pos) (var |= 1 << pos)
#define _CLR_BIT(var, pos) (var &= ~(1 << pos))
#define _INV_BIT(var, pos) (var ^= 1 << pos)

// Memory space sub-register writes
#define _SET_BYTE(reg, byte, val) (*((volatile uint8_t*) &reg + byte)) = val)
#define _SET_WORD(reg, word, val) (*((volatile uint16_t*) &reg + word)) = val)

/**
 * Clear a multi-bit field in a register
 */
#define _FLDCLR(reg, field) (reg &= ~(field ## _Msk))
/**
 * Set the value of a register field if it has been zeroed before
 */
#define _FLDSET(reg, field, val) (reg |= ((val & field ## _Msk) << field ## _Pos))

#endif /* BIT_UTIL */
