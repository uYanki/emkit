#define USE_DMA_TO_TFT

#define COLOR_DEPTH 16

#define IWIDTH      128
#define IHEIGHT     128

#define CUBE_SIZE   358

#include <TFT_eSPI.h>

// Library instance
TFT_eSPI tft = TFT_eSPI();  // Declare object "tft"

// Create two sprites for a DMA toggle buffer
TFT_eSprite spr[2] = {TFT_eSprite(&tft), TFT_eSprite(&tft)};

// Toggle buffer selection
bool sprSel = 0;

// Pointers to start of Sprties in RAM
uint16_t* sprPtr[2];

// Define the cube face colors
uint16_t palette[] = {
    TFT_WHITE,        // 1
    TFT_GREENYELLOW,  // 2
    TFT_YELLOW,       // 3
    TFT_PINK,         // 4
    TFT_MAGENTA,      // 5
    TFT_CYAN,         // 6
};

// Used for fps measuring
uint16_t counter     = 0;
long     startMillis = millis();
uint16_t interval    = 100;

// size / 2 of cube edge
float d    = 15;
float px[] = {
    -d, d, d, -d, -d, d, d, -d};
float py[] = {
    -d, -d, d, d, -d, -d, d, d};
float pz[] = {
    -d, -d, -d, -d, d, d, d, d};

// Define the triangles
// The order of the vertices MUST be CCW or the
// shoelace method won't work to detect visible edges
int faces[12][3] = {
    {0, 1, 4},
    {1, 5, 4},
    {1, 2, 5},
    {2, 6, 5},
    {5, 7, 4},
    {6, 7, 5},
    {3, 4, 7},
    {4, 3, 0},
    {0, 3, 1},
    {1, 3, 2},
    {2, 3, 6},
    {6, 3, 7},
};

// mapped coordinates on screen
float p2x[] = {0, 0, 0, 0, 0, 0, 0, 0};
float p2y[] = {0, 0, 0, 0, 0, 0, 0, 0};

// rotation angle in radians
float r[] = {0, 0, 0};

// Frames per second
String fps = "0fps";

// Sprite draw position
int16_t xpos = 0;
int16_t ypos = 0;

// Prime number initial value
int prime_max = 2;

// 3 axis spin control
bool spinX = true;
bool spinY = true;
bool spinZ = true;

// Min and max of cube edges, "int" type used for compatibility with original sketch min() function
int xmin, ymin, xmax, ymax;

/////////////////////////////////////////////////////////// setup ///////////////////////////////////////////////////
void setup()
{
    Serial.begin(115200);

    tft.init();

    tft.fillScreen(TFT_BLACK);

    xpos = 0;
    ypos = (tft.height() - IHEIGHT) / 2;

    // Define cprite colour depth
    spr[0].setColorDepth(COLOR_DEPTH);
    spr[1].setColorDepth(COLOR_DEPTH);

    // Create the 2 sprites
    sprPtr[0] = (uint16_t*)spr[0].createSprite(IWIDTH, IHEIGHT);
    sprPtr[1] = (uint16_t*)spr[1].createSprite(IWIDTH, IHEIGHT);

    // Define text datum and text colour for Sprites
    spr[0].setTextColor(TFT_BLACK);
    spr[0].setTextDatum(MC_DATUM);
    spr[1].setTextColor(TFT_BLACK);
    spr[1].setTextDatum(MC_DATUM);

#ifdef USE_DMA_TO_TFT
    // DMA - should work with ESP32, STM32F2xx/F4xx/F7xx processors
    // NOTE: >>>>>> DMA IS FOR SPI DISPLAYS ONLY <<<<<<
    tft.initDMA();  // Initialise the DMA engine (tested with STM32F446 and STM32F767)
#endif

    // Animation control timer
    startMillis = millis();
}

/////////////////////////////////////////////////////////// loop ///////////////////////////////////////////////////

/**
  Detected visible triangles. If calculated area > 0 the triangle
  is rendered facing towards the viewer, since the vertices are CCW.
  If the area is negative the triangle is CW and thus facing away from us.
*/
int shoelace(int x1, int y1, int x2, int y2, int x3, int y3)
{
    // (x1y2 - y1x2) + (x2y3 - y2x3)
    return x1 * y2 - y1 * x2 + x2 * y3 - y2 * x3 + x3 * y1 - y3 * x1;
}

