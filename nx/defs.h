#ifndef VALS_H
#define VALS_H

/* How Frame Interval is calculated /*
---------------------------------------------------------------------------------------

 * 59.65549883230804 FPS = 16.76291405778042 MS = 0.016762914057780419624 Seconds
 * Target period is 0.016762914057780419624
 * Rounding down to 0.016762 should be accurate enough,
 * and it fits nicely with a prescalar of 8 (16 MHz or 16000000 Hz)
 * New clock speed = 0.016762 * (16000000 / 8 )
 * 0.016762 * 2000000 = 33524
 * Now we just need to check every time the TCNT1 register matches the calculated value.
 * To increment the target seed timer by smaller intervals, we can simply divide the
 * result interval by even divisors (33524 / 2, 4) where 2 = 16762 and 4 = 8381
*/

// 60 fps = 16.666666 MS ~= 0.0166666666666667 sec
// 0.016666 * (16000000 / 8 ) Pre-scalar still == 8
// 0.016666 * 2000000 = 33,332
// Default = 33332
// Half = 16666
// Quarter = 8333

// GBA 16.7427 MS
// 0.016742 * (16000000 / 8) == 33484
// Default = 33484
// Half = 16742
// Quarter = 8371

#ifdef DEFAULT
#       ifdef GBA
#           define F_INTERVAL 33484
#       endif
#       ifdef NDS
#           define F_INTERVAL 33524
#       endif
#       ifdef NX1
#           define F_INTERVAL 33332
#       endif
#   define DI(x) ((x))
#endif

#ifdef HALF
#       ifdef GBA
#           define F_INTERVAL 16742
#       endif
#       ifdef NDS
#           define F_INTERVAL 16762
#       endif
#       ifdef NX1
#           define F_INTERVAL 16666
#       endif
#   define DI(x) ((x)*2)
#endif

#ifdef QUARTER
#       ifdef GBA
#           define F_INTERVAL 8371
#       endif
#       ifdef NDS
#           define F_INTERVAL 8381
#       endif
#       ifdef NX1
#           define F_INTERVAL 8333
#       endif
#   define DI(x) ((x)*4)
#endif

#endif
