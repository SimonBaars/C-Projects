#include <stdio.h>
#include <stdlib.h>

typedef struct{
	int studentnummer;
	int *cijferlijst;
} Student;

float bepaalGemiddelde(int array[], int arraySize){
	float totaal = 0.0;
	for(int i = 0; i<arraySize; i++){
		totaal+=array[i];
	}
	return totaal/arraySize;
}

void printStudentInfo(Student student){
	printf("-------------------------------\n");
	printf("Studentnummer: %d\n",student.studentnummer);
	printf("Gemiddelde cijfer: %f\n",bepaalGemiddelde(student.cijferlijst, 15));
	printf("-------------------------------\n");
}

int main(){
	Student *studenten = malloc(10*sizeof(Student));
	for(int i = 0; i<10; i++){
		studenten[i].studentnummer=(rand() % 10000)+50000;
		studenten[i].cijferlijst = malloc(15*sizeof(int));
		for(int j = 0; j<15; j++){
			studenten[i].cijferlijst[j]=rand() % 11;
		}
		printStudentInfo(studenten[i]);
	}
	for(int i = 0; i<10; i++){
		free(studenten[i].cijferlijst);
	}
	free(studenten);
	return 0;
}