/**
  Rotates and renders the cube.
**/
void drawCube()
{
    double speed = 90;
    if (spinX)
    {
        r[0] = r[0] + PI / speed;  // Add a degree
    }
    if (spinY)
    {
        r[1] = r[1] + PI / speed;  // Add a degree
    }
    if (spinZ)
    {
        r[2] = r[2] + PI / speed;  // Add a degree
    }

    if (r[0] >= 360.0 * PI / 90.0)
    {
        r[0] = 0;
    }
    if (r[1] >= 360.0 * PI / 90.0)
    {
        r[1] = 0;
    }
    if (r[2] >= 360.0 * PI / 90.0)
    {
        r[2] = 0;
    }

    float ax[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float ay[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float az[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    // Calculate all vertices of the cube
    for (int i = 0; i < 8; i++)
    {
        float px2 = px[i];
        float py2 = cos(r[0]) * py[i] - sin(r[0]) * pz[i];
        float pz2 = sin(r[0]) * py[i] + cos(r[0]) * pz[i];

        float px3 = cos(r[1]) * px2 + sin(r[1]) * pz2;
        float py3 = py2;
        float pz3 = -sin(r[1]) * px2 + cos(r[1]) * pz2;

        ax[i] = cos(r[2]) * px3 - sin(r[2]) * py3;
        ay[i] = sin(r[2]) * px3 + cos(r[2]) * py3;
        az[i] = pz3 - 150;

        p2x[i] = IWIDTH / 2 + ax[i] * CUBE_SIZE / az[i];
        p2y[i] = IHEIGHT / 2 + ay[i] * CUBE_SIZE / az[i];
    }

    // Fill the buffer with color 0 (Black)
    spr[sprSel].fillSprite(TFT_BLACK);

    for (int i = 0; i < 12; i++)
    {
        if (shoelace(p2x[faces[i][0]], p2y[faces[i][0]], p2x[faces[i][1]], p2y[faces[i][1]], p2x[faces[i][2]], p2y[faces[i][2]]) > 0)
        {
            int x0 = p2x[faces[i][0]];
            int y0 = p2y[faces[i][0]];
            int x1 = p2x[faces[i][1]];
            int y1 = p2y[faces[i][1]];
            int x2 = p2x[faces[i][2]];
            int y2 = p2y[faces[i][2]];

            xmin = min(xmin, x0);
            ymin = min(ymin, y0);
            xmin = min(xmin, x1);
            ymin = min(ymin, y1);
            xmin = min(xmin, x2);
            ymin = min(ymin, y2);
            xmax = max(xmax, x0);
            ymax = max(ymax, y0);
            xmax = max(xmax, x1);
            ymax = max(ymax, y1);
            xmax = max(xmax, x2);
            ymax = max(ymax, y2);

            spr[sprSel].fillTriangle(x0, y0, x1, y1, x2, y2, palette[i / 2]);
            if (i % 2)
            {
                int avX = 0;
                int avY = 0;
                for (int v = 0; v < 3; v++)
                {
                    avX += p2x[faces[i][v]];
                    avY += p2y[faces[i][v]];
                }
                avX = avX / 3;
                avY = avY / 3;
            }
        }
    }

    // spr[sprSel].drawString(fps, IWIDTH / 2, IHEIGHT / 2, 4);
    // delay(100);
}

// This is to provide a processing load to see the improvement DMA gives
uint32_t computePrimeNumbers(int32_t n)
{
    if (n < 2)
    {
        return 1;
    }

    int32_t i, fact, j, p = 0;

    // Serial.print("\nPrime Numbers are: \n");
    for (i = 1; i <= n; i++)
    {
        fact = 0;
        for (j = 1; j <= n; j++)
        {
            if (i % j == 0)
            {
                fact++;
            }
        }
        if (fact == 2)
        {
            p = i;  // Serial.print(i); Serial.print(", ");
        }
    }
    // Serial.println();
    return p;  // Biggest
}

void loop()
{
    uint32_t updateTime = 0;  // time for next update
    bool     bounce     = false;
    int      wait       = 0;  // random (20);

    // Random movement direction
    int dx = 1;
    if (random(2))
    {
        dx = -1;
    }
    int dy = 1;
    if (random(2))
    {
        dy = -1;
    }

    // Grab exclusive use of the SPI bus
    tft.startWrite();

    // Loop forever
    while (1)
    {
        // Pull it back onto screen if it wanders off
        if (xpos < -xmin)
        {
            dx     = 1;
            bounce = true;
        }
        if (xpos >= tft.width() - xmax)
        {
            dx     = -1;
            bounce = true;
        }
        if (ypos < -ymin)
        {
            dy     = 1;
            bounce = true;
        }
        if (ypos >= tft.height() - ymax)
        {
            dy     = -1;
            bounce = true;
        }

        if (bounce)
        {
            // Randomise spin
            if (random(2))
            {
                spinX = true;
            }
            else
            {
                spinX = false;
            }
            if (random(2))
            {
                spinY = true;
            }
            else
            {
                spinY = false;
            }
            if (random(2))
            {
                spinZ = true;
            }
            else
            {
                spinZ = false;
            }
            bounce = false;
            // wait = random (20);
        }

        if (updateTime <= millis())
        {
            // Use time delay so sprtie does not move fast when not all on screen
            updateTime = millis() + wait;
            xmin       = IWIDTH / 2;
            xmax       = IWIDTH / 2;
            ymin       = IHEIGHT / 2;
            ymax       = IHEIGHT / 2;
            drawCube();

#ifdef USE_DMA_TO_TFT
            if (tft.dmaBusy())
            {
                prime_max++;  // Increase processing load until just not busy
            }
            tft.pushImageDMA(xpos, ypos, IWIDTH, IHEIGHT, sprPtr[sprSel]);
            sprSel = !sprSel;
#else
#ifdef PRIME_NUMBER_PROCESSOR_LOAD
            prime_max = PRIME_NUMBER_PROCESSOR_LOAD;
#endif
            spr[sprSel].pushSprite(xpos, ypos);  // Blocking write (no DMA) 115fps
#endif

            counter++;
            // only calculate the fps every <interval> iterations.
            if (counter % interval == 0)
            {
                long millisSinceUpdate = millis() - startMillis;
                fps                    = String((int)(interval * 1000.0 / (millisSinceUpdate))) + " fps";
                Serial.println(fps);
                startMillis = millis();
            }
#ifdef PRIME_NUMBER_PROCESSOR_LOAD
            // Add a processor task
            uint32_t pr = computePrimeNumbers(prime_max);
            Serial.print("Biggest = ");
            Serial.println(pr);
#endif
            // Change coord for next loop
            xpos += dx;
            ypos += dy;
        }
    }  // End of forever loop

    // Release exclusive use of SPI bus ( here as a reminder... forever loop prevents execution)
    tft.endWrite();
}
