#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "jsonData.h"
/*
 *LaGrangeGen.c
 *
 *Generalized LaGrange function
 *
 *Inputs: key.txt input.txt, output.txt 
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

int modMersenne(int num, int mers, int numShift){
//	printf("%s %d\n","num:", num);
//	if(num<0)
//		printf("%s %d\n","num:", num);
	int temp;
	int notMod = ~mers;
//	while(num & notMod){
		temp = num & mers;
		num = temp + (num >> numShift);
//	}

	return num;
}

int evaluate(int x, int polys[], int polysize, int mod, int numShift){ // evaluates the polynomial in polys with argument x
        
                                      	 // uses fast exponentiation to compute x^n
	int i;
	int sum = 0;
	int polyTable[polysize]; 	// Stores x^(2n)  that will be used to calculate x^m 
	memset(polyTable, 0 , sizeof(polyTable));
	int temp;
	temp = x & mod;
	x = temp + (x >> numShift);
	temp = x & mod;
	x = temp + (x >> numShift);
	temp = x & mod;
	x = temp + (x >> numShift);
	polyTable[0] = x;
	int check = 1;
	int log = 0;
	
	for(i = 0; i < polysize; ++i){
		int poly = i;
		int j;
		int product = 1;
		
		if(i & check){
			++log;
			polyTable[log] = modMersenne((polyTable[log-1]*polyTable[log-1]),mod, numShift);
			check <<= 1;
		}
		for(j = 0; j < log; ++j){
			if(((poly) >> j) & '\x01'){
				product = modMersenne((product * polyTable[j]),mod, numShift);
			}
		}
		sum = modMersenne((sum + modMersenne((product * modMersenne((polys[polysize - i - 1]), mod, numShift)), mod, numShift)), mod, numShift);
	}
	sum = modMersenne((sum + mod),mod, numShift);

	return sum;
}

int polyGenerator(int points[], int yValues[], int size, int poly[]){ 	// Takes in all points, their correspoding y-values, the size
									// of the points array, and the poly array to store the coeffs in
	int i=0, j;
	long long polyTemp[size][size];
	long long denoms[size]; 		// keeps track of denomInator of each row
	memset(denoms, 0, sizeof(denoms[0])*size); 	// Initializes arrays to all 0s in memory 
	memset(poly, 0, sizeof(poly[0])*size);
	
	long long mod = 225735769; 	// Works as long as y-values do not exceed 8190 
					// we mod by the least common multiple of all possible primes that
					// we could later mod by to retain information but avoid an overflow.
					// in the default case this is 7*31*127*8191
	for(i=0; i<size; ++i){
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

	for(i=0; i<size;++i){
		int modin = modInverse(denoms[i], mod);
		for(j=0; j < size; ++j){
			polyTemp[i][j] = (polyTemp[i][j] * modin) % mod;
			poly[j]= ((yValues[i] * polyTemp[i][j])%mod + poly[j])%mod;
		}
	}
	return mod;
}

int readInputToBuffer(FILE * ifp, char ** input, long * inputsize){
 	// Go to the end of the file //
	if(fseek(ifp, 0L, SEEK_END)== 0){
		// Get the size of the file. //
		*inputsize = ftell(ifp);
		if (*inputsize == -1) {
			fputs("Error finding size of file", stderr);
		return -1;
		 }
		//Allocate our buffer of that size +1 for null termination. //
		*input = malloc (sizeof(char) * ((*inputsize)+1));
		
		// Return to start of file //
		if(fseek(ifp, 0L, SEEK_SET)!=0 ) {
			fputs("Error returning to start of file", stderr);
			return -1;
		}
		//Read the entire file into memory//
		size_t newLen = fread(*input, sizeof(char), *inputsize, ifp);
		if(newLen == 0){
			fputs("Error reading file", stderr);
			return -1;
		} else {
			// Null termination character at the end of the input buffer //
			(*input)[newLen++] = '\0'; 
		}
		return 0;
	} else return -1;
}


int keyIdentifier(FILE * ifp,int *input,int *output){
	char byt; 
	int inputCount = 0; 
	int outputCount = 0;
	int i = 0, j;
	char numBuf[20];
	while((byt=getc(ifp))!= EOF){
	
		if(byt != ' ' && byt != '\n' && byt != '\r'){
			if(byt != '>'){
				input[inputCount++] = -byt;
			}else{
				while((byt = getc(ifp)) == ' ' || byt == '\n' || byt == '\r'){}
				i=0;
				while(byt != '\n'){
					 numBuf[i++] = byt;
					 byt=getc(ifp);
				}
				numBuf[i++] = '\x00';
				
			//	output[outputCount++] = atoi(numBuf);
			//	continue; 
				
				for(j=0; j < i-1; ++j){
					if(atoi(numBuf) == 0 && numBuf[j] != '0'){ // If it's a character, cast and put into output
						output[outputCount++] = (int)numBuf[j]; 
						break;
					}
					else{
						output[outputCount++] = atoi(numBuf); //If it's a number, convert to int and put into output
						break;
					}
				}
				
			}
		}
	}
	return inputCount;
} 

int getFileSize(FILE *ifp){
 // Go to the end of the file //
 	int keySize;
	if(fseek(ifp, 0L, SEEK_END)== 0){
	// Get the size of the file. //
		keySize = ftell(ifp);
		if (keySize == -1) {
			fputs("Error finding size of file", stderr);
		}
		// Return to start of file //
		if(fseek(ifp, 0L, SEEK_SET)!=0 ) {
			fputs("Error returning to start of file", stderr);
		}
	}
	return keySize;
}

int main (int argc, char **argv){ // [key][input file][output file name] [number of runs]
					//allows user to specify input characters and the names of an input file and an output file.
					//The input file's contents are transformed using the polynomial formed from polyGenerator
					//and the evaluate method and then written to a file with the name specified by them
	FILE * ifp;
	FILE * ofp;
	FILE * kfp;
	int keySize;
	int runs = atoi(argv[4]);
	int i,j;
	long inputSize;
	double polyTime[runs];
	double evalTime[runs];
	char * input;
	struct timeval time0,time1;
	
	kfp = fopen(argv[1],"r");
	if(kfp != NULL){
		keySize = getFileSize(kfp);
	} else{ 
		printf("key file not accessible.");	
		return -1; 
	}
	
	int points[keySize];
	int yValues[keySize];
	memset(yValues,0,sizeof(yValues));
	
	keySize = keyIdentifier(kfp,points,yValues);
	fclose(kfp);
	int poly[keySize];
	memset(poly,0,sizeof(poly[0])*keySize);
	int mod;
	for(i=0;i<runs;++i){
		gettimeofday(&time0,NULL);
		mod = polyGenerator(points, yValues, keySize, poly);
		gettimeofday(&time1,NULL);
		polyTime[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
	}
	
	ifp = fopen(argv[2],"r");
	
	if(ifp != NULL){
		readInputToBuffer(ifp,&input,&inputSize);		
		fclose(ifp);
	}else{
		printf("%s\n", "the input file given does not exist");
		return -1;
	}
	
	char output[inputSize];

// <Begin New Stuff>
	int numShift = 0;
	while((mod >> numShift)&1)++numShift; // Can also use while((mod>>numShift)&1)

	for(i=0; i<keySize; ++i){ // remove negative number values before evaluating 
			poly[i] = (poly[i] + mod)%mod;
	}

// <\End New Stuff>
	
	
	for(i=0;i<runs;i++){
		gettimeofday(&time0,NULL);
		for(j=0;j<inputSize;++j){
			output[j] = evaluate(input[j],poly,keySize,mod, numShift); 
		}
		gettimeofday(&time1,NULL);
		evalTime[i] = (time1.tv_sec-time0.tv_sec)*1000000LL + time1.tv_usec - time0.tv_usec;
	}
	if((ofp = fopen(argv[3], "w")) == NULL){
		printf("error opening output file");
		return -1;
	}

	
	for(i=0;i<inputSize;++i){
		fprintf(ofp,"%d", output[i]);
	}
		
	int numDataForms = 2;
	double *timeArray[numDataForms];
	char *labelArray[numDataForms];	
	timeArray[0] = polyTime;
	timeArray[1] = evalTime;
	
	labelArray[0] = "PolyTimes";
	labelArray[1] = "EvalTimes";
	
	write_laGrange_file(timeArray, labelArray, numDataForms, runs, points, yValues, keySize);
	
	for(i=0;i<keySize;++i){
		printf("%s %d %s %d %c","Point: ", points[i], "yValue: ", evaluate(-points[i],poly,keySize,mod, numShift),'\n');
	}
	
	free(input);
	
	fclose(ofp);
	return 0;
}


