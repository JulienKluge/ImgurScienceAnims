#include <math.h>
#include <random>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

float ProduceRandomVelocityFloat()
{
	return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
}
void DumpPositionVectors(float xPos[], float yPos[], int length, int dataIndex)
{
	ofstream outFile;
	outFile.open(".\\dataCSV\\" + to_string(dataIndex) + "positions.csv");
	for (int i = 0; i < length; ++i)
	{
		outFile << xPos[i] << "," << yPos[i] << "\n";
	}
	outFile.close();
}

int main()
{
	const int NK = 24;
	const int N = NK * NK;
	const float L = 30;

	float xPos[N];
	float yPos[N];
	float xVel[N];
	float yVel[N];

	for (int i = 0; i < NK; ++i)
	{
		for (int j = 0; j < NK; ++j)
		{
			int index = i * NK + j;
			xPos[index] = static_cast<float>(i + 1) * (L / static_cast<float>(NK + 1));
			yPos[index] = static_cast<float>(j + 1) * (L / static_cast<float>(NK + 1));
			xVel[index] = ProduceRandomVelocityFloat();
			yVel[index] = ProduceRandomVelocityFloat();
		}
	}

	float H = 0.01f;
	float TMax = 300;
	int MaxSteps = static_cast<int>(ceil(TMax / H));
	float T = 0;
	float xFrc[N];
	float yFrc[N];
	float *gParams = new float[MaxSteps];
	int fileIndex = 0;
	for (int Step = 0; Step < MaxSteps; ++Step)
	{
		for (int i = 0; i < N; ++i)
		{
			xPos[i] += H * xVel[i];
			yPos[i] += H * yVel[i];
			if (xPos[i] < 0) {
				xPos[i] = -xPos[i]; xVel[i] = -xVel[i];
			}
			else if (xPos[i] > L) {
				xPos[i] = -xPos[i] + (2.0f * L); xVel[i] = -xVel[i];
			}
			if (yPos[i] < 0) {
				yPos[i] = -yPos[i]; yVel[i] = -yVel[i];
			}
			else if (yPos[i] > L) {
				yPos[i] = -yPos[i] + (2.0f * L); yVel[i] = -yVel[i];
			}
		}
		if (T > 50.0f && (Step % 2) == 0)
		{
			for (int i = 0; i < N; ++i)
			{
				xVel[i] *= 0.998f;
				yVel[i] *= 0.998f;
			}
		}
		for (int i = 0; i < N; ++i)
		{
			xFrc[i] = 0.0f;
			yFrc[i] = 0.0f;
		}
		for (int i = 0; i < N; ++i)
		{
			for (int j = i + 1; j < N; ++j)
			{
				float rx = xPos[i] - xPos[j];
				float ry = yPos[i] - yPos[j];
				float rs = (rx * rx) + (ry * ry);
				float F = 24.0f * (1 / (rs * rs * rs * rs)) * ((2.0f / (rs * rs * rs)) - 1.0f);
				xFrc[i] += (F * rx);
				yFrc[i] += (F * ry);
				xFrc[j] -= (F * rx);
				yFrc[j] -= (F * ry);
			}
		}
		for (int i = 0; i < N; ++i)
		{
			xVel[i] += (H * xFrc[i]);
			yVel[i] += (H * yFrc[i]);
		}
		float gParam = L;
		for (int i = 0; i < N; ++i)
		{
			for (int j = i + 1; j < N; ++j)
			{
				float dx = xPos[i] - xPos[j];
				float dy = yPos[i] - yPos[j];
				float r = sqrtf((dx * dx) + (dy * dy));
				if (r < gParam)
				{
					gParam = r;
				}
			}
		}
		gParams[Step] = gParam;
		T += H;
		if ((Step % 100) == 0)
		{
			++fileIndex;
			DumpPositionVectors(xPos, yPos, N, fileIndex);
			cout << "dump  " << fileIndex << " / 300" << endl;
		}
	}

	delete[] gParams;
	return 0;
}