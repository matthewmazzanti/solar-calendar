#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Number of days in a year
const int YEAR = 365;

// RGB Color, 24 bit
typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} color;

// Day + Color struct, allows pairing a day to a color. Current use is for
// specifying the solinoxes we care about
typedef struct {
	uint16_t day;
	color color;
} day_color;

// Color delta. Defines 3 deltas, one for each color component. Allows for
// linear interpolation between the colors
typedef struct {
	float red;
	float green;
	float blue;
} color_delta;

// Delta between two days, including a color delta. Day delta is wrapped
// per-year EG 355 -> 80 has a day delta of 90
typedef struct {
	uint16_t day;
	color_delta color;
} day_color_delta;

// The key days we want to display on the calendar, the solinoxes. Each defines
// a day and a color to display on that day. Between any two days, we want to
// have a gradient of color from the defined colors
day_color solinoxes[] = {
	{
		.day = 80,
		.color = { .red = 0, .green = 100, .blue = 0 },
	},
	{
		.day = 172,
		.color = { .red = 100, .green = 90, .blue = 0 },
	},
	{
		.day = 266,
		.color = { .red = 165, .green = 42, .blue = 42 },
	},
	{
		.day = 355,
		.color = { .red = 67, .green = 84, .blue = 90 },
	},
};

// For the LEDs - this will need to call the led write function
void write(day_color sol) {
	printf(
		"day: %d red: %d green: %d blue: %d\n",
		sol.day,
		sol.color.red,
		sol.color.green,
		sol.color.blue
	);
}

// For the LEDs - this will need to call the led show function
void show() {
	printf("show colors!\n");
}

// Calculate a day_color_delta from two day_colors
day_color_delta calc_delta(day_color start, day_color end) {
	int day = end.day - start.day;
	if (day < 0) {
		day += YEAR;
	}

	int red = end.color.red - start.color.red;
	int green = end.color.green - start.color.green;
	int blue = end.color.blue - start.color.blue;

	day_color_delta result = {
		.day = day,
		.color =  {
			.red = (float)red / (float)day,
			.green = (float)green / (float)day,
			.blue = (float)blue / (float)day,
		},
	};

	return result;
}

// Linearly interpolate from the start, slope, and an x value. Essentially a 3D
// y = mx + b
color scale(color start, color_delta slope, int x) {
	// We can change the struct, since fucntions copy their arguments
	start.red += x * slope.red;
	start.green += x * slope.green;
	start.blue += x * slope.blue;
	return start;
}

// Draw a gradiet between two day_colors
void draw_gradient(day_color start, day_color end) {
	day_color_delta delta = calc_delta(start, end);

	for (int i = 0; i < delta.day; i++) {
		int curr_day = (i + start.day) % YEAR;

		day_color curr = {
			.day = curr_day,
			.color = scale(start.color, delta.color, i),
		};

		write(curr);
	}
}

// Draw each solinox in the set color
void draw_solinoxes() {
	color color = { .red = 0, .green = 0, .blue = 0 };

	int len = sizeof(solinoxes) / sizeof(day_color);
	for (int i = 0; i < len; i++) {
		day_color curr = {
			.day = solinoxes[i].day,
			.color = color,
		};
		write(curr);
	}
}

// Days each month start at
int month_lengths[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// Draw month start in set color
void draw_month_starts() {
	color color = { .red = 0, .green = 0, .blue = 0 };

	int day = 0;
	int len = sizeof(month_lengths) / sizeof(int);
	for (int i = 0; i < len; i++) {
		day_color curr = { .day = day, .color = color };
		write(curr);
		day += month_lengths[i];
	}
}

int main(void) {
	int solinoxes_len = sizeof(solinoxes) / sizeof(day_color);

	printf("draw gradients\n");
	for (int i = 0; i < solinoxes_len; i++) {
		day_color start = solinoxes[i];
		day_color end = solinoxes[(i + 1) % solinoxes_len];

		printf("\ndraw gradient %d\n", i);
		draw_gradient(start, end);
	}

	printf("\ndraw month starts\n");
	draw_month_starts();

	printf("\ndraw solinoxes\n");
	draw_solinoxes();

	printf("\n");
	show();
}
