#include <wiringPi.h>
#include <stdio.h>

#define BCM_GPIO5 5
#define BCM_GPIO6 6
#define BCM_GPIO13 13
#define BCM_GPIO19 19
#define BCM_GPIO26 26

int main()
{
<<<<<<< HEAD
  int pin[] = {5, 6, 13, 19, 26};
  wiringPiSetupGpio();
  for (int i = 0; i < 5; i++)
    {
      pinMode(pin[i], INPUT);
    }
  while (1)
    {
      for (int i = 0; i < 5; i++)
	{
	  if (digitalRead(pin[i]) == HIGH)
	    {
	      printf("1");
	    }
	    else
	    {
	      printf("0");
	    }
	    printf(" ");
	    
	}
	  printf("\r");
    }
  return 0;
=======
	int pin[] = {5, 6, 13, 19, 26};
	wiringPiSetupGpio();
	for (int i = 0; i < 5; i++)
	{
		pinMode(pin[i], INPUT);
	}
	while (1)
	{
		for (int i = 0; i < 5; i++)
		{
			if (digitalRead(pin[i]) == HIGH)
			{
				printf("1");
			}
			else
			{
				printf("0");
			}
			printf(" ");
		}
		printf("\r");
	}
	return 0;
>>>>>>> c9d511d9f52f8778bd36a6ba8d2db8c4c0af97eb
}
