#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(){
	struct timeb start,end;
	int i = 0;
	int diff;
	ftime(&start);
	
	//while(1){
		for(int i = 0;i<10; i++){
			t1=clock();
			sleep(1);
			t2=clock();
			printf("Delay at %f", (((float)t2 - (float)t1)));
		}
	//}
	return 0;
}
