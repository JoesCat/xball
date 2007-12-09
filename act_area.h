#ifndef __ACT_AREA_H__
#define __ACT_AREA_H__

typedef struct {
    char *label;                /* Button widget's name */
    void (*callback)();         /* Button widget's callback */
    caddr_t data;               /* Callback data */
} ActionAreaItem;

Widget CreateActionArea();

#endif /* __ACT_AREA_H__ */
