#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_ 
#ifdef __cplusplus
extern "C"
{
#endif

    void init(int n, int m, int **temp);

    int query(int x1, int y1, int x2, int y2);

    void change(int x, int y, int temp);

#ifdef __cplusplus
}
#endif

#endif //_TEMPERATURE_H_
