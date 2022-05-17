#ifndef _BASIC_UTILITY__H
#define _BASIC_UTILITY__H

#define KILL_COM_OBJECT(x) if (x) { x->Release(); x = 0; }
#define DESTROY(x) if (x) { delete x; x = 0;}
#define KILL_ARRAY(x) if (x) { delete [] x; x = 0; }

float Lerp(float a, float b, float t);


#endif _BASIC_UTILITY__H