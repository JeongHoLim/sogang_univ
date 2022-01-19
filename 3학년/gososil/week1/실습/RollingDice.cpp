#include "pch.h"
#include "tipsware.h"
#include "time.h"
#define _USE_MATH_DEFINES
#include "math.h"

NOT_USE_MESSAGE

#define MAX_TRY 100

int main()
{
	//srand(time(NULL));

	SelectPenObject(RGB(0, 0, 0));

	int hit = 0;
	int miss = 0;
	float relProb = 5.0 / 36.0;
	float prob = 0;
	int x, y;
	
	// TODO 

	for (int i = 1; i <= MAX_TRY; i++) {
		x = rand() % 6 + 1;
		y = rand() % 6 + 1;

		if (x + y == 8) {
			TextOut(120 * ((i-1) / 20), 20 * ((i-1) % 20), "try %d: %d,%d (hit!)", i, x, y);
			hit++;
		}
		else {
			TextOut(120 * ((i-1) / 20), 20 * ((i-1) % 20), "try %d: %d,%d", i, x, y);

		}
	}

	//
	prob = (float)hit / MAX_TRY;
	SetTextColor(RGB(128, 0, 0));
	
	TextOut(0, 420, "MAX_TRY : %d", MAX_TRY);

	// 실제 주사위 값
	TextOut(0, 440, "실제 값: %.6f", relProb);
	// 계산된 값
	TextOut(0, 460, "계산된 값: %.6f", prob);

	ShowDisplay();

	return 0;
}
