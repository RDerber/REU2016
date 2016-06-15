#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*
 *LaGrangeGen.c
 *
 *Generalized LaGrange function
 *
 *Inputs: input.txt, output.txt, [char1] [char2]..., [yValue1] [yValue2]...,
 *
 *Creates Lagrange polynomial that maps the characters (char1, char2, ...) to their corresponding yValues (yValue1, yValue2, ...)
 *
 *Reads in characters from input.txt, then converts them using the generated polynomial into yValues, and outputs the yValues to output.txt
 *
 *Possible Issues: doesn't ignore new line characters, undefined behavior for all non specified characters
 */







int* polyMult(int points[], long long prod[], int numPoints, long long mod){ 	//Takes in an array of points (with one 
										//already removed) and the size of this array
										//and modifies prod[] so it contains the 
										//coefficients of the numerators of each line
	int i;
	int k;
	memset(prod,0,((numPoints+1)*sizeof(prod[0])));

	prod[0] = 1;					//next 3 lines perform 1st polynomial mult
	prod[1] = (points[0] + points[1]) % mod;
	prod[2] = (points[0]*points[1]) % mod;

	for(i = 2; i< numPoints; ++i){

		int j;
		long long prodTemp[numPoints+1];
		memset(prodTemp,0,sizeof(prodTemp));

		for(k=0;k<numPoints+1;++k){
			prodTemp[k]=prod[k];
		}
		for(j = 1;j <= i;++j){
			long long last = prod[i];
			if(j==1){
				prod[j] = (prod[j] + points[i]) % mod;
                        }else {
				if(j==i){
                                	prod[i+1] = last;		//copies prod[i] to prod[i+1]
				}
				prod[j] = ((prodTemp[j-1]*points[i])%mod + prodTemp[j])%mod;
			}
		}
		prod[i+1] = (prod[i+1] * points[i])%mod;
	}
	return 0;
}

long long denom(int x, int tempPoints[], int numPoints, long long mod){ // Takes in the removed point, the array of points without
									// the removed point, and the size of that array,
									//  returns the denominator of that line 
	int i;
	long long den = 1;
	long long temp;
	for(i = 0; i < numPoints; ++i){
		temp = -x+tempPoints[i];
		den = (den * temp) % mod ;	//modded by the product of mersenne primes to maintain information but avoid overflow
	}

	return den;
}



int findMod(long long denoms [], int size, int maxY){ 	// Takes in all denominators and finds a mersenne prime that is
							// not a factor of any of them
	int mersenne[] = {7,31,127,8191};
	int i;
	int j;
	int count;
	int mersenneSize = sizeof(mersenne)/sizeof(mersenne[0]);

	for(i=0; i<mersenneSize;++i){

		if(mersenne[i] <= maxY){}	//skips a mersenne prime if it is smaller than the maximum output
		else{

 		count = 0;

			for(j=0; j<size; ++j){

				if((denoms[j]%mersenne[i])!= 0){
					++count; 

				}

				if(count==size){

					return mersenne[i];

				}
			}
		}
	}
	printf("%s", "no prime found");
	return 1;
}

int modInverse(long long num, int mod){ // Takes in a number A  and a modulus number B; returns the inverse mod of AmodB 
				 	 // So that (inversMod * A) mod B =1
				 	 // As B is prime, we use Fermat's little theorem (Flt) to find this inverse 
	int inverse = 1;
	num = num % mod; // the "a" value in Flt
	int modm2= mod - 2;
	int mult = (num+mod) % mod;
	int i;

	for(i = 0; i < (sizeof(modm2)*8); ++i){
		if((modm2>>i) & '\x01'){
			inverse = (inverse * mult) % mod;
		}
		mult = (mult*mult) % mod;
	}

	inverse = (inverse + mod) % mod;
	return inverse;
}

