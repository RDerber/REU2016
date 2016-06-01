#include <stdlib.h>
#include <stdio.h>

int* polyMult(int points[], int prod[], int numPoints){
	int i;
	prod[0] = 1;
	prod[1] = points[0] + points[1];
	prod[2] = points[0]*points[1];
	printf("%d %c",numPoints, '\n');
	for(i = 2; i< numPoints; ++i){
		int j;
		int prodTemp[5];
		int k;
		for(k=0;k<numPoints+1;++k){
			prodTemp[k]=prod[k];
		}
		for(j = 1;j <= i;++j){
			int last = prod[i];
			if(j==1){
				prod[j] = prod[j] + points[i];
                        }else {
				if(j==i){
                                	prod[i+1] = last;
				}
				prod[j] = prodTemp[j-1]*points[i] + prodTemp[j];
			}
		}
		prod[i+1] *= points[i];
		for(k=0;k<numPoints+1;++k){
			printf("%d",prod[k]);
			printf("%c",'\n');
		}
	}
	return prod;
}

long denom(int x, int points[], int numPoints){
	int i;
	long den = 1;
	long temp;
	for(i = 0; i < numPoints; ++i){
		temp = x-points[i];
		den *= temp;
	}
	printf("%d %c", den, '\n');
	return den;
}

int findMod(long denoms [], int size){
	int mersenne[] = {7,31,127,8191};
	int i;
	int j;
	int count;
	for(i=0; i<4;++i){
 		
		for(j=0; j<size; ++j){
			if(denoms[j]%mersenne[i]!= 0){
				++count; 
			} else{
			continue; 	
			}		
		}
		if(count==size){
			printf("%d %c", mersenne[i], '\n');
			return mersenne[i];
		}
	}
	printf("%s", "no prime found");
	return 0;
}

int main(int argc, char * argv){
	int points[] = {-65,-67,-71,-84};
	int prod[5];
	int numPoints = sizeof(points)/sizeof(points[0]);
	polyMult(points,prod,numPoints);
	long denoms[numPoints];
	denoms[0] = denom(-68, points, numPoints);
	denoms[1] = denom(-75, points, numPoints);
	findMod(denoms, 2);
	return 0;
}
