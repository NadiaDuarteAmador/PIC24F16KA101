
/* 
 * File:   
 * Author: Shahdad
 * Comments:
 * Revision history: 
 */
 
#ifndef IOS_H
#define	IOS_H

#include <xc.h>  


void IOinit (void);
void IOcheck(void);
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void);


#endif	