/*
Authors: Matthijs Douze & Herve Jegou 
Contact: matthijs.douze@inria.fr  herve.jegou@inria.fr

*/


#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include "nn.h"
#include "vector.h"
#include "sorting.h"

#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#define INLINE _inline
#else
#define BLASCALL(f) f ##_
#define INLINE inline
#endif

/********************************************************************************************************************************************/


size_t vlad_sizeof(int k, int d, int flags) 
{
	size_t s=d*k;
	if(flags==6) s*=3;
	if(flags==15) s*=2;
	if(flags==17) s*=2;
	return s;
}
/********************************************************************************************************************************************/


static double sqr (double x) /* inline */
{
	return x * x;
}

/********************************************************************************************************************************************/


void vlad_compute(int k, int d, const float *centroids, int n, const float *v,int flags, float *desc) 
{

	int i,j,l,n_quantile,i0,i1,ai,a,ma,ni;
	int *perm ;
	float un , diff;
	float *tab,*u,*avg,*sum,*mom2,*dists;
	int *hist,*assign;


	if(flags<11 || flags>=13) 
	{
		assign=ivec_new(n);

		nn(n,k,d,centroids,v,assign,NULL,NULL);    

		if(flags==6 || flags==7) 
		{
			n_quantile = flags==6 ? 3 : 1;
			fvec_0(desc,k*d*n_quantile);
			perm      = ivec_new(n);
			tab       = fvec_new(n);
			ivec_sort_index(assign,n,perm);
			i0=0;
			for(i=0;i<k;i++) 
			{
				i1=i0;
				while(i1<n && assign[perm[i1]]==i) 
				{
					i1++;
				}

				if(i1==i0) continue;

				for(j=0;j<d;j++) 
				{        
					for(l=i0;l<i1;l++)
					{
						tab[l-i0]=v[perm[l]*d+j];
					}
					ni=i1-i0;
					fvec_sort(tab,ni);
					for(l=0;l<n_quantile;l++) 
					{
						desc[(i*d+j)*n_quantile+l]=(tab[(l*ni+ni/2)/n_quantile]-centroids[i*d+j])*ni;
					}
				}

				i0=i1;
			}
			free(perm);
			free(tab);
		} 
		else if(flags==5) 
		{
			fvec_0(desc,k*d);

			for(i=0;i<n;i++) 
			{
				for(j=0;j<d;j++) 
				{
					desc[assign[i]*d+j]+=v[i*d+j];
				}
			}

		} 
		else if(flags==8 || flags==9) 
		{
			fvec_0(desc,k*d);

			u   = fvec_new(d);

			for(i=0;i<n;i++) 
			{
				fvec_cpy(u,v+i*d,d);
				fvec_sub(u,centroids+assign[i]*d,d);
				un=(float)sqrt(fvec_norm2sqr(u,d));

				if(un==0) continue;
				if(flags==8) 
				{        
					fvec_div_by(u,d,un);
				} else if(flags==9) 
				{
					fvec_div_by(u,d,sqrt(un));
				}

				fvec_add(desc+assign[i]*d,u,d);

			}
			free(u);
		} 
		else if(flags==10) 
		{
			fvec_0(desc,k*d);

			for(i=0;i<n;i++) 
			{
				for(j=0;j<d;j++) 
				{
					desc[assign[i]*d+j]+=v[i*d+j];
				}
			}

			for(i=0;i<k;i++) 
			{
				fvec_normalize(desc+i*d,d,2.0);  
			}

		} 
		else if(flags==13) 
		{

			fvec_0(desc,k*d);

			for(i=0;i<n;i++) 
			{
				for(j=0;j<d;j++) 
				{
					desc[assign[i]*d+j]+=(float)sqr(v[i*d+j]-centroids[assign[i]*d+j]);
				}
			}     

		} 
		else if(flags==14) 
		{
			avg = fvec_new_0(k*d);

			for(i=0;i<n;i++) 
			{
				for(j=0;j<d;j++) 
				{
					avg[assign[i]*d+j]+=v[i*d+j]-centroids[assign[i]*d+j];
				}
			}

			hist=ivec_new_histogram(k,assign,n);

			for(i=0;i<k;i++) 
			{
				if(hist[i]>0) 
				{
					for(j=0;j<d;j++) 
					{
						avg[i*d+j]/=hist[i];
					}
				}
			}

			free(hist);

			fvec_0(desc,k*d);
			for(i=0;i<n;i++) 
			{
				for(j=0;j<d;j++) 
				{
					desc[assign[i]*d+j]+=(float)(sqr(v[i*d+j]-centroids[assign[i]*d+j]-avg[assign[i]*d+j]));
				}
			}

			fvec_sqrt(desc,k*d);

			free(avg);
		}  
		else if(flags==15) 
		{
			fvec_0(desc,k*d*2);
			sum = desc;

			for(i=0;i<n;i++) 
			{
				for(j=0;j<d;j++) 
				{
					sum[assign[i]*d+j]+=v[i*d+j]-centroids[assign[i]*d+j];
				}
			}

			hist = ivec_new_histogram(k,assign,n);

			mom2 = desc+k*d;

			for(i=0;i<n;i++) 
			{
				ai=assign[i];
				for(j=0;j<d;j++) 
				{
					mom2[ai*d+j]+=(float)(sqr(v[i*d+j]-centroids[ai*d+j]-sum[ai*d+j]/hist[ai]));
				}
			}
			fvec_sqrt(mom2,k*d);
			free(hist);


		} 
		else if(flags==17) 
		{
			fvec_0(desc,k*d*2);

			for(i=0;i<n;i++) 
			{
				for(j=0;j<d;j++) 
				{
					diff=v[i*d+j]-centroids[assign[i]*d+j];
					if(diff>0)
					{
						desc[assign[i]*d+j]+=diff;
					}
					else 
					{
						desc[assign[i]*d+j+k*d]-=diff;
					}
				}
			}

		} 
		else 
		{
			fvec_0(desc,k*d);

			for(i=0;i<n;i++) 
			{
				for(j=0;j<d;j++) 
				{
					desc[assign[i]*d+j]+=v[i*d+j]-centroids[assign[i]*d+j];
				}
			}


			if(flags==1) 
			{
				hist=ivec_new_histogram(k,assign,n);
				/* printf("unbalance factor=%g\n",ivec_unbalanced_factor(hist,k)); */

				for(i=0;i<k;i++) 
				{
					for(j=0;j<d;j++) 
					{
						desc[i*d+j]/=hist[i];    
					}
				}
				free(hist);
			}

			if(flags==2) 
			{
				for(i=0;i<k;i++) 
				{
					fvec_normalize(desc+i*d,d,2.0);
				}
			}

			if(flags==3 || flags==4) 
			{
				assert(!"not implemented");
			}

			if(flags==16) 
			{
				hist=ivec_new_histogram(k,assign,n);
				for(i=0;i<k;i++) 
				{
					if(hist[i]>0) 
					{
						fvec_norm(desc+i*d,d,2);
						fvec_mul_by(desc+i*d,d,sqrt(hist[i]));
					}
				}
				free(hist);
			}


		}
		free(assign);
	} 
	else if(flags==11 || flags==12) 
	{
		ma=flags==11 ? 4 : 2;
		assign=ivec_new(n*ma);

		dists=knn(n,k,d,ma,centroids,v,assign,NULL,NULL);    

		fvec_0(desc,k*d);

		for(i=0;i<n;i++) 
		{
			for(j=0;j<d;j++) 
			{
				for(a=0;a<ma;a++) 
				{
					desc[assign[ma*i+a]*d+j]+=v[i*d+j]-centroids[assign[ma*i+a]*d+j];
				}
			}
		} 

		free(dists);

		free(assign);
	}

}
