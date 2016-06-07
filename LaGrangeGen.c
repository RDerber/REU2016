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
 *
 * CURRENT PROBLEMS: 
 *	- Overflow error corrupts number data for more than 10 characters (depending on the character's ascii decimal value) 
 */







int* polyMult(int points[], long long prod[], int numPoints, long long mod){ //Takes in an array of points (with one already removed) and the size of this array
							//and modifies prod[] so it contains the coefficients of the numerators of each line
	int i;
	int k;
	memset(prod,0,((numPoints+1)*sizeof(prod[0])));
//	printf("%s %c","PolyMult: ",'\n');
//	printf("%s %lld \n","Mod: ",mod);

	prod[0] = 1;
	prod[1] = (points[0] + points[1]) % mod;
	prod[2] = (points[0]*points[1]) % mod;
//	printf("%d %c",numPoints, '\n');
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
                                	prod[i+1] = last;
				}
				prod[j] = ((prodTemp[j-1]*points[i])%mod + prodTemp[j])%mod;
			}
		}
		prod[i+1] = (prod[i+1] * points[i])%mod;
//		for(k=0;k<numPoints+1;++k){
//			printf("%s %d %s", "Coefficient of X^",k,": ");
//			printf("%lli",prod[k]);
//			printf("%c",'\n');
//		}
	}

//	printf("%s %c", "End PolyMult.", '\n');
	return 0;
}

long long denom(int x, int tempPoints[], int numPoints, long long mod){ // Takes in the removed point, the array of points without the removed point, and the
						// size of that array, returns the denominator of that line 
	int i;
	long long den = 1;
	long long temp;
	for(i = 0; i < numPoints; ++i){
		temp = -x+tempPoints[i];
		den = (den * temp) % mod ;//modded by the product of mersenne primes to maintain information but avoid overflow
	}
//	printf("%d %c", den, '\n');
	return den;
}



int findMod(long long denoms [], int size, int maxY){ // Takes in all denominators and finds a mersenne prime that is
							// not a factor of any of them
	int mersenne[] = {7,31,127,8191};
	int i;
	int j;
	int count;
	for(i=0; i<4;++i){
//		printf( "%d %c", maxY, '\n');
		if(mersenne[i] <= maxY){
		}
		else{
 		count = 0;
			for(j=0; j<size; ++j){
				if((denoms[j]%mersenne[i])!= 0){
					++count; 
				}	
	//	printf("%d %c %d %c %d %c", denoms[j],'\n', denoms[j]%mersenne[i], '\n', count, '\n');
			}
				if(count==size){
	//				printf("%d %c", mersenne[i], '\n');
					return mersenne[i];
				}
		}
	}
//	int primes[] = {103,107,109,113};                        // These primes should not be needed as all characters in ascii have
//								// a value of less than 8191, so no linear combination of them should
//								// be divisible by 8191, as it is prime.
//	int primeSize = sizeof(primes)/sizeof(primes[1]);
	
//    for(i=0; i < primeSize;++i){
//      count = 0;
//	        for(j=0; j<size; ++j){
//                      if((denoms[j]%primes[i])!= 0){
//                              ++count;
//			}
//             printf("%d %c %d %c %d %c", denoms[j],'\n', denoms[j]%mersenne[i], '\n', count, '\n');
//		}
//		if(count == size){
     //              printf("%d %c", mersenne[i], '\n');
//			return primes[i];
//		}
//	}
	printf("%s", "no prime found");
	return 1;
}

