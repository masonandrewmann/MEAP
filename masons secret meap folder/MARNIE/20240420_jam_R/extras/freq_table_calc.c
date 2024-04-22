#include <stdio.h>
#include <math.h>

FILE *fptr;
float i = 0;
float freq = 0;

int main(){

fptr = fopen("nums.txt", "w");

for(i = 0; i < 128; i++){
	freq = 0.1092266667f * pow(2, (i-60) / 12.f);
	fprintf(fptr,  "%.6f", freq);
	fprintf(fptr, ", ");
	printf("%.6f", freq);
	printf(", ");
}



fclose(fptr);

return 0;
}


