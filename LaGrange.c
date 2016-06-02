#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int* polyMult(int points[], int prod[], int numPoints){ //Takes in an array of points (with one already removed) and the size of this array
							//and modifies prod[] so it contains the coefficients of the numerators of each line
	int i;
	prod[0] = 1;
	prod[1] = points[0] + points[1];
	prod[2] = points[0]*points[1];
//	printf("%d %c",numPoints, '\n');
	for(i = 2; i< numPoints; ++i){
		int j;
		int prodTemp[numPoints+1];
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
	return 0;
}

long denom(int x, int tempPoints[], int numPoints){ // Takes in the removed point, the array of points without the removed point, and the
						// size of that array, returns the denominator of that line 
	int i;
	long den = 1;
	long temp;
	for(i = 0; i < numPoints; ++i){
		temp = -x+tempPoints[i];
		den *= temp;
	}
//	printf("%d %c", den, '\n');
	return den;
}



int findMod(long denoms [], int size){ // Takes in all denominators and finds a mersenne prime that is not a factor of any of them
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

int modInverse(int num, int mod){ // Takes in a number A  and a modulus number B; returns the inverse mod of AmodB 
				  // So that (inversMod * A) mod B =1 
	int inverse=1;
	num = num % mod; // a value in Flt
	printf("%s %d %c", "num: ", num, '\n');
	int modm2= mod - 2;
	int mult = (num+mod) % mod;
//	printf("%s %d %c", "mult: ", mult, '\n');
	int i;
	for(i=0;i<(sizeof(modm2)*8); ++i){
		if((modm2>>i) & '\x01'){
			inverse = (inverse * mult) % mod;
			printf("%s %d %s", "i: ", i, ", ");
		}
		
		mult = (mult*mult) % mod;

	}
	
	inverse = (inverse + mod) % mod;

	return inverse;

}

int evaluate(int x, int poly []){
	

}

int * polyGenerator(int points[], int yValues[], int size, int poly[]){ // Takes in all points, their correspoding y-values, the size
									// of the points array, and the poly array to store the coeffs in

	int i=0;
	int polyTemp[size][size];
	long denoms[size]; // keeps track of denomenator of each row
	memset(denoms, 0, sizeof(denoms)); // Initializes arrays to all 0s in memory 
	memset(poly, 0, sizeof(poly));

	for(i=0; i<size; ++i){
		int j;
		int pointsTemp[size-1];// size of pointsTemp is one less than number of Points as we remove one point during each iteration
		for(j = 1; j < size; ++j){
			pointsTemp[j-1] = points[(j+i)%size]; // store the coefficients in order for each row
		
	//		printf("%d %s", pointsTemp[j-1],", "); // pointsTemp is working
		}
	//	printf("%c", '\n');
		polyMult(pointsTemp,polyTemp[i], size-1);
	
	//	for(j=0;j<size;++j){      			// polyMult is working
	//		printf("%d %s", polyTemp[i][j],", ");
	//	}
	//	printf("%c",'\n');

		denoms[i] = denom(points[i], pointsTemp, size-1);//The size of the pointsTemp array is one less than the number of points

	 	printf("%d %c", denoms[i],'\n');		// denom is working
	}
	//	int i;
//	for(i=0; i < 2; ++i){
//		printf("%d %c %d %c", denoms[i],'\n', denoms[i]%7, '\n');
//	}
//
	int mod = findMod(denoms, size);
	printf("%d %c", mod, '\n');
	
	int j;
	for(i=0; i<size;++i){
		int modin = modInverse(denoms[i], mod);
		printf("%d %c", modin, '\n');
		for(j=0; j < size; ++j){
			polyTemp[i][j] = polyTemp[i][j] * modin;
			poly[j]= ((yValues[i] * polyTemp[i][j])%mod + poly[j])%mod;
		}
	}


	return 0;

	
//	int check=1;
//	int k;
//	for(k=0; k<5; ++k){
//		check = (check * denoms[1]) % mod;
//	}
	
//	check = (check+7) % mod;
	
//	printf("%d %c %d %c %d %c %d %c",denoms[1] % mod,'\n',mod,'\n',modin,'\n',(check),'\n');
	
}

int main (int argc, char *argv[]){
	
        int points[] = {-65,-67,-71,-84,-83};
        int yValues[] = {0,1,2,3,4};
        int size = sizeof(points)/sizeof(points[0]);
	int poly[size];

	polyGenerator(points, yValues, size, poly);

	int i;
		
	for(i=0; i<size; ++i){
                printf("%d %s", poly[i],", ");

        }


}