int modInverse(long long num, int mod){ // Takes in a number A  and a modulus number B; returns the inverse mod of AmodB 
				 	 // So that (inversMod * A) mod B =1
				 	 // As B is prime, we use Fermat's little theorem (Flt) to find this inverse 
	int inverse = 1;
	num = num % mod; // the "a" value in Flt
//	printf("%s %d %c", "num: ", num, '\n');
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

int evaluate(int x, int polys[], int polysize, int mod){ // evaluates the polynomial in polys with argument x
                                                 	 // uses fast exponentiation to compute x^n
	int i;
	int sum = 0;
	int polyTable[polysize]; // Stores all X's with power of 2 exponent 
	memset(polyTable, 0 , sizeof(polyTable));
	polyTable[0] = x;
	int check = 1;
	int log = 0;
//	printf("%s %d %c", "size of poly: ", polysize, '\n');
	for(i = 0; i < polysize; ++i){
		int poly = i;
		int j;
		int product = 1;
	//	printf("%s %d \n", "check: ", check);
		if(i & check){
			++log;
			polyTable[log] = (polyTable[log-1]*polyTable[log-1])%mod;
			check <<= 1;
		}
		for(j = 0; j < log; ++j){
//			printf("%s %d \n", "polyTable: ", polyTable[j]);
			if(((poly) >> j) & '\x01'){
				product = (product * polyTable[j]) % mod;
			}
		}
	//	printf(" %s \n", "-------");
		sum = (sum + (product * (polys[polysize - i - 1] % mod)) % mod) % mod;
//		printf("%s %d %s %d %s %d %s %d \n","Product: ",product,", Sum: ",sum, "polys: ", polys[polysize-i-1],"polyTable: ", polyTable[i]);
	}
	sum = (sum + mod) % mod;
	return sum;
}

int polyGenerator(int points[], int yValues[], int size, int poly[]){ // Takes in all points, their correspoding y-values, the size
									// of the points array, and the poly array to store the coeffs in
	int i=0;
	long long polyTemp[size][size];
	long long denoms[size]; // keeps track of denomenator of each row
	memset(denoms, 0, sizeof(denoms)); // Initializes arrays to all 0s in memory 
	memset(poly, 0, sizeof(poly));
	long long mod =225735769; // Works as long as y-values do not exceed 8190 
				//297501458944711;// we mod by the least common denominator of all possible primes that
					// we could later mod by to retain information.
					// old mod number that is multiple of all Mersenne Primes: 225735769;
	for(i=0; i<size; ++i){
		int j;
		int pointsTemp[size-1];// size of pointsTemp is one less than number of Points as we remove one point during each iteration
		for(j = 1; j < size; ++j){
			pointsTemp[j-1] = points[(j+i)%size]; // store the coefficients in order for each row, negative because (x-point)
		
	//		printf("%d %s", pointsTemp[j-1],", "); // pointsTemp is working
		}
	//	printf("%c", '\n');
		polyMult(pointsTemp,polyTemp[i], size-1, mod);
	
	//	for(j=0;j<size;++j){      			// polyMult is working
	//		printf("%d %s", polyTemp[i][j],", ");
	//	}
	//	printf("%c",'\n');

		denoms[i] = denom(points[i], pointsTemp, size-1,mod);//The size of the pointsTemp array is one less than the number of points

//	 	printf("%d %c", denoms[i],'\n');		// denom is working
	}
	//	int i;
//	for(i=0; i < 2; ++i){
//		printf("%d %c %d %c", denoms[i],'\n', denoms[i]%7, '\n');
//	}

	int maxY = 0;
	for(i=0; i<size; ++i){
		if(yValues[i]>maxY){
			maxY=yValues[i];
		}
	}
	mod = findMod(denoms, size, maxY);
//	printf("%d %c", mod, '\n');
	
	int j;
	for(i=0; i<size;++i){
		int modin = modInverse(denoms[i], mod);
//		printf("%d %c", modin, '\n');
		for(j=0; j < size; ++j){
			polyTemp[i][j] = (polyTemp[i][j] * modin) % mod;
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
	memset(yValues,0,sizeof(yValues));
//	int size = sizeof(points)/sizeof(points[0]);
//	printf("%s %d", "Size: ", size);
//	printf("%s %d", "ArgC: ", argc);

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

	int ch=0;
	char temp = 0;
	while((ch = getc(ifp)) != EOF){
		temp = evaluate(ch, poly, size, mod);
//		printf("%s %d %s %d \n","Ch: ", ch, ", temp: ",temp);
		fprintf(ofp, "%d",temp);
	}
	int ans = 0;
	int eval = 67;
	ans = evaluate(eval, poly, size, mod);
	printf("%c %s %d %c %d %c", '\n',"Evaluated at: ",eval,'\n', ans, '\n');

	fclose(ofp);
	fclose(ifp);
	return 0;
}


