#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <ctype.h>

#define HEIGHT 16
#define WIDTH 50

typedef enum Direction
{
	RIGHT, UP, DOWN, LEFT
} Button;

typedef enum DifficultyLevel
{
	NORMAL = 150,
	HARD = 100,
	INSANE = 50
} Difficulty;

typedef struct TAIL
{
	int tailX;
	int tailY;
	struct TAIL * nextTailNode;
	struct TAIL * prevTailNode;
}tailNode;

void LoadingScreen(void);
int PrintMenu(char * FileName);
void PrintScoreBoard(char * FileName);
void Instructions(char * FileName);
Difficulty SetDifficulty(void);
void DrawGrid(void);
void GoToXY(int x, int y);
tailNode * CreateFirstTailNode(void);
void ShowStats(int score, Difficulty difficulty_level);
const char * DifficultyToString(Difficulty difficulty_level);
void FoodGenerator(int *foodX, int *foodY, int x, int y, tailNode * firstTailNode);
void DrawSnake(int x, int y, int snakeLenght, tailNode * tail, enum Difficulty difficulty_level);
void ImplementXYTailCoordiantes(int x, int y, tailNode * firstTailNode, int snakeLenght);
void InputCheck(int *x, int *y, Button * button, int snakeLenght);
int DirectionCheck(int prevButton, int snakeLenght, Button * button);
int Logic(int x, int y, int foodX, int foodY, int *score, int *snakeLenght);
void CreateTailNode(tailNode * firstTailNode);
int EndGame(int x, int y, tailNode * firstTailNode);
int RestartGame(int *x, int *y, int *snakeLenght, int *score, tailNode * firstTailNode, Difficulty difficulty_level, Button * button, char * FileName);
void DeleteTailNodes(tailNode * firstTailNode);
int DoesFileExist(char * FileName);
void SaveScoreToFile(char * FileName, int * score, Difficulty difficulty_level);

int main(void)
{
	int Score = 0, snakeLenght = 1;
	int x = WIDTH / 2, y = HEIGHT / 2, foodX = 0, foodY = 0; //coordinates
	char FileName[] = "scoreboard.txt";
	tailNode * Tail = NULL;
	Button button = RIGHT;
	Difficulty difficulty_level = NORMAL;

	LoadingScreen();

	while (PrintMenu(FileName))
	{
	difficulty_level = SetDifficulty();
		do {
			DrawGrid();
			Tail = CreateFirstTailNode();
			ShowStats(Score, difficulty_level);
			FoodGenerator(&foodX, &foodY, x, y, Tail);
			do
			{
				ImplementXYTailCoordiantes(x, y, Tail, snakeLenght);
				InputCheck(&x, &y, &button, snakeLenght);
				if (Logic(x, y, foodX, foodY, &Score, &snakeLenght))
				{
					CreateTailNode(Tail);
					ShowStats(Score, difficulty_level);
					FoodGenerator(&foodX, &foodY, x, y, Tail);
				}
				DrawSnake(x, y, snakeLenght, Tail, difficulty_level);
			} while (EndGame(x, y, Tail));
		} while (RestartGame(&x, &y, &snakeLenght, &Score, Tail, difficulty_level, &button, FileName));
	}
	return 0;
}

void LoadingScreen(void)
{
	printf("Game loading..\n");
	for (int i = 0; i < 16; i++)
	{
		if (i < 4)
		{
			printf("%c", 176);
		}
		else if (i >= 4 && i < 8)
		{
			printf("%c", 177);
		}
		else if (i >= 8 && i < 12)
		{
			printf("%c", 178);
		}
		else
		{
			printf("%c", 219);
		}
		Sleep(150);
	}
	system("cls");
	printf("---WELCOME!---");
	Sleep(1000);

}

int PrintMenu(char * FileName)
{
	char UserInput;

	do
	{
		system("cls");
		printf("1. Start game\n2. Scoreboard\n3. How to play\n4. Exit\n");
		UserInput = getch();
		switch (UserInput)
		{
		case '1':
			return 1;
			break;
		case '2':
			PrintScoreBoard(FileName);
			break;
		case '3':
			Instructions(FileName);
			break;
		case '4':
			return 0;
			break;
		default:
			break;
		}
	} while ((UserInput != '1' && UserInput != '4') || (UserInput == '2' || UserInput == '3'));
}

