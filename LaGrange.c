#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int* polyMult(int points[], int prod[], int numPoints){
	int i;
	prod[0] = 1;
	prod[1] = points[0] + points[1];
	prod[2] = points[0]*points[1];
//	printf("%d %c",numPoints, '\n');
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
//		for(k=0;k<numPoints+1;++k){
//			printf("%d",prod[k]);
//			printf("%c",'\n');
//		}
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
//	printf("%d %c", den, '\n');
	return den;
}



int findMod(long denoms [], int size){
	int mersenne[] = {7,31,127,8191};
	int i;
	int j;
	int count = 0;

	for(i=0; i<4;++i){
 		
		for(j=0; j<size; ++j){
			if((denoms[j]%mersenne[i])!= 0){
				++count; 
			}
	//	printf("%d %c %d %c %d %c", denoms[j],'\n', denoms[j]%mersenne[i], '\n', count, '\n');
		}
		if(count==size){
	//		printf("%d %c", mersenne[i], '\n');
			return mersenne[i];
		}
	}
	printf("%s", "no prime found");
	return 1;
}

int modInverse(int num, int mod){
	int inverse=1;
	int denom = num;
	denom = denom%mod; // a value in Flt
	int modm2= mod-2;
	int mult=1;
	int i;
	for(i=0;i<(sizeof(mod)*8); ++i){

		if((modm2>>i) & '\x01'){
			inverse = (inverse * mult) % mod;
		}
		
		mult = (mult*denom) % mod;

	}
	
	inverse = (inverse + mod) % mod;

	return inverse;

}

int main(int argc, char * argv){

	int num = ((-15048 % 7)*(-2));
	
	int points[] = {-65,-67,-71,-84};
	int yValues[] = {0,1,2,3};
	int size = sizeof(points)/sizeof(points[0]);
	int i=0;
	int pointsTemp[size][size-1];// size of pointsTemp is one less than number of Points as we remove one point during each iteration
	long denoms[size];
	int poly[size+1];
	memset(denoms, 0, sizeof(denoms));
	memset(poly, 0, sizeof(poly));

	for(i=0; i<size; ++i){
		int j;
		for(j = 1; j < size; ++j){
			pointsTemp[i][j-1] = points[(j+i)%size];
		}
		denoms[i] = denom(points[i], pointsTemp[i], size-1);//The size of the pointsTemp array is one less than the number of points
	}
	//	int i;
//	for(i=0; i < 2; ++i){
//		printf("%d %c %d %c", denoms[i],'\n', denoms[i]%7, '\n');
//	}
//
	int mod = findMod(denoms, size);

	int j;
	for(i=0; i<size;++i){
		int modin = modInverse(denoms[i],mod);
		for(j=0; j < size-1; ++j){
			polyTemp[i][j] = polyTemp[i][j] * modin;
			poly[i] += yValues[i] * polytTemp[i][j];
		}
	}
	
	int check=1;
	int k;
	for(k=0; k<5; ++k){
		check = (check * denoms[1]) % mod;
	}
	
	check = (check+7) % mod;
	
	printf("%d %c %d %c %d %c %d %c %d %c",denoms[1] % mod,'\n',mod,'\n',modin,'\n',(check),'\n', num, '\n');
	
	return 0;
}
