/*******************************************************************************************
*
*   raylib - Simple Game template
*
*   <Game title>
*   <Game description>
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014-2019 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
///
///
///  This Demo is based on a template made by Ramon Santamaria for the Raylib library.
///  It has been modified by Dallin Backstrom (dbackstr@ualbert.ca)
///
///  I hearby release all the modifications,
///  insofar as I am authorized to do so,
///  that I have made to the original code,
///  under the same liscence and terms as the
///  original work, that is, a permisive
///  zlib/libpng license.
///
///  Edits exist throughout this source code.
///  they have not been labled, but can be
///  identified by comparing the original template,
///  available at https://github.com/raysan5/raylib
///

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

#define MAX_SCRAMBLE 25

typedef enum State {WAITING = 0, SCRAMBLE, SORT} State;

int *intArray;
Color *pixArray;

Texture2D imageOut;

int state;
int framesDelay;
int oldFrame;

int width;
int height;

// declare functions;

void swapCol(int col1, int col2, int width, int height);
void Draw(State state, Texture2D texture, int framesDelay, int screenWidth, int screenHeight);

// helper Functions

void swap(int num1, int num2) {
   int temp = intArray[num1];
   intArray[num1] = intArray[num2];
   intArray[num2] = temp;

   // mirror any swap made in the int array in the image array,
   swapCol(num1,num2,width,height);
}

void swapCol(int col1, int col2, int width, int height)
{
  int i;
  Color temp;
  for (i=0; i<height-1; i++)
  {
    temp = pixArray[(col1+i*width)];
    pixArray[(col1+i*width)] = pixArray[(col2+i*width)];
    pixArray[(col2+i*width)] = temp;
  }

  // update, and display the updated texture,
  // during the scramble, once every 32 array access
  // during the sort, ever framesDelay array access
  switch(state)
  {
    case SCRAMBLE:
    if (oldFrame < 32) { oldFrame++; }
    else
    {
      oldFrame = 0;
      UpdateTexture(imageOut, pixArray);
      Draw(state, imageOut, framesDelay, GetScreenWidth(), GetScreenHeight());
    }
    break;

    case SORT:
    if (framesDelay == 1)
    {
      UpdateTexture(imageOut, pixArray);
      Draw(state, imageOut, framesDelay, GetScreenWidth(), GetScreenHeight());
    }
    else if ( oldFrame < framesDelay) { oldFrame++; }
    else
    {
      oldFrame = 0;
      UpdateTexture(imageOut, pixArray);
      Draw(state, imageOut, framesDelay, GetScreenWidth(), GetScreenHeight());
    }
    break;

    default:
    UpdateTexture(imageOut, pixArray);
    Draw(state, imageOut, framesDelay, GetScreenWidth(), GetScreenHeight());
    break;
  }
}


int partition(int left, int right, int pivot) {
   int leftPointer = left -1;
   int rightPointer = right;

   while(true) {
      while(intArray[++leftPointer] < pivot) {
         //do nothing
      }

      while(rightPointer > 0 && intArray[--rightPointer] > pivot) {
         //do nothing
      }

      if(leftPointer >= rightPointer) {
         break;
      } else {
        // swaps two items
        swap(leftPointer,rightPointer);
      }
   }
   // swaps the pivot
   swap(leftPointer,right);
   return leftPointer;
}

void quickSort(int left, int right) {
   if(right-left <= 0) {
      return;
   } else {
      int pivot = intArray[right];
      int partitionPoint = partition(left, right, pivot);
      quickSort(left,partitionPoint-1);
      quickSort(partitionPoint+1,right);
   }
}

void scramble(int length)
{
  int i;
  int rand;
  for (i=0; i<length; i++)
  {
    rand = GetRandomValue(0,length-1);
    swap(i,rand);
  }
}

void initArray(int length)
{
  intArray = malloc(length*sizeof(int));
  int i;
  for (i=0; i<length; i++)
  {
    intArray[i] = i+1;
  }
}

void Draw(State state, Texture2D texture, int framesDelay, int screenWidth, int screenHeight)
{
  BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawTexture(texture, screenWidth/2-texture.width/2, screenHeight/2 - texture.height/2 - 40, WHITE );

    DrawText(FormatText("Delay (array access per frame): %i", framesDelay), 120, 10, 20, GREEN);

    switch(state)
    {
      case WAITING:
        DrawText("Press Spacebar to Begin, or change the delay with the arrow keys.",20,350,20,BLACK);
        break;
      case SCRAMBLE:
        DrawText("Scrambling...",20,350,20,BLACK);
        break;
      case
        SORT: DrawText("Sorting...",20,350,20,BLACK);
        break;
      default: break;
    }
  EndDrawing();
}


//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization (Note windowTitle is unused on Android)
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    framesDelay = 1;
    oldFrame = 0;

    state = WAITING;

    int scrambles = 0;

    InitWindow(screenWidth, screenHeight, "raylib template - simple game");

    InitAudioDevice();

    // load image, turn it into a color array, create a texture from it,
    // get it's dimensions for later reference and then unload the image, leaving,
    // the texture in gpu memory and the color array in cpu memory.

    Image cronk = LoadImage("cronk.png");
    pixArray = GetImageData(cronk);
    cronk = LoadImageEx(pixArray, cronk.width, cronk.height);
    imageOut = LoadTextureFromImage(cronk);
    height = cronk.height;
    width = cronk.width;
    UnloadImage(cronk);

    // initialize an orderd array of ints at the pointer intArray,
    // of the same size as the width of the image.
    initArray(width);

    SetTargetFPS(60);               // Set desired framerate (frames-per-second)
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_SPACE) && state == WAITING)
        {
          state = SCRAMBLE;
        }
        if (IsKeyPressed(KEY_RIGHT) && framesDelay < 32){ framesDelay++; }
        if (IsKeyPressed(KEY_LEFT) && framesDelay > 1){ framesDelay--; }



        switch (state) {
          case WAITING: break;
          case SCRAMBLE:
            if(scrambles <= MAX_SCRAMBLE)
            {
              scramble(width);
              scrambles++;
            }
            else
            {
              scrambles = 0;
              state = SORT;
            }
            break;
          case SORT:

            quickSort(0,width-1);

            state = WAITING;
            break;
          default: break;
        }


        UpdateTexture(imageOut, pixArray);
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        Draw(state, imageOut, framesDelay, screenWidth, screenHeight);


        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    // TODO: Unload all loaded data (textures, fonts, audio) here!
    UnloadTexture(imageOut);

    CloseAudioDevice();

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