void PrintScoreBoard(char * FileName)
{
	FILE * filePointer;
	int singleCharacter, c;
	filePointer = fopen(FileName, "r");
	system("cls");
	if (filePointer == NULL)
	{
		printf("Scoreboard is empty!");
	}
	else
	{
		while ((singleCharacter = getc(filePointer)) != EOF)
		{
			putchar(singleCharacter);
		}
		fclose(filePointer);
	}
	printf("\nPress any key to continue..");
	while (getch() == NULL);
}

void Instructions(char * FileName)
{
	system("cls");
	printf("Use arrow keys to move the snake around.\nIf the snake hits a wall or bites itself - GAME OVER!");
	printf("\nYou can save your score after the end of each game.\nThe file is located in the game directory under the name %s", FileName);
	printf("\nGood luck and have fun!");
	GoToXY(30, 5);
	printf(" - Game created by: Kamen H. Hristov\n");
	printf("Press any key to continue..");
	while (getch() == NULL);
}

Difficulty SetDifficulty(void)
{
	char user_Input;
	system("cls");
	printf("Choose Difficulty level:\n");
	printf("1. NORMAL\n2. HARD\n3. INSANE!!!\n");

	do
	{
		user_Input = getch();
		switch (user_Input)
		{
		case '1':
			return NORMAL;
			break;
		case '2':
			return HARD;
			break;
		case '3':
			return INSANE;
			break;
		default:
			break;
		}
	} while (user_Input != '1' && user_Input != '2' && user_Input != '3');
}

void DrawGrid(void)
{
	char Grid[HEIGHT][WIDTH];

	system("cls");
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (i == 0 && j == 0) //prints upper left corner
			{
				printf("%c", 201);
			}
			else if (i == 0 && j == WIDTH - 1) //prints upper right corner
			{
				printf("%c", 187);
			}
			else if (i == HEIGHT - 1 && j == 0) //prints lower left corner
			{
				printf("%c", 200);
			}
			else if (i == HEIGHT - 1 && j == WIDTH - 1) //printf lower right corner
			{
				printf("%c", 188);
			}
			else if (i == 0) //prints first row
			{
				printf("%c", 205);
			}

			else if (j == 0) //prints left column
			{
				printf("%c", 186);
			}

			else if (i == HEIGHT - 1) //prints last row
			{
				;
				printf("%c", 205);
			}
			else if (j == WIDTH - 1) //prints right column
			{
				printf("%c", 186);
			}
			else //fills Grid with black matter
			{
				printf(" ");
			}
			if (j == WIDTH - 1) //new line after every row
			{
				printf("\n");
			}
			Sleep(1);
		}
	}
}

void GoToXY(int x, int y) //set the buffer to a specific coordinates on the screen, MSDN
{
	COORD Coordinates;
	Coordinates.X = x;
	Coordinates.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Coordinates);
}

tailNode * CreateFirstTailNode(void)
{
	tailNode * firstTailNode = NULL;
	firstTailNode = (tailNode*)malloc(sizeof(tailNode));
	firstTailNode->nextTailNode = NULL;
	firstTailNode->prevTailNode = NULL;

	return firstTailNode;
}

void ShowStats(int score, Difficulty difficulty_level)
{
	GoToXY(WIDTH + 1, 0);
	printf("Score: %d", score);

	GoToXY(WIDTH + 1, 1);
	printf("%s", DifficultyToString(difficulty_level));


}

const char * DifficultyToString(Difficulty difficulty_level)
{
	switch (difficulty_level)
	{
	case NORMAL:
		return "NORMAL";
		break;
	case HARD:
		return "HARD";
		break;
	case INSANE:
		return "INSANE";
		break;
	}
}

