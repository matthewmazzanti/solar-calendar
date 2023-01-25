#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} color;

typedef struct {
	float red;
	float green;
	float blue;
} color_delta;

typedef struct {
	uint16_t day;
	color color;
} solinox;

typedef struct {
	uint16_t day;
	color_delta color;
} solinox_delta;

solinox solinoxes[] = {
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

int start_of_month[] = {1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};

void write(solinox sol) {
	printf(
		"day: %d red: %d green: %d blue: %d\n",
		sol.day,
		sol.color.red,
		sol.color.green,
		sol.color.blue
	);
}

solinox_delta calc_delta(solinox start, solinox end) {
	int day = end.day - start.day;
	if (day < 0) {
		day += 365;
	}

	int red = end.color.red - start.color.red;
	int green = end.color.green - start.color.green;
	int blue = end.color.blue - start.color.blue;

	solinox_delta result = {
		.day = day,
		.color =  {
			.red = (float)red / (float)day,
			.green = (float)green / (float)day,
			.blue = (float)blue / (float)day,
		},
	};

	return result;
}

int main(void) {
	int count_solinoxes = sizeof(solinoxes) / sizeof(solinox);

	for (int i = 0; i < count_solinoxes; i++) {
		solinox start = solinoxes[i];
		solinox end = solinoxes[(i + 1) % count_solinoxes];
		solinox_delta delta = calc_delta(start, end);

		printf(
			"%d %d %d %d\n",
			start.day,
			255,
			255,
			255
		);


		for (int j = 1; j < delta.day; j++) {
			int curr_day = (j + start.day) % 365;

			bool is_start_of_month = false;
			for (int k = 0; k < sizeof(start_of_month) / sizeof(int); k++) {
				if (curr_day == start_of_month[k] - 1) {
					is_start_of_month = true;
					break;
				}
			}

			if (is_start_of_month) {
				printf(
					"%d %d %d %d\n",
					curr_day,
					0,
					0,
					100
				);

				continue;
			}

			color_delta curr = {
				.red = start.color.red + (j * delta.color.red),
				.green = start.color.green + (j * delta.color.green),
				.blue = start.color.blue + (j * delta.color.blue),
			};

			printf(
				"%d %d %d %d\n",
				curr_day,
				(int)curr.red,
				(int)curr.green,
				(int)curr.blue
			);

		}
	}
}
