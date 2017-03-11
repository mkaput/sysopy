#ifndef ADDRBOOK_UTILS_H
#define ADDRBOOK_UTILS_H

#include <stdbool.h>

/**
 * Value comparator.
 *
 * Should return:
 * <ul>
 *  <li>a &lt; b => negative number</li>
 *  <li>a &gt; b => positive number</li>
 *  <li>a = b => 0</li>
 *  </ul>
 */
typedef int (*ValueComparator)(void *a, void *b);

#endif //ADDRBOOK_UTILS_H
