#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int mult(int x, int y, int mod){
        return (x*y)%mod;
}

int poww(int x, int power, int mod){
        int i;
        int ans = 1;
        for(i=0;i<power;i++){
                ans = mult(ans,x,mod);
        }
        return ans;
}


int lagrange(char x){
	int xmod4 = x%5;
	double a = 167/100776.0;
	double b= -3175/8398.0;
	double c= 2897065/100776.0;
	double d= -469225/646.0;
	
//	int ans =( 167*x*x*x/(100776.0))-(3175*x*x/8398.0)+(2897065*x/100776.0)-(469225/646.0);

	double am5= 2%5;
	double bm5 = 0%5;
	double cm5 = 0%5;
	double dm5 = 0%5;

	int ans = mult(am5,poww(x,3,5),5) +mult(bm5,poww(x,2,5),5) + mult(cm5,poww(x,1,5),5) + dm5;

	return ans;
}

int [] lagrangePoly(int a, int b, int c, int d){
	int coef[5];
	int i;
	for(i=0;i<5;++i){
		coef[i] = 1;
	}
	int temp[5];
	int coef[2] = bc + db 
	int i;
}

int main (int argc, char *argv[]){
int inputs[argc - 1];
int ans = lagrange(*argv[1]);
printf("%d", ans);
return 0;
}
