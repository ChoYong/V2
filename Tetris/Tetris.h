#pragma once
#include "Matrix.h"
#include <cmath>

#define MAX_TET_OPS 100
#define MAX_BLK_DEGREES 4

enum TetrisState { Running, NewBlock, Finished };

class Tetris;

class ActionHandler {
public:
	virtual bool run(Tetris *t, char key) = 0;
};

class Tetris {
private:
	static int max(int a, int b) { return (a > b ? a : b); }
	int *createArrayScreen(int dy, int dx, int dw);
protected:
	TetrisState state;
	static Matrix **createSetOfBlocks(int *setOfArrays[]);
	static int findBlockSize(int *setOfArrays[], int t, int d);
	static int findLargestBlockSize(int *setOfArrays[]);
	class TetrisOperation {
	public:
		char key;
		ActionHandler *hDo, *hUndo;
		TetrisState preState, postStateDo, postStateUndo;
	};
	int nops = 0;
	int max_ops = MAX_TET_OPS;
	TetrisOperation *operations = new TetrisOperation[max_ops];
	TetrisOperation *findOperationByKey(char key);
public:
	static int iScreenDw; // large enough to cover the largest block
	static int nBlockTypes; // number of block types (typically 7)
	static int nBlockDegrees; // number of block degrees (typically 4)
	static Matrix **setOfBlockObjects; // Matrix object arrays of all blocks
	int iScreenDy; // height of the background screen
	int iScreenDx; // width of the background screen
	int top; // y of the top left corner of the current block
	int left; // x of the top left corner of the current block
	int idxBlockType; // index for the current block type
	int idxBlockDegree; // index for the current block degree
	int currDy, currDx;
	Matrix *iScreen; // input screen (as background)
	Matrix *oScreen; // output screen
	Matrix *currBlk; // current block
	Tetris();
	~Tetris();
	Tetris(int cy, int cx);
	static void init(int *setOfBlockArrays[], int types, int degrees);
//	void printScreen();
	void setOperation(char key, TetrisState preState, ActionHandler *hDo,
		TetrisState postStateDo, ActionHandler *hUndo,
		TetrisState postStateUndo);
	bool anyConflict(bool updateNeeded);
	TetrisState accept(char key);
};

class OnLeft : public ActionHandler {
public:
	bool run(Tetris *t, char key) {
		t->left = t->left - 1;
		return t->anyConflict(true);
	}
};

class OnRight : public ActionHandler {
public:
	bool run(Tetris *t, char key) {
		t->left = t->left + 1;
		return t->anyConflict(true);
	}
};

class OnDown : public ActionHandler {
public:
	bool run(Tetris *t, char key) {
		t->top = t->top + 1;
		return t->anyConflict(true);
	}
};

class OnUp : public ActionHandler {
public:
	bool run(Tetris *t, char key) {
		t->top = t->top - 1;
		return t->anyConflict(true);
	}
};

class OnDrop : public ActionHandler {
public:
	bool run(Tetris *t, char key) {
		do { t->top = t->top + 1; } while (t->anyConflict(false) == false);
		return t->anyConflict(true);
	}
};

class OnCW : public ActionHandler {
public:
	bool run(Tetris *t, char key) {
		t->idxBlockDegree = (t->idxBlockDegree + 1) % t->nBlockDegrees;
		t->currBlk = &(t->setOfBlockObjects[t->idxBlockType][t->idxBlockDegree]);
		return t->anyConflict(true);
	}
};

class OnCCW : public ActionHandler {
public:
	bool run(Tetris *t, char key) {
		t->idxBlockDegree = (t->idxBlockDegree + MAX_BLK_DEGREES - 1) % t->nBlockDegrees;
		t->currBlk = &(t->setOfBlockObjects[t->idxBlockType][t->idxBlockDegree]);
		return t->anyConflict(true);
	}
};

class OnNewBlock : public ActionHandler {
public:
	bool run(Tetris *t, char key) {
		t->oScreen = deleteFullLines(t->oScreen, t->currBlk, t->top,
			t->iScreenDy, t->iScreenDx, t->iScreenDw);
		t->iScreen = new Matrix(t->oScreen);
		t->top = 0;
		t->left = t->iScreenDw + t->iScreenDx / 2 - t->iScreenDw / 2;
		t->idxBlockType = key - '0'; // copied from key
		t->idxBlockDegree = 0;
		t->currBlk = &(t->setOfBlockObjects[t->idxBlockType][t->idxBlockDegree]);
		return t->anyConflict(true);
	}
protected:
	Matrix *deleteFullLines(Matrix *screen, Matrix *blk, int top,
		int dy, int dx, int dw) {
		Matrix *line, *zero, *temp;
		if (blk == NULL) // called right after the game starts. 
			return screen; // no lines to be deleted
		int cy, y, nDeleted = 0, nScanned = blk->get_dy();
		if (top + blk->get_dy() - 1 >= dy)
			nScanned -= (top + blk->get_dy() - dy);
		zero = new Matrix(1, dx - 2 * dw);
		for (y = nScanned - 1; y >= 0; y--) {
			cy = top + y + nDeleted;
			line = screen->clip(cy, 0, cy + 1, screen->get_dx());
			if (line->sum() == screen->get_dx()) {
				temp = screen->clip(0, 0, cy, screen->get_dx());
				screen->paste(temp, 1, 0);
				screen->paste(zero, 0, dw);
				nDeleted++;
			}
		}
		delete line;
		delete temp;
		delete zero;
		return screen;
	}
};

class OnFinished : public ActionHandler {
public:
	bool run(Tetris *t, char key) {
		cout << "OnFinished.run() called" << endl;
		return false;
	}
};
