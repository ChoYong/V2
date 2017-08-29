#include "Tetris.h"

int Tetris::iScreenDw = 0;
int Tetris::nBlockDegrees = 0;
int Tetris::nBlockTypes = 0;
Matrix **Tetris::setOfBlockObjects = NULL;

int *Tetris::createArrayScreen(int dy, int dx, int dw) {
	int y, x;
	int Dy = dy + dw;
	int Dx = dx + 2 * dw;
	int *array = new int[Dy * Dx];
	for (y = 0; y < dy + dw; y++)
		for (x = 0; x < dx + 2 * dw; x++)
			array[y * Dx + x] = 0;
	for (y = 0; y < dy; y++) {
		for (x = 0; x < dw; x++)
			array[y * Dx + x] = 1;
		for (x = dw + dx; x < Dx; x++)
			array[y * Dx + x] = 1;
	}
	for (y = dy; y < Dy; y++)
		for (x = 0; x < Dx; x++)
			array[y * Dx + x] = 1;
	return array;
}

Matrix **Tetris::createSetOfBlocks(int *setOfArrays[]) {
	int i, colRow;
	Matrix **setOfObjects = new Matrix*[nBlockTypes];
	for (i = 0; i < nBlockTypes; i++)
		setOfObjects[i] = new Matrix[nBlockDegrees];
	for (int t = 0; t < nBlockTypes; t++) {
		for (int d = 0; d < nBlockDegrees; d++) {
			colRow = findBlockSize(setOfArrays, t, d);
			setOfObjects[t][d] = *new Matrix(setOfArrays[t * nBlockDegrees + d],
				colRow, colRow);
		}
	}
	return setOfObjects;
}

int Tetris::findBlockSize(int *setOfArrays[], int t, int d) {
	int i, j;
	for (i = 0; i < 1000; i++) {
		if (setOfArrays[t * nBlockDegrees + d][i] < 0) {
			for (j = 1; j < i; j++)
				if (i / j == j) break;
			break;
		}
	}
	return j;
}

int Tetris::findLargestBlockSize(int *setOfArrays[]) {
	int size = 0, colRow;
	for (int t = 0; t < nBlockTypes; t++) {
		for (int d = 0; d < nBlockDegrees; d++) {
			colRow = findBlockSize(setOfArrays, t, d);
			size = max(size, colRow);
		}
	}
	return size;
}

void Tetris::init(int *setOfBlockArrays[], int types, int degrees) {
	nBlockTypes = types; //nBlockTypes = setOfBlockArrays.length;
	nBlockDegrees = degrees; //nBlockDegrees = setOfBlockArrays[0].length;
	setOfBlockObjects = createSetOfBlocks(setOfBlockArrays);
	iScreenDw = findLargestBlockSize(setOfBlockArrays);
}

void Tetris::setOperation(char key, TetrisState preState, ActionHandler *hDo,
	TetrisState postStateDo, ActionHandler *hUndo,
	TetrisState postStateUndo) {
	operations[nops].key = key;
	operations[nops].hDo = hDo;
	operations[nops].hUndo = hUndo;
	operations[nops].preState = preState;
	operations[nops].postStateDo = postStateDo;
	operations[nops].postStateUndo = postStateUndo;
	nops++;
}

Tetris::TetrisOperation *Tetris::findOperationByKey(char key) {
	TetrisOperation *hop = NULL;
	for (int id = 0; operations[id].key != 0; id++) {
		if (operations[id].key == key) {
			hop = &operations[id];
			break;
		}
	}
	return hop;
}


Tetris::~Tetris() {
	delete iScreen;
	delete oScreen;
	delete currBlk;
	for (int i = 0; i < nBlockTypes; i++)
		delete[] setOfBlockObjects[i];
	delete setOfBlockObjects;
}

Tetris::Tetris(int cy, int cx) {
	if (cy < iScreenDw || cx < iScreenDw)
		cout << "too small screen" << endl;
	iScreenDy = cy;
	iScreenDx = cx;
	iScreen = new Matrix(createArrayScreen(iScreenDy, iScreenDx, iScreenDw),
		iScreenDy + iScreenDw, iScreenDx + 2 * iScreenDw);
	oScreen = new Matrix(iScreen);
	currBlk = &(setOfBlockObjects[idxBlockType][idxBlockDegree]); // ?
	state = NewBlock;
}

bool Tetris::anyConflict(bool updateNeeded) {
	bool anyConflict;
	Matrix *iBlk, *oBlk;
	iBlk = iScreen->clip(top, left, top + currBlk->get_dy(),
		left + currBlk->get_dx());
	oBlk = iBlk->add(currBlk);
	if (updateNeeded == true) {
		oScreen->paste(iScreen, 0, 0);
		oScreen->paste(oBlk, top, left);
	}
	anyConflict = oBlk->anyGreaterThan(1);
	delete iBlk;
	delete oBlk;

	return anyConflict;
}

TetrisState Tetris::accept(char key) {
	TetrisOperation *hop = findOperationByKey(key);
	if (hop == NULL) {
		cout << "unknown key!" << endl;
		return state;
	}
	if (state != hop->preState) {
		cout << "state mismatch for the current key!" << endl;
		return state;
	}
	if ((hop->hDo)->run(this, key) == false) // no conflicts!
		return state = hop->postStateDo;
	else { // a conflict occurs!
		hop->hUndo->run(this, key);
		return state = hop->postStateUndo;
	}
}

/*void Tetris::printScreen() {
	Matrix *screen = oScreen; // copied from oScreen
	int dy = screen->get_dy();
	int dx = screen->get_dx();
	int dw = iScreenDw;
	int **array = screen->get_array();
	for (int y = 0; y < dy - dw + 1; y++) {
		for (int x = dw - 1; x < dx - dw + 1; x++) {
			if (array[y][x] == 0)
				cout << "□ ";
			else if (array[y][x] == 1)
				cout << "■ ";
			else
				cout << "X ";
		}
		cout << endl;
	}
}*/