void FoodGenerator(int *foodX, int *foodY, int x, int y, tailNode * firstTailNode) //generates random food objects on grid
{
	tailNode * nodeIterator = firstTailNode;
	srand(time(NULL)); //seed = current time
	int i;
	do //in case coordinates of food coincide with the coordinates of the snake
	{
		*foodX = 2 * rand() % ((WIDTH - 1) - 1) + 1;
		*foodY = rand() % ((HEIGHT - 1) - 1) + 1;
		if (*foodX == x && *foodY == y)
		{

			*foodX = 2 * rand() % ((WIDTH - 1) - 1) + 1;
			*foodY = rand() % ((HEIGHT - 1) - 1) + 1;
		}

		while (nodeIterator->nextTailNode != NULL)
		{
			if (*foodX == nodeIterator->tailX && *foodY == nodeIterator->tailY)
			{
				*foodX = 2 * rand() % ((WIDTH - 1) - 1) + 1;
				*foodY = rand() % ((HEIGHT - 1) - 1) + 1;
			}
			nodeIterator = nodeIterator->nextTailNode;
		}
	} while ((*foodX == nodeIterator->tailX && *foodY == nodeIterator->tailY) && (*foodX == x && *foodY == y));

	GoToXY(*foodX, *foodY);
	printf("*");
}

void DrawSnake(int x, int y, int snakeLenght, tailNode * tail, enum Difficulty difficulty_level)//draws snake's firstNodeSegment & body
{
	tailNode * nodeIterator = tail;
	int k;

	GoToXY(x, y);
	printf("+");
	while (nodeIterator != NULL)
	{
		if (nodeIterator->nextTailNode == NULL)
		{
			GoToXY(nodeIterator->tailX, nodeIterator->tailY);
			printf(" ");
		}
		else
		{
			GoToXY(nodeIterator->tailX, nodeIterator->tailY);
			printf("o");
		}
		nodeIterator = nodeIterator->nextTailNode;
	}
	//GoToXY(nodeIterator->tailX, nodeIterator->tailY);
	//printf(" ");

	GoToXY(0, HEIGHT);
	Sleep(difficulty_level);
}

void ImplementXYTailCoordiantes(int x, int y, tailNode * firstTailNode, int snakeLenght)
{
	tailNode * lastTailNode = NULL;
	tailNode * temporaryNode = firstTailNode;
	while (temporaryNode->nextTailNode != NULL)
	{
		temporaryNode = temporaryNode->nextTailNode;
	}
	lastTailNode = temporaryNode;
	while (lastTailNode->prevTailNode != NULL)
	{
		temporaryNode = lastTailNode->prevTailNode;
		lastTailNode->tailX = temporaryNode->tailX;
		lastTailNode->tailY = temporaryNode->tailY;
		lastTailNode = lastTailNode->prevTailNode;
	}
	lastTailNode->tailX = x;
	lastTailNode->tailY = y;
}

void InputCheck(int *x, int *y, Button * button, int snakeLenght) //validating user input
{
	char c;
	int keyStroke, prevButton = *button;

	if (kbhit())
	{
		keyStroke = getch();
		if (keyStroke == 224) 
		{
			switch (getch())
			{
			case 72:
				*button = UP;
				break;
			case 80:
				*button = DOWN;
				break;
			case 75:
				*button = LEFT;
				break;
			case 77:
				*button = RIGHT;
				break;
			default:
				break;
			}
		}
	}

	if (DirectionCheck(prevButton, snakeLenght, button) != 0) //stops reverse movement
	{
		*button = prevButton;
	}

	switch (*button)
	{
	case UP:
		(*y)--;
		break;
	case DOWN:
		(*y)++;
		break;
	case LEFT:
		*x = (*x) - 2;
		break;
	case RIGHT:
		*x = (*x) + 2;
		break;
	}
}

