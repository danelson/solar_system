#ifndef PLANETS_H
#define PLANETS_H

void init(void);
void initTexture(void);
void initLights(void);

void display(void);
void keyR(void);
void keyS(void);
void keyI(void);
void keyO(void);
void keyZ(void);
void keyX(void);
void keyUp(void);
void keyDown(void);
void keyLeft(void);
void keyRight(void);
void rehsape(int w, int h);

void keyboard(unsigned char key, int x, int y);
void specialKeyFunc(int Key, int x, int y);

void drawEllipse(float x, float y);
void setupTexture(int index, char name[]);
void createTextureImage(char name[]);

#endif