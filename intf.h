#ifndef __INTF_H__
#define __INTF_H__

#define INTFAC  16      /* Multiplied to everything but velocities */

typedef long intf;

#define int2intf( val)   ((intf)((val)*INTFAC))
#define intf2int( val)   ((long)((val)/INTFAC))

#endif
