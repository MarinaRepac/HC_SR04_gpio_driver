#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define BUF_LEN 80

double filter(double);

double alpha = 1;
int filter_counter = 0;
double previous_distance;
double current_distance;
double previous_result;
double current_result;

int main()
{
    int file_desc;
    int ret_val;
    char tmp[BUF_LEN];
        
    char c;
    int echo_width;
    double distance = 0;
    double filtered_distance;
    int counter = 0;

   
    /* Wait for user command and check if the right key is pressed. */
    printf("\nPlace the object between 20 cm and 60 cm away from the sensor.\n");
    printf("\n-------------------- Press ENTER to begin --------------------\n");
    
    while(1)
    {
        c = getchar();
		if(c == '\n')
		{
			break;
		}
    }
    
	while(1)
	{
			
		/* Open gpio_driver file. */			
		file_desc = open("/dev/gpio_driver", O_RDWR);

		if(file_desc < 0)
		{
			printf("ERROR: 'gpio_driver' not opened\n");
			return -1;
		}
		
		memset(tmp, 0, BUF_LEN);

		/* Read from gpio_driver file. */
		ret_val = read(file_desc, tmp, BUF_LEN);
		if(ret_val != -1)
		{
			echo_width = atoi(tmp);
			  
			/* Check if the echo width has valid value (range check). */
			if(echo_width < 1160 || echo_width > 3480)
			{
				printf("------------ object is out of range (20cm - 60cm) ------------\n");
				usleep(500000);
			} else
			{
				distance = (double)echo_width/58;
				counter++;	
			}
		} else
		{
			printf("ERROR: 'gpio_driver' read problem\n");
		    return -1;
		}
	
		filtered_distance = filter(distance);
		
		/* Close gpio_driver file. */
		close(file_desc);

		if(counter == 400)
		{			
			printf("Distance: %.2lf cm\n", filtered_distance);
			counter = 0;										
		}											
    }
    
    return 0;
}

/* IIR filter */
double filter(double d)
{
	if(filter_counter != 3)
	{
		filter_counter++;
	}
	
	if((filter_counter) == 1)
	{
		return d;
	}
	
	else if(filter_counter == 2)
	{
		previous_result = d;
		previous_distance = d;
		return previous_result;		
	}
		
	current_distance = d;	
	
	current_result = alpha * (previous_result + current_distance - previous_distance);   
    previous_distance = current_distance;
    previous_result = current_result;
    
    filter_counter = 0;
    
    return current_result;
}
