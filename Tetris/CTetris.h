#pragma once

#include "Tetris.h"

class CTetris;

class CTetris : public Tetris {
public:
	CTetris(int cy, int cx);
	~CTetris();
	bool anyConflict(bool updateNeeded);
//	void printScreen();
};

void generateGhost(CTetris *ct, char key);

class OnCBlkLeft : public ActionHandler {
public:
	bool anyConflict;
	bool run(Tetris *t, char key) {
		CTetris *ct = (CTetris *)t;
		ct->left = ct->left - 1;
		anyConflict = ct->anyConflict(true);
		if (anyConflict == false) generateGhost(ct, key);	
		return anyConflict;
	}
};

class OnCBlkRight : public ActionHandler {
public:
	bool anyConflict;
	bool run(Tetris *t, char key) {
		CTetris *ct = (CTetris *)t;
		ct->left = ct->left + 1;		
		anyConflict = ct->anyConflict(true);
		if (anyConflict == false) generateGhost(ct, key);	
		return anyConflict;
	}
};

class OnCBlkDown : public ActionHandler {
public:
	bool anyConflict;
	bool run(Tetris *t, char key) {
		CTetris *ct = (CTetris *)t;
		ct->top = ct->top + 1;
		anyConflict = ct->anyConflict(true);
		if (anyConflict == false) generateGhost(ct, key);	
		return anyConflict;
	}
};

class OnCBlkUp : public ActionHandler {
public:
	bool anyConflict;
	bool run(Tetris *t, char key) {
		CTetris *ct = (CTetris *)t;
		ct->top = ct->top - 1;
		anyConflict = ct->anyConflict(true);
		if (anyConflict == false) generateGhost(ct, key);	
		return anyConflict;
	}
};

class OnCBlkDrop : public ActionHandler {
public:
	bool run(Tetris *t, char key) {
		CTetris *ct = (CTetris *)t;
		do { ct->top = ct->top + 1; }
		while (ct->anyConflict(false) == false);
		return ct->anyConflict(true);
	}
};

class OnCBlkCW : public ActionHandler {
public:
	bool anyConflict;
	bool run(Tetris *t, char key) {
		CTetris *ct = (CTetris *)t;
		ct->idxBlockDegree = (ct->idxBlockDegree + 1) % ct->nBlockDegrees;
		ct->currBlk = &(ct->setOfBlockObjects[ct->idxBlockType][ct->idxBlockDegree]);
		anyConflict = ct->anyConflict(true);
		if (anyConflict == false) generateGhost(ct, key);	
		return anyConflict;
	}
};

class OnCBlkCCW : public ActionHandler {
public:
	bool anyConflict;
	bool run(Tetris *t, char key) {
		CTetris *ct = (CTetris *)t;
		ct->idxBlockDegree = (ct->idxBlockDegree + MAX_BLK_DEGREES - 1) % ct->nBlockDegrees;
		ct->currBlk = &(ct->setOfBlockObjects[ct->idxBlockType][ct->idxBlockDegree]);
		anyConflict = ct->anyConflict(true);
		if (anyConflict == false) generateGhost(ct, key);	
		return anyConflict;
	}
};

class OnNewCBlock : public ActionHandler {
public:
	bool anyConflict;
	bool run(Tetris *t, char key) {
		CTetris *ct = (CTetris *)t;
		ct->oScreen->clearNegative();
		ct->oScreen = deleteFullLines(ct->oScreen, ct->currBlk, ct->top, ct->iScreenDy, ct->iScreenDx, ct->iScreenDw);
		ct->iScreen = new Matrix(ct->oScreen);
		ct->top = 0;
		ct->left = ct->iScreenDw + ct->iScreenDx / 2 - ct->iScreenDw / 2;
		ct->idxBlockType = key - '0'; // copied from key
		ct->idxBlockDegree = 0;
		ct->currBlk = &(ct->setOfBlockObjects[ct->idxBlockType][ct->idxBlockDegree]);
		anyConflict = ct->anyConflict(true);
		if (anyConflict == false) generateGhost(ct, key);	
		return anyConflict;
	}
protected:
	Matrix *deleteFullLines(Matrix *screen, Matrix *blk, int top, int dy, int dx, int dw) {
		Matrix *line, *bline, *zero, *temp;
		if (blk == NULL) // called right after the game starts. 
			return screen; // no lines to be deleted
		int cy, y, nDeleted = 0, nScanned = blk->get_dy();
		if (top + blk->get_dy() - 1 >= dy)
			nScanned -= (top + blk->get_dy() - dy);
		zero = new Matrix(1, dx - 2 * dw);
		for (y = nScanned - 1; y >= 0; y--) {
			cy = top + y + nDeleted;
			line = screen->clip(cy, 0, cy + 1, screen->get_dx());
			bline = line->int2bool();
			if (bline->sum() == screen->get_dx()) {
				temp = screen->clip(0, 0, cy, screen->get_dx());
				screen->paste(temp, 1, 0);
				screen->paste(zero, 0, dw);
				nDeleted++;
			}
			delete line;
			delete bline;
		}
		delete zero;
		return screen;
	}
};

class OnCFinished : public ActionHandler {
public:
	bool run(Tetris *t, char key) {
		cout << "OnCFinished.run() called" << endl;
		return false;
	}
};
