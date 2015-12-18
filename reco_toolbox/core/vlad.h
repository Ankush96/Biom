/*
Authors: Matthijs Douze & Herve Jegou 
Contact: matthijs.douze@inria.fr  herve.jegou@inria.fr
*/



#ifndef VLAD_H_INCLUDED
#define VLAD_H_INCLUDED

#include <stdlib.h>


/* flags==0: simple sum of descriptors 
 * flags==15: sum + second moments
 */

size_t vlad_sizeof (int k, int d, int flags);
      


/* compute descriptor. Writes the descriptor corresponding to v (size n*d) in desc (size vlad_sizeof(...)) */
void vlad_compute (int k, int d, const float *centroids, 
                   int n, const float *v,
                   int flags, float *desc);




#endif