int DirectionCheck(int prevButton, int snakeLenght, Button * button) //stops reverse movement, called only in InputCheck..
{
	if (snakeLenght > 1)
	{
		if (prevButton == UP && *button == DOWN)
		{
			return 1;
		}
		else if (prevButton == DOWN && *button == UP)
		{
			return 1;
		}
		else if (prevButton == LEFT && *button == RIGHT)
		{
			return 1;
		}
		else if (prevButton == RIGHT && *button == LEFT)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

int Logic(int x, int y, int foodX, int foodY, int *score, int *snakeLenght) //evaluates if the snake has hit a food object
{
	if (x == foodX && y == foodY)
	{
		*score = (*score) + 10;
		(*snakeLenght)++;
		return 1;
	}
	else
	{
		return 0;
	}

}

void CreateTailNode(tailNode * firstTailNode) //creates tail nodes
{
	tailNode * newTailNode = NULL;
	tailNode * temporaryNode = NULL;

	newTailNode = (tailNode*)malloc(sizeof(tailNode));
	temporaryNode = firstTailNode;
	while (temporaryNode->nextTailNode != NULL)
	{

		temporaryNode = temporaryNode->nextTailNode;
	}
	temporaryNode->nextTailNode = newTailNode;
	newTailNode->nextTailNode = NULL;
	newTailNode->prevTailNode = temporaryNode;
}

int EndGame(int x, int y, tailNode * firstTailNode) //evaluate if the snake has bitten itself or has reached the end of the grid
{
	tailNode * nodeIterator = firstTailNode;
	while (nodeIterator->nextTailNode != NULL)
	{
		if (nodeIterator->tailX == x && nodeIterator->tailY == y)
		{
			return 0;
		}
		nodeIterator = nodeIterator->nextTailNode;
	}

	if (x == WIDTH - 1 || y == HEIGHT - 1 || x == -1 || y == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}

}

int RestartGame(int *x, int *y, int *snakeLenght, int *score, tailNode * firstTailNode,Difficulty difficulty_level, Button * button, char * FileName)
{
	char restartGame, isSaveScoreTrue;
	DeleteTailNodes(firstTailNode);
	*snakeLenght = 1;
	*x = WIDTH / 2;
	*y = HEIGHT / 2;
	*button = RIGHT;


	GoToXY(WIDTH / 2 - 5, HEIGHT / 2);
	printf("GAME OVER!");
	if (*score != NULL)
	{
		GoToXY(WIDTH / 2 - 7, HEIGHT / 2 + 1);
		printf("Save score? (Y/N)");
		do
		{
			switch (isSaveScoreTrue = tolower(getch()))
			{
			case 'y':
				SaveScoreToFile(FileName, score, difficulty_level);
				break;
			case 'n':
				break;
			default:
				break;
			}
		} while (isSaveScoreTrue != 'y' && isSaveScoreTrue != 'n');
	}
	*score = 0; // sets score to zero

	GoToXY(0, HEIGHT);
	printf("Play again?\n");
	printf("Press 'Enter' to restart or 'Esc' to exit.. \n");
	do
	{
		restartGame = getch();
		switch (restartGame)
		{
		case 13: //Enter
			return 1;
			break;
		case 27: //Escape
			system("cls");
			return 0;
			break;
		default:
			break;
		}
	} while (restartGame != 13 && restartGame != 27);
}

void DeleteTailNodes(tailNode * firstTailNode)
{
	tailNode * temporaryNode = NULL;
	while (firstTailNode != NULL)
	{
		temporaryNode = firstTailNode;
		firstTailNode = firstTailNode->nextTailNode;
		free(temporaryNode);
	}
}

int DoesFileExist(char * FileName)
{
	FILE * filePointer = NULL;
	if ((filePointer = fopen(FileName, "r")))
	{
		fclose(filePointer);
		return 1;
	}
	else
	{
		return 0;
	}
}

void SaveScoreToFile(char * FileName, int * score, Difficulty difficulty_level)
{
	FILE * filePointer;
	unsigned char Name[3];

	system("cls");
	if (DoesFileExist(FileName))
	{
		filePointer = fopen(FileName, "a");
		fprintf(filePointer, "\n");
	}
	else
	{
		filePointer = fopen(FileName, "w");
	}
	if (filePointer == NULL)
	{
		printf("Oops! An error occurred! Score couldn't be saved!\n");
		fclose(filePointer);
		return;
	}

	printf("Enter a 3 digit tag:\n");
	for (int i = 0; i < 3; i++)
	{
		Name[i] = toupper(getch());
		while (Name[i] >= 0 && Name[i] <= 32)
		{
			Name[i] = toupper(getch());
		}
		putchar(Name[i]);
		fprintf(filePointer, "%c", Name[i]);
	}

	fprintf(filePointer, "\t%d\t%s", *score, DifficultyToString(difficulty_level));
	fclose(filePointer);
}
