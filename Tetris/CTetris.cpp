#include "CTetris.h"
#include "colors.h"

CTetris::CTetris(int cy, int cx) : Tetris(cy, cx) {
  cout << "CTetris() called" << endl;
};

CTetris::~CTetris() {
  delete iScreen;
  delete oScreen;
  delete currBlk;
  cout << "~CTetris() called" << endl;
}

bool CTetris::anyConflict(bool updateNeeded) {
  bool anyConflict;
  Matrix *iCBlk, *oCBlk; // color blocks
  Matrix *iBBlk, *cBBlk, *oBBlk; // binary blocks

  iCBlk = iScreen->clip(top, left, top + currBlk->get_dy(),
			left + currBlk->get_dx());
  oCBlk = iCBlk->add(currBlk);
  iBBlk = iCBlk->int2bool();
  cBBlk = currBlk->int2bool();
  oBBlk = iBBlk->add(cBBlk);
  
  anyConflict = oBBlk->anyGreaterThan(1);
  delete iBBlk;
  delete cBBlk;
  delete oBBlk;
  
  if (updateNeeded == true) {
    oScreen->paste(iScreen, 0, 0);
    oScreen->paste(oCBlk, top, left);
  }
  delete iCBlk;
  delete oCBlk;
  
  return anyConflict;
}

void generateGhost(CTetris *ct, char key) {
	int blkTop, blkLeft;
	ct->oScreen->clearNegative();
	Matrix *gScreen = new Matrix(ct->oScreen);
	Matrix *gBlk, *giBlk, *goBlk;
	blkTop = ct->top;
	blkLeft = ct->left;
	do { ct->top = ct->top + 1; }
	while (ct->anyConflict(false) == false);
	ct->top = ct->top - 1;
	ct->anyConflict(true);
	giBlk = gScreen->clip(ct->top, ct->left, ct->top + ct->currBlk->get_dy(), ct->left + ct->currBlk->get_dx());
	gBlk = ct->currBlk->int2negative();
	goBlk = giBlk->add(gBlk);
	ct->oScreen->paste(gScreen, 0, 0);
	ct->oScreen->paste(goBlk, ct->top, ct->left);
	ct->oScreen->odd2even();
	ct->top = blkTop;
	ct->left = blkLeft;
	delete giBlk;
	delete gBlk;
	delete goBlk;
	delete gScreen;
}
