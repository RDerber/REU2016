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
	int count;

	for(i=0; i<4;++i){
 		count = 0;
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

	int primes[] = {2,3,5,11,13,17,19,23,29,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113};
	int primeSize = sizeof(primes)/sizeof(primes[1]);
	
        for(i=0; i < primeSize;++i){
        count = 0;
	        for(j=0; j<size; ++j){
                        if((denoms[j]%primes[i])!= 0){
                                ++count;
			}
//             printf("%d %c %d %c %d %c", denoms[j],'\n', denoms[j]%mersenne[i], '\n', count, '\n');
		}
		if(count == size){
     //              printf("%d %c", mersenne[i], '\n');
			return primes[i];
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
	for(i = 0; i < (sizeof(modm2)*8); ++i){
		if((modm2>>i) & '\x01'){
			inverse = (inverse * mult) % mod;
//			printf("%s %d %s", "i: ", i, ", ");
		}
		
		mult = (mult*mult) % mod;

	}
	
	inverse = (inverse + mod) % mod;

	return inverse;

}

int evaluate(int x, int polys[], int polysize, int mod){ // evaluate method not working - alters memory, changing polynomial coeff values
	int i;
	int sum = 0;
//	printf("%s %d %c", "size of poly: ", polysize, '\n');
	for(i = 0; i < polysize; ++i){
		int poly = (polysize - i) - 1;
		int j;
		int product = 1;
		int mult = (x + mod) % mod;
		for(j = 0; j < sizeof(poly)*8; ++j){
			if(((poly) >> j) & '\x01'){
				product = (product * mult) % mod;
			}
			mult = (mult*mult)%mod;
		}
		sum = (sum + (product * polys[i]) % mod) % mod;
	}
	sum = (sum + mod) % mod;
	return sum;
}

int polyGenerator(int points[], int yValues[], int size, int poly[]){ // Takes in all points, their correspoding y-values, the size
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
			pointsTemp[j-1] = points[(j+i)%size]; // store the coefficients in order for each row, negative because (x-point)
		
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


	return mod;

	
//	int check=1;
//	int k;
//	for(k=0; k<5; ++k){
//		check = (check * denoms[1]) % mod;
//	}
	
//	check = (check+7) % mod;
	
//	printf("%d %c %d %c %d %c %d %c",denoms[1] % mod,'\n',mod,'\n',modin,'\n',(check),'\n');
	
}

int main (int argc, char *argv[]){
	FILE * ifp;
	FILE * ofp;
	ifp = fopen(argv[1], "r");
	ofp = fopen(argv[2], "w");
	int size = (argc - 3)/2;
	int points[size];
	int yValues[size];
//	int size = sizeof(points)/sizeof(points[0]);
	printf("%s %d", "Size: ", size);
	printf("%s %d", "ArgC: ", argc);

	int i;
	for(i = 0; i < size; ++i){
		points[i] = - *argv[i+3];
		yValues[i] = atoi(argv[i + 3 + size]);
		printf("%s %d %s %d %c","Point: ", points[i], "yValue: ", yValues[i],'\n');
	}

//	int points[] = {-65, -67, -71, -84};
//	int yValues[] = {0, 1, 2, 3};
	int poly[size];
	memset(poly,0,sizeof(poly));
	int mod = polyGenerator(points, yValues, size, poly);
	
	for(i = 0; i < size; ++i){
		printf("%d %s", poly[i],", ");
	}

	int ch;
	char flinec;
	while((flinec = getc(ifp)) != '\n'){
		fprintf(ofp, "%c", flinec);
	}
	fprintf(ofp, "%c", '\n');

	while((ch = getc(ifp)) != EOF){
		char temp = evaluate(ch, poly, size, mod);
		fprintf(ofp, "%d",temp);
	}
	int ans = 0;
	ans = evaluate(13, poly, size, mod);
	printf("%c %d %c", '\n', ans, '\n');

	fclose(ofp);
	fclose(ifp);
	return 0;
}


