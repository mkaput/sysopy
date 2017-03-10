#ifndef SYSOPY_KSIAZKA_ADRESOWA_UTILS_H
#define SYSOPY_KSIAZKA_ADRESOWA_UTILS_H

#include <stdbool.h>

/**
 * Value comparator.
 *
 * Should return:
 * <ul>
 *  <li>a < b => negative number</li>
 *  <li>a > b => positive number</li>
 *  <li>a = b => 0</li>
 *  </ul>
 */
typedef int (*ValueComparator)(void *a, void *b);

#endif //SYSOPY_KSIAZKA_ADRESOWA_UTILS_H
