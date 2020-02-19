//
//  testomp.h
//  dummylib
//
//  Created by Anuj Jain on 12/24/19.
//  Copyright © 2019 Anuj Jain. All rights reserved.
//

#ifndef testomp_h
#define testomp_h

#include <stdio.h>
void testparallelfn(int xid);
#define DEBSEQ 0x077CB531
#define MAX_DEB_SEQ_SIZE 0xFFFFFFFF
#define DEB_SEQ_REM_WINDOW 27

static const int DEBRUIJNBITPOS[32] =
{
    0, 1, 28, 2, 29, 14, 24, 3,
    30, 22, 20, 15, 25, 17, 4, 8,
    31, 27, 13, 23, 21, 19, 16, 7,
    26, 12, 18, 6, 11, 5, 10, 9
};

#endif /* testomp_h */