int evaluate(int x, int polys[], int polysize, int mod){ // evaluates the polynomial in polys with argument x
                                                 	 // uses fast exponentiation to compute x^n
	int i;
	int sum = 0;
	int polyTable[polysize]; 	// Stores x^(2n)  that will be used to calculate x^m 
	memset(polyTable, 0 , sizeof(polyTable));
	polyTable[0] = x;
	int check = 1;
	int log = 0;

	for(i = 0; i < polysize; ++i){
		int poly = i;
		int j;
		int product = 1;

		if(i & check){
			++log;
			polyTable[log] = (polyTable[log-1]*polyTable[log-1])%mod;
			check <<= 1;
		}
		for(j = 0; j < log; ++j){
			if(((poly) >> j) & '\x01'){

				product = (product * polyTable[j]) % mod;
			}
		} 
		sum = (sum + (product * (polys[polysize - i - 1] % mod)) % mod) % mod;
	}
	sum = (sum + mod) % mod;
	return sum;
}

int polyGenerator(int points[], int yValues[], int size, int poly[]){ 	// Takes in all points, their correspoding y-values, the size
									// of the points array, and the poly array to store the coeffs in
	int i=0;
	long long polyTemp[size][size];
	long long denoms[size]; 		// keeps track of denomInator of each row
	memset(denoms, 0, sizeof(denoms)); 	// Initializes arrays to all 0s in memory 
	memset(poly, 0, sizeof(poly));
	long long mod = 225735769; 	// Works as long as y-values do not exceed 8190 
					// we mod by the least common multiple of all possible primes that
					// we could later mod by to retain information but avoid an overflow.
					// in the default case this is 7*31*127*8191
	for(i=0; i<size; ++i){
		int j;
		int pointsTemp[size-1];// size of pointsTemp is one less than number of Points as we remove one point during each iteration

		for(j = 1; j < size; ++j){
			pointsTemp[j-1] = points[(j+i)%size]; // store the coefficients in order for each row, negative because (x-point)
		}

		polyMult(pointsTemp,polyTemp[i], size-1, mod);
		denoms[i] = denom(points[i], pointsTemp, size-1,mod);		//The size of the pointsTemp array is one 
	}									//less than the number of points
	int maxY = 0;

	for(i=0; i<size; ++i){
		if(yValues[i]>maxY){
			maxY=yValues[i];
		}
	}
	mod = findMod(denoms, size, maxY);

	int j;
	for(i=0; i<size;++i){
		int modin = modInverse(denoms[i], mod);
		for(j=0; j < size; ++j){
			polyTemp[i][j] = (polyTemp[i][j] * modin) % mod;
			poly[j]= ((yValues[i] * polyTemp[i][j])%mod + poly[j])%mod;
		}
	}
	return mod;
}

int main (int argc, char **argv){	//allows user to specify input characters and the names of an input file and an output file.
					//The input file's contents are transformed using the polynomial formed from polyGenerator
					//and the evaluate method and then written to a file with the name specified by them
	FILE * ifp;
	FILE * ofp;
	if((ifp = fopen(argv[1], "r")) == NULL){
		printf("error opening input file");
		return -1;
	}
	if((ofp = fopen(argv[2], "w")) == NULL){
		printf("error opening output file");
		return -1;
	}
	int size = (argc - 3)/2;
	int points[size];
	int yValues[size];
	memset(yValues,0,sizeof(yValues));

	int i;
	for(i = 0; i < size; ++i){
		points[i] = - *argv[i+3];
		yValues[i] = atoi(argv[i + 3 + size]);
		printf("%s %d %s %d %c","Point: ", points[i], "yValue: ", yValues[i],'\n');
	}

	int poly[size];
	memset(poly,0,sizeof(poly));
	int mod = polyGenerator(points, yValues, size, poly);
	int ch=0;
	char temp = 0;

	while((ch = getc(ifp)) != EOF){				//inefficient, haven't gotten around to fixing getc usage.
		temp = evaluate(ch, poly, size, mod);		//should make this a buffer of input file contents that is
		fprintf(ofp, "%d",temp);			//operated on
	}

	fclose(ofp);
	fclose(ifp);
	return 0;
}


