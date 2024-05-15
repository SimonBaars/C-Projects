#include <stdio.h>

typedef enum { groen,blauw,rood,oranje } ledkleur;
typedef enum { uit,aan } ledstatus;

struct led {
    int poort;
    ledkleur kleur;
    ledstatus status;
};

typedef struct {
    int poort;
    ledkleur kleur;
    ledstatus status;
} led2;

struct led mijnLed1= {8,groen,aan};

led2 mijnLed2; //struct hoeft niet meerworden aangegeven doordat deze op een andere manier is aangemaakt

int main() {
    mijnLed2.status=uit;
    mijnLed2.poort=7;
    mijnLed2.kleur=rood;

    printf("%d\n", mijnLed2.status);
    return 0;
}
