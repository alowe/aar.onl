#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#define UNIFORM 0
#define NORMAL 1

int scores[20] = {6, 13, 4, 18, 1, 20, 5, 12, 9, 14, 11, 8, 16, 7, 19, 3, 17, 2, 15, 10};

double randr(double min, double max){
  //returns a random float between min and max 
  //presumably uniform distribution, but 
  double d = (double)rand()/((double) RAND_MAX);
  return d*(max-min)-min;
}

double randn (double mu, double sigma)
{
  //normal distribution with average mu, standard deviation of sigma/2
  //from http://phoxis.org/2013/05/04/generating-random-numbers-from-normal-distribution-in-c/ 
  double U1, U2, W, mult;
  sigma = sigma/2.0;
  static double X1, X2;
  static int call = 0;
 
  if (call == 1)
    {
      call = !call;
      return (mu + sigma * (double) X2);
    }
 
  do
    {
      U1 = -1 + ((double) rand () / RAND_MAX) * 2;
      U2 = -1 + ((double) rand () / RAND_MAX) * 2;
      W = pow (U1, 2) + pow (U2, 2);
    }
  while (W >= 1 || W == 0);
 
  mult = sqrt ((-2 * log (W)) / W);
  X1 = U1 * mult;
  X2 = U2 * mult;
 
  call = !call;
 
  return (mu + sigma * (double) X1);
}

int throwDart(double x, double y, double precision, int distrib){
  //(x,y) is the coords of where the player aims in mm
  //precision is the radius of expected throws in mm

  //using BDO specificiations for dartboard outlined at http://www.bdodarts.com/uploads/files/BDO_PLAYING_RULES.pdf   
  //assuming dart board is diameter 451.0 mm centered at origin (radius 225.5)
  //'Bull' diameter-  12.7mm (radius 6.35)
  //25 ring diameter- 31.8mm (radius 15.9)
  //Doub/Treb rings-   8.0mm wide
  //Outside edge of 'Double' wire to centre bull-  170.0mm
  //Outside edge of 'Trebel' wire to center bull-  107.0mm


  //first adjust throw based on precision
  //TODO: add flag for normal distribution?
  double radius;
  if(distrib == UNIFORM){
    radius = randr(0,precision);
  } else{
    //assume normal
    radius = randn(0,precision);
  }

  double theata = randr(0,2*M_PI); 
  x += radius*cos(theata);
  y += radius*sin(theata);

  //convert adjusted coords to polar
  radius = sqrt(x*x+y*y);

  //now compute the score
  int modifier = 1;
  if (radius<6.35){
    return 50; //Bullseye!
  } else if (radius<15.9){
    return 25; //Almost!
  } else if (radius>225.5){
    return 0; //Rats!
  } else if(radius>162 && radius<170){
    modifier = 2; //Double!
  } else if(radius>99 && radius<107){
    modifier = 3; //Triple!
  }

  //only need to calculate angle if it wasn't bullseye/miss
  theata = fmod(atan2(y,x)+41*M_PI/20.0, 2*M_PI); //returns values between [0, 2*Pi] 
  //shifted by 1/40 of a rotation so that 0-Pi/10 corresponds to the 6 region, Pi/10-2Pi/10 corresponds to 13 region, etc.
  //printf("Angle of %f\n", (theata-M_PI/20.0)*180/M_PI);
  return scores[(int)(theata*10/M_PI)]*modifier;
}

int main(int argc, char *argv[])
{
  //ways to call:
  //ntrials
  //x y ntrials
  //x y ntrials precision distribution (0 for uniform 1 for normal)
  //x y ntrials precision distribution seed 
  double x = 103;
  double y = 0; //default to trip 20
  int ntrials = 1000; //default value if not specified 
  int precision = 10;
  int distribution = NORMAL; //default to normal
  int seed = 12345;
  //clock_t cstart = time(NULL);
  //printf("Time: %d\n", cstart);
  if (argc <= 3){
    if (argc == 2){
      ntrials = atoi(argv[1]);
    }
  if (argc == 3){
    printf("Wrong number of inputs.\n\
        Ignoring second parameter.\n\
        Accepted input formats are:\n\
        Lab1 ntrials\n\
        Lab1 x_coord y_coord ntrials (precision_in_mm, distribution, seed)\n\
        variables in parens are optional default to 10, normal and 12345 respectively\n\
        Use 0 for uniform distribution and 1 for normal distribution");
  }
  } else if (argc >= 4){
    x = (double) atoi(argv[1]);
    y = (double) atoi(argv[2]);
    ntrials = (double) atoi(argv[3]);
    if (argc >= 5){
      precision = atoi(argv[4]);
    }
    if (argc >= 6){
      distribution = atoi(argv[5]);
    }
    if (argc >= 7){
      seed = atoi(argv[6]);
    }
  }
  srand(seed);

  int i;
  long int score = 0;
  int s;

  clock_t cstart = clock();
  printf("Throwing %d darts at location (%f, %f) with precision of %d mm.\n", ntrials, x, y, precision); 
  for(i=0; i<ntrials; i++){
    s = throwDart(x,y,precision,distribution);
    score += s;
  }
  double average = ((double) score)/((double) ntrials);
  clock_t cend = clock();

  clock_t elapsed = cend-cstart;
  double t_elapsed = 1.0 * elapsed / CLOCKS_PER_SEC;
  //cend = time(NULL);
  printf("Process took %f seconds\n", t_elapsed);
  printf("Total score of %d\n", score);
  printf("Average score of %f\n", average);
  FILE *pFile;
  pFile = fopen("results.csv", "a");
  //# trials, total score, average score, time, distribution
  //fprintf(pFile, "%d, %d, %f, %f, %d\n", ntrials, score, average, t_elapsed, distribution);
  fprintf(pFile, "%d, %f, %f, %d, %f, %d, %d\n", ntrials, x, y, (int) score, average, precision, distribution);
  fclose(pFile);
}
