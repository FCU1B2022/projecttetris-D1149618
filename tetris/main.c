typedef enum {
	RED = 41,
	GREEN,
	YELLOW,
	BLUE,
	PURPLE,
	CYAN,
	WHITE,
	BLACK = 0,
}Color;

typedef enum {
	EMPTY = -1,
	I,
	J,
	L,
	O,
	S,
	T,
	Z
}ShapeId;

typedef struct {
	ShapeId shape;
	Color color;
	int size;
	char rotates[4][4][4; ]
}Shape;

Shape shape[7] = {
	.shape = I;
	.color CYAN;
	.size = 4;

}

typedef struct {
	Color color;
	ShapeId shape;
}Block;

int main()
{
	Color cur;
	for (int i = 0; i < 7; i++)
	{
		for (int r = 0; r < 4; r++)
		{
			for (int s; s < shape[i].size; s++)
			{
				for (int t = 0; t < shape[i].size)
			}
		}
	}
}