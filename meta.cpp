#include "meta.hpp"

//Font character list declaration

extern "C" int FONTCHARS[] = {  //Line break prevents horrifically long warning when building
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,402,417,439,506,507,508,509,510,511,536,537,538,539,593,632,710,711,713,728,729,730,731,732,733,894,900,901,902,903,904,905,906,908,910,911,912,913,914,915,916,917,918,919,920,921,922,923,924,925,926,927,928,929,931,932,933,934,935,936,937,938,939,940,941,942,943,944,945,946,947,948,949,950,951,952,953,954,955,956,957,958,959,960,961,962,963,964,965,966,967,968,969,970,971,972,973,974,976,1012,1024,1025,1026,1027,1028,1029,1030,1031,1032,1033,1034,1035,1036,1037,1038,1039,1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1052,1053,1054,1055,1056,1057,1058,1059,1060,1061,1062,1063,1064,1065,1066,1067,1068,1069,1070,1071,1072,1073,1074,1075,1076,1077,1078,1079,1080,1081,1082,1083,1084,1085,1086,1087,1088,1089,1090,1091,1092,1093,1094,1095,1096,1097,1098,1099,1100,1101,1102,1103,1104,1105,1106,1107,1108,1109,1110,1111,1112,1113,1114,1115,1116,1117,1118,1119,1168,1169,7451,7462,7464,7808,7809,7810,7811,7812,7813,7839,7922,7923,8208,8210,8211,8212,8213,8215,8216,8217,8218,8219,8220,8221,8222,8223,8224,8225,8226,8230,8231,8240,8242,8243,8245,8249,8250,8252,8254,8255,8256,8260,8276,8308,8309,8310,8311,8312,8313,8314,8315,8319,8321,8322,8323,8324,8325,8326,8327,8328,8329,8330,8331,8355,8356,8359,8362,8364,8453,8467,8470,8482,8486,8494,8528,8529,8531,8532,8533,8534,8535,8536,8537,8538,8539,8540,8541,8542,8592,8593,8594,8595,8596,8597,8616,8706,8709,8710,8712,8719,8721,8722,8725,8729,8730,8734,8735,8745,8747,8776,8800,8801,8804,8805,8857,8960,8962,8976,8992,8993,9121,9122,9123,9124,9125,9126,9472,9473,9474,9475,9484,9485,9486,9487,9488,9489,9490,9491,9492,9493,9494,9495,9496,9497,9498,9499,9500,9501,9502,9503,9504,9505,9506,9507,9508,9509,9510,9511,9512,9513,9514,9515,9516,9517,9518,9519,9520,9521,9522,9523,9524,9525,9526,9527,9528,9529,9530,9531,9532,9533,9534,9535,9536,9537,9538,9539,9540,9541,9542,9543,9544,9545,9546,9547,9552,9553,9554,9555,9556,9557,9558,9559,9560,9561,9562,9563,9564,9565,9566,9567,9568,9569,9570,9571,9572,9573,9574,9575,9576,9577,9578,9579,9580,9581,9582,9583,9584,9585,9586,9587,9588,9589,9590,9591,9592,9593,9594,9595,9596,9597,9598,9599,9600,9601,9602,9603,9604,9605,9606,9607,9608,9609,9610,9611,9612,9613,9614,9615,9616,9617,9618,9619,9620,9621,9622,9623,9624,9625,9626,9627,9628,9629,9630,9631,9632,9633,9634,9635,9636,9637,9638,9639,9640,9641,9642,9643,9644,9645,9646,9647,9648,9649,9650,9651,9652,9653,9658,9660,9668,9670,9674,9675,9679,9688,9689,9698,9699,9700,9701,9702,9744,9786,9787,9788,9792,9794,9824,9827,9829,9830,9834,9835,10003,64257,64258,65533};

//Optimized functions
//myGetGlyphIndex is long, so placed at bottom of the file

void myDrawTexture(Texture2D texture, Rectangle sourceRec, Rectangle destRec, Vector2 origin, float rotation, Color tint)
{
    // Check if texture is valid
    if (texture.id > 0)
    {
        float width = (float)texture.width;
        float height = (float)texture.height;

        bool flipX = false;

        if (sourceRec.width < 0) { flipX = true; sourceRec.width *= -1; }
        if (sourceRec.height < 0) sourceRec.y -= sourceRec.height;

        rlEnableTexture(texture.id);

        rlPushMatrix();
            rlTranslatef(destRec.x, destRec.y, 0.0f);
//            rlRotatef(rotation, 0.0f, 0.0f, 1.0f);
//            rlTranslatef(-origin.x, -origin.y, 0.0f);

            rlBegin(RL_QUADS);
                rlColor4ub(tint.r, tint.g, tint.b, tint.a);
                rlNormal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer

                // Bottom-left corner for texture and quad
                if (flipX) rlTexCoord2f((sourceRec.x + sourceRec.width)/width, sourceRec.y/height);
                else rlTexCoord2f(sourceRec.x/width, sourceRec.y/height);
                rlVertex2f(0.0f, 0.0f);

                // Bottom-right corner for texture and quad
                if (flipX) rlTexCoord2f((sourceRec.x + sourceRec.width)/width, (sourceRec.y + sourceRec.height)/height);
                else rlTexCoord2f(sourceRec.x/width, (sourceRec.y + sourceRec.height)/height);
                rlVertex2f(0.0f, destRec.height);

                // Top-right corner for texture and quad
                if (flipX) rlTexCoord2f(sourceRec.x/width, (sourceRec.y + sourceRec.height)/height);
                else rlTexCoord2f((sourceRec.x + sourceRec.width)/width, (sourceRec.y + sourceRec.height)/height);
                rlVertex2f(destRec.width, destRec.height);

                // Top-left corner for texture and quad
                if (flipX) rlTexCoord2f(sourceRec.x/width, sourceRec.y/height);
                else rlTexCoord2f((sourceRec.x + sourceRec.width)/width, sourceRec.y/height);
                rlVertex2f(destRec.width, 0.0f);
            rlEnd();
        rlPopMatrix();

        rlDisableTexture();
    }
}

void myDrawText(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint)
{
    int length = strlen(text);      // Total length in bytes of the text, scanned by codepoints in loop

    int textOffsetY = 0;            // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;       // Offset X to next character to draw
    
    float scaleFactor = fontSize/font.baseSize;     // Character quad scaling factor

    for (int i = 0; i < length; i++)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetNextCodepoint(&text[i], &codepointByteCount);
        int index = myGetGlyphIndex(codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;

        if (codepoint == '\n')
        {
            // NOTE: Fixed line spacing of 1.5 line-height
            // TODO: Support custom line spacing defined by user
            textOffsetY += (int)((font.baseSize + font.baseSize/2)*scaleFactor);
            textOffsetX = 0.0f;
        }
        else
        {
            if ((codepoint != ' ') && (codepoint != '\t')) 
            {
                Rectangle rec = { position.x + textOffsetX + font.chars[index].offsetX*scaleFactor,
                                  position.y + textOffsetY + font.chars[index].offsetY*scaleFactor, 
                                  font.recs[index].width*scaleFactor, 
                                  font.recs[index].height*scaleFactor };
    
                myDrawTexture(font.texture, font.recs[index], rec, (Vector2){ 0, 0 }, 0.0f, tint);
            }

            if (font.chars[index].advanceX == 0) textOffsetX += ((float)font.recs[index].width*scaleFactor + spacing);
            else textOffsetX += ((float)font.chars[index].advanceX*scaleFactor + spacing);
        }
        
        i += (codepointByteCount - 1);   // Move text bytes counter to next codepoint
    }
}

/******************************************************************************/
//getLevelIFStream() and getLevelFileP()
//Checks multiple locations for filename and sets string correctly so that
//the file can be written back to later.
/******************************************************************************/

ifstream getLevelIFStream(string& fileName) {
    ifstream levelFile;
    levelFile.open(fileName);
    if (!levelFile) {
        string attempt2 = string("levels/").append(fileName);
        levelFile.open(attempt2);
        if (!levelFile) {
            string attempt3 = string("levels\\").append(fileName);
            levelFile.open(attempt3);
            if (!levelFile) {
                cerr << "Could not open " << fileName << endl;
                exit(EXIT_FAILURE);
            }
            else {
                fileName = attempt3;
            }
        }
        else {
            fileName = attempt2;
        }
    }
    return levelFile;
}

FILE* getLevelFileP(string& fileName) {
    FILE* levelFile = NULL;
    levelFile = fopen(fileName.c_str(), "rb");
    if (!levelFile) {
        string attempt2 = string("levels/").append(fileName);
        levelFile = fopen(attempt2.c_str(), "rb");
        if (!levelFile) {
            string attempt3 = string("levels\\").append(fileName);
            levelFile = fopen(attempt3.c_str(), "rb");
            if (!levelFile) {
                cerr << "Could not open " << fileName << endl;
                exit(EXIT_FAILURE);
            }
            else {
                fileName = attempt3;
            }
        }
        else {
            fileName = attempt2;
        }
    }
    return levelFile;
}

//I don't know how the compiler is actually doing this, but
//this seems like the fastest implementation, ugly as it is.

int myGetGlyphIndex(int codepoint) {
    switch(codepoint) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 4;
        case 5: return 5;
        case 6: return 6;
        case 7: return 7;
        case 8: return 8;
        case 9: return 9;
        case 10: return 10;
        case 11: return 11;
        case 12: return 12;
        case 13: return 13;
        case 14: return 14;
        case 15: return 15;
        case 16: return 16;
        case 17: return 17;
        case 18: return 18;
        case 19: return 19;
        case 20: return 20;
        case 21: return 21;
        case 22: return 22;
        case 23: return 23;
        case 24: return 24;
        case 25: return 25;
        case 26: return 26;
        case 27: return 27;
        case 28: return 28;
        case 29: return 29;
        case 30: return 30;
        case 31: return 31;
        case 32: return 32;
        case 33: return 33;
        case 34: return 34;
        case 35: return 35;
        case 36: return 36;
        case 37: return 37;
        case 38: return 38;
        case 39: return 39;
        case 40: return 40;
        case 41: return 41;
        case 42: return 42;
        case 43: return 43;
        case 44: return 44;
        case 45: return 45;
        case 46: return 46;
        case 47: return 47;
        case 48: return 48;
        case 49: return 49;
        case 50: return 50;
        case 51: return 51;
        case 52: return 52;
        case 53: return 53;
        case 54: return 54;
        case 55: return 55;
        case 56: return 56;
        case 57: return 57;
        case 58: return 58;
        case 59: return 59;
        case 60: return 60;
        case 61: return 61;
        case 62: return 62;
        case 63: return 63;
        case 64: return 64;
        case 65: return 65;
        case 66: return 66;
        case 67: return 67;
        case 68: return 68;
        case 69: return 69;
        case 70: return 70;
        case 71: return 71;
        case 72: return 72;
        case 73: return 73;
        case 74: return 74;
        case 75: return 75;
        case 76: return 76;
        case 77: return 77;
        case 78: return 78;
        case 79: return 79;
        case 80: return 80;
        case 81: return 81;
        case 82: return 82;
        case 83: return 83;
        case 84: return 84;
        case 85: return 85;
        case 86: return 86;
        case 87: return 87;
        case 88: return 88;
        case 89: return 89;
        case 90: return 90;
        case 91: return 91;
        case 92: return 92;
        case 93: return 93;
        case 94: return 94;
        case 95: return 95;
        case 96: return 96;
        case 97: return 97;
        case 98: return 98;
        case 99: return 99;
        case 100: return 100;
        case 101: return 101;
        case 102: return 102;
        case 103: return 103;
        case 104: return 104;
        case 105: return 105;
        case 106: return 106;
        case 107: return 107;
        case 108: return 108;
        case 109: return 109;
        case 110: return 110;
        case 111: return 111;
        case 112: return 112;
        case 113: return 113;
        case 114: return 114;
        case 115: return 115;
        case 116: return 116;
        case 117: return 117;
        case 118: return 118;
        case 119: return 119;
        case 120: return 120;
        case 121: return 121;
        case 122: return 122;
        case 123: return 123;
        case 124: return 124;
        case 125: return 125;
        case 126: return 126;
        case 127: return 127;
        case 160: return 128;
        case 161: return 129;
        case 162: return 130;
        case 163: return 131;
        case 164: return 132;
        case 165: return 133;
        case 166: return 134;
        case 167: return 135;
        case 168: return 136;
        case 169: return 137;
        case 170: return 138;
        case 171: return 139;
        case 172: return 140;
        case 173: return 141;
        case 174: return 142;
        case 175: return 143;
        case 176: return 144;
        case 177: return 145;
        case 178: return 146;
        case 179: return 147;
        case 180: return 148;
        case 181: return 149;
        case 182: return 150;
        case 183: return 151;
        case 184: return 152;
        case 185: return 153;
        case 186: return 154;
        case 187: return 155;
        case 188: return 156;
        case 189: return 157;
        case 190: return 158;
        case 191: return 159;
        case 192: return 160;
        case 193: return 161;
        case 194: return 162;
        case 195: return 163;
        case 196: return 164;
        case 197: return 165;
        case 198: return 166;
        case 199: return 167;
        case 200: return 168;
        case 201: return 169;
        case 202: return 170;
        case 203: return 171;
        case 204: return 172;
        case 205: return 173;
        case 206: return 174;
        case 207: return 175;
        case 208: return 176;
        case 209: return 177;
        case 210: return 178;
        case 211: return 179;
        case 212: return 180;
        case 213: return 181;
        case 214: return 182;
        case 215: return 183;
        case 216: return 184;
        case 217: return 185;
        case 218: return 186;
        case 219: return 187;
        case 220: return 188;
        case 221: return 189;
        case 222: return 190;
        case 223: return 191;
        case 224: return 192;
        case 225: return 193;
        case 226: return 194;
        case 227: return 195;
        case 228: return 196;
        case 229: return 197;
        case 230: return 198;
        case 231: return 199;
        case 232: return 200;
        case 233: return 201;
        case 234: return 202;
        case 235: return 203;
        case 236: return 204;
        case 237: return 205;
        case 238: return 206;
        case 239: return 207;
        case 240: return 208;
        case 241: return 209;
        case 242: return 210;
        case 243: return 211;
        case 244: return 212;
        case 245: return 213;
        case 246: return 214;
        case 247: return 215;
        case 248: return 216;
        case 249: return 217;
        case 250: return 218;
        case 251: return 219;
        case 252: return 220;
        case 253: return 221;
        case 254: return 222;
        case 255: return 223;
        case 256: return 224;
        case 257: return 225;
        case 258: return 226;
        case 259: return 227;
        case 260: return 228;
        case 261: return 229;
        case 262: return 230;
        case 263: return 231;
        case 264: return 232;
        case 265: return 233;
        case 266: return 234;
        case 267: return 235;
        case 268: return 236;
        case 269: return 237;
        case 270: return 238;
        case 271: return 239;
        case 272: return 240;
        case 273: return 241;
        case 274: return 242;
        case 275: return 243;
        case 276: return 244;
        case 277: return 245;
        case 278: return 246;
        case 279: return 247;
        case 280: return 248;
        case 281: return 249;
        case 282: return 250;
        case 283: return 251;
        case 284: return 252;
        case 285: return 253;
        case 286: return 254;
        case 287: return 255;
        case 288: return 256;
        case 289: return 257;
        case 290: return 258;
        case 291: return 259;
        case 292: return 260;
        case 293: return 261;
        case 294: return 262;
        case 295: return 263;
        case 296: return 264;
        case 297: return 265;
        case 298: return 266;
        case 299: return 267;
        case 300: return 268;
        case 301: return 269;
        case 302: return 270;
        case 303: return 271;
        case 304: return 272;
        case 305: return 273;
        case 306: return 274;
        case 307: return 275;
        case 308: return 276;
        case 309: return 277;
        case 310: return 278;
        case 311: return 279;
        case 312: return 280;
        case 313: return 281;
        case 314: return 282;
        case 315: return 283;
        case 316: return 284;
        case 317: return 285;
        case 318: return 286;
        case 319: return 287;
        case 320: return 288;
        case 321: return 289;
        case 322: return 290;
        case 323: return 291;
        case 324: return 292;
        case 325: return 293;
        case 326: return 294;
        case 327: return 295;
        case 328: return 296;
        case 329: return 297;
        case 330: return 298;
        case 331: return 299;
        case 332: return 300;
        case 333: return 301;
        case 334: return 302;
        case 335: return 303;
        case 336: return 304;
        case 337: return 305;
        case 338: return 306;
        case 339: return 307;
        case 340: return 308;
        case 341: return 309;
        case 342: return 310;
        case 343: return 311;
        case 344: return 312;
        case 345: return 313;
        case 346: return 314;
        case 347: return 315;
        case 348: return 316;
        case 349: return 317;
        case 350: return 318;
        case 351: return 319;
        case 352: return 320;
        case 353: return 321;
        case 354: return 322;
        case 355: return 323;
        case 356: return 324;
        case 357: return 325;
        case 358: return 326;
        case 359: return 327;
        case 360: return 328;
        case 361: return 329;
        case 362: return 330;
        case 363: return 331;
        case 364: return 332;
        case 365: return 333;
        case 366: return 334;
        case 367: return 335;
        case 368: return 336;
        case 369: return 337;
        case 370: return 338;
        case 371: return 339;
        case 372: return 340;
        case 373: return 341;
        case 374: return 342;
        case 375: return 343;
        case 376: return 344;
        case 377: return 345;
        case 378: return 346;
        case 379: return 347;
        case 380: return 348;
        case 381: return 349;
        case 382: return 350;
        case 383: return 351;
        case 402: return 352;
        case 417: return 353;
        case 439: return 354;
        case 506: return 355;
        case 507: return 356;
        case 508: return 357;
        case 509: return 358;
        case 510: return 359;
        case 511: return 360;
        case 536: return 361;
        case 537: return 362;
        case 538: return 363;
        case 539: return 364;
        case 593: return 365;
        case 632: return 366;
        case 710: return 367;
        case 711: return 368;
        case 713: return 369;
        case 728: return 370;
        case 729: return 371;
        case 730: return 372;
        case 731: return 373;
        case 732: return 374;
        case 733: return 375;
        case 894: return 376;
        case 900: return 377;
        case 901: return 378;
        case 902: return 379;
        case 903: return 380;
        case 904: return 381;
        case 905: return 382;
        case 906: return 383;
        case 908: return 384;
        case 910: return 385;
        case 911: return 386;
        case 912: return 387;
        case 913: return 388;
        case 914: return 389;
        case 915: return 390;
        case 916: return 391;
        case 917: return 392;
        case 918: return 393;
        case 919: return 394;
        case 920: return 395;
        case 921: return 396;
        case 922: return 397;
        case 923: return 398;
        case 924: return 399;
        case 925: return 400;
        case 926: return 401;
        case 927: return 402;
        case 928: return 403;
        case 929: return 404;
        case 931: return 405;
        case 932: return 406;
        case 933: return 407;
        case 934: return 408;
        case 935: return 409;
        case 936: return 410;
        case 937: return 411;
        case 938: return 412;
        case 939: return 413;
        case 940: return 414;
        case 941: return 415;
        case 942: return 416;
        case 943: return 417;
        case 944: return 418;
        case 945: return 419;
        case 946: return 420;
        case 947: return 421;
        case 948: return 422;
        case 949: return 423;
        case 950: return 424;
        case 951: return 425;
        case 952: return 426;
        case 953: return 427;
        case 954: return 428;
        case 955: return 429;
        case 956: return 430;
        case 957: return 431;
        case 958: return 432;
        case 959: return 433;
        case 960: return 434;
        case 961: return 435;
        case 962: return 436;
        case 963: return 437;
        case 964: return 438;
        case 965: return 439;
        case 966: return 440;
        case 967: return 441;
        case 968: return 442;
        case 969: return 443;
        case 970: return 444;
        case 971: return 445;
        case 972: return 446;
        case 973: return 447;
        case 974: return 448;
        case 976: return 449;
        case 1012: return 450;
        case 1024: return 451;
        case 1025: return 452;
        case 1026: return 453;
        case 1027: return 454;
        case 1028: return 455;
        case 1029: return 456;
        case 1030: return 457;
        case 1031: return 458;
        case 1032: return 459;
        case 1033: return 460;
        case 1034: return 461;
        case 1035: return 462;
        case 1036: return 463;
        case 1037: return 464;
        case 1038: return 465;
        case 1039: return 466;
        case 1040: return 467;
        case 1041: return 468;
        case 1042: return 469;
        case 1043: return 470;
        case 1044: return 471;
        case 1045: return 472;
        case 1046: return 473;
        case 1047: return 474;
        case 1048: return 475;
        case 1049: return 476;
        case 1050: return 477;
        case 1051: return 478;
        case 1052: return 479;
        case 1053: return 480;
        case 1054: return 481;
        case 1055: return 482;
        case 1056: return 483;
        case 1057: return 484;
        case 1058: return 485;
        case 1059: return 486;
        case 1060: return 487;
        case 1061: return 488;
        case 1062: return 489;
        case 1063: return 490;
        case 1064: return 491;
        case 1065: return 492;
        case 1066: return 493;
        case 1067: return 494;
        case 1068: return 495;
        case 1069: return 496;
        case 1070: return 497;
        case 1071: return 498;
        case 1072: return 499;
        case 1073: return 500;
        case 1074: return 501;
        case 1075: return 502;
        case 1076: return 503;
        case 1077: return 504;
        case 1078: return 505;
        case 1079: return 506;
        case 1080: return 507;
        case 1081: return 508;
        case 1082: return 509;
        case 1083: return 510;
        case 1084: return 511;
        case 1085: return 512;
        case 1086: return 513;
        case 1087: return 514;
        case 1088: return 515;
        case 1089: return 516;
        case 1090: return 517;
        case 1091: return 518;
        case 1092: return 519;
        case 1093: return 520;
        case 1094: return 521;
        case 1095: return 522;
        case 1096: return 523;
        case 1097: return 524;
        case 1098: return 525;
        case 1099: return 526;
        case 1100: return 527;
        case 1101: return 528;
        case 1102: return 529;
        case 1103: return 530;
        case 1104: return 531;
        case 1105: return 532;
        case 1106: return 533;
        case 1107: return 534;
        case 1108: return 535;
        case 1109: return 536;
        case 1110: return 537;
        case 1111: return 538;
        case 1112: return 539;
        case 1113: return 540;
        case 1114: return 541;
        case 1115: return 542;
        case 1116: return 543;
        case 1117: return 544;
        case 1118: return 545;
        case 1119: return 546;
        case 1168: return 547;
        case 1169: return 548;
        case 7451: return 549;
        case 7462: return 550;
        case 7464: return 551;
        case 7808: return 552;
        case 7809: return 553;
        case 7810: return 554;
        case 7811: return 555;
        case 7812: return 556;
        case 7813: return 557;
        case 7839: return 558;
        case 7922: return 559;
        case 7923: return 560;
        case 8208: return 561;
        case 8210: return 562;
        case 8211: return 563;
        case 8212: return 564;
        case 8213: return 565;
        case 8215: return 566;
        case 8216: return 567;
        case 8217: return 568;
        case 8218: return 569;
        case 8219: return 570;
        case 8220: return 571;
        case 8221: return 572;
        case 8222: return 573;
        case 8223: return 574;
        case 8224: return 575;
        case 8225: return 576;
        case 8226: return 577;
        case 8230: return 578;
        case 8231: return 579;
        case 8240: return 580;
        case 8242: return 581;
        case 8243: return 582;
        case 8245: return 583;
        case 8249: return 584;
        case 8250: return 585;
        case 8252: return 586;
        case 8254: return 587;
        case 8255: return 588;
        case 8256: return 589;
        case 8260: return 590;
        case 8276: return 591;
        case 8308: return 592;
        case 8309: return 593;
        case 8310: return 594;
        case 8311: return 595;
        case 8312: return 596;
        case 8313: return 597;
        case 8314: return 598;
        case 8315: return 599;
        case 8319: return 600;
        case 8321: return 601;
        case 8322: return 602;
        case 8323: return 603;
        case 8324: return 604;
        case 8325: return 605;
        case 8326: return 606;
        case 8327: return 607;
        case 8328: return 608;
        case 8329: return 609;
        case 8330: return 610;
        case 8331: return 611;
        case 8355: return 612;
        case 8356: return 613;
        case 8359: return 614;
        case 8362: return 615;
        case 8364: return 616;
        case 8453: return 617;
        case 8467: return 618;
        case 8470: return 619;
        case 8482: return 620;
        case 8486: return 621;
        case 8494: return 622;
        case 8528: return 623;
        case 8529: return 624;
        case 8531: return 625;
        case 8532: return 626;
        case 8533: return 627;
        case 8534: return 628;
        case 8535: return 629;
        case 8536: return 630;
        case 8537: return 631;
        case 8538: return 632;
        case 8539: return 633;
        case 8540: return 634;
        case 8541: return 635;
        case 8542: return 636;
        case 8592: return 637;
        case 8593: return 638;
        case 8594: return 639;
        case 8595: return 640;
        case 8596: return 641;
        case 8597: return 642;
        case 8616: return 643;
        case 8706: return 644;
        case 8709: return 645;
        case 8710: return 646;
        case 8712: return 647;
        case 8719: return 648;
        case 8721: return 649;
        case 8722: return 650;
        case 8725: return 651;
        case 8729: return 652;
        case 8730: return 653;
        case 8734: return 654;
        case 8735: return 655;
        case 8745: return 656;
        case 8747: return 657;
        case 8776: return 658;
        case 8800: return 659;
        case 8801: return 660;
        case 8804: return 661;
        case 8805: return 662;
        case 8857: return 663;
        case 8960: return 664;
        case 8962: return 665;
        case 8976: return 666;
        case 8992: return 667;
        case 8993: return 668;
        case 9121: return 669;
        case 9122: return 670;
        case 9123: return 671;
        case 9124: return 672;
        case 9125: return 673;
        case 9126: return 674;
        case 9472: return 675;
        case 9473: return 676;
        case 9474: return 677;
        case 9475: return 678;
        case 9484: return 679;
        case 9485: return 680;
        case 9486: return 681;
        case 9487: return 682;
        case 9488: return 683;
        case 9489: return 684;
        case 9490: return 685;
        case 9491: return 686;
        case 9492: return 687;
        case 9493: return 688;
        case 9494: return 689;
        case 9495: return 690;
        case 9496: return 691;
        case 9497: return 692;
        case 9498: return 693;
        case 9499: return 694;
        case 9500: return 695;
        case 9501: return 696;
        case 9502: return 697;
        case 9503: return 698;
        case 9504: return 699;
        case 9505: return 700;
        case 9506: return 701;
        case 9507: return 702;
        case 9508: return 703;
        case 9509: return 704;
        case 9510: return 705;
        case 9511: return 706;
        case 9512: return 707;
        case 9513: return 708;
        case 9514: return 709;
        case 9515: return 710;
        case 9516: return 711;
        case 9517: return 712;
        case 9518: return 713;
        case 9519: return 714;
        case 9520: return 715;
        case 9521: return 716;
        case 9522: return 717;
        case 9523: return 718;
        case 9524: return 719;
        case 9525: return 720;
        case 9526: return 721;
        case 9527: return 722;
        case 9528: return 723;
        case 9529: return 724;
        case 9530: return 725;
        case 9531: return 726;
        case 9532: return 727;
        case 9533: return 728;
        case 9534: return 729;
        case 9535: return 730;
        case 9536: return 731;
        case 9537: return 732;
        case 9538: return 733;
        case 9539: return 734;
        case 9540: return 735;
        case 9541: return 736;
        case 9542: return 737;
        case 9543: return 738;
        case 9544: return 739;
        case 9545: return 740;
        case 9546: return 741;
        case 9547: return 742;
        case 9552: return 743;
        case 9553: return 744;
        case 9554: return 745;
        case 9555: return 746;
        case 9556: return 747;
        case 9557: return 748;
        case 9558: return 749;
        case 9559: return 750;
        case 9560: return 751;
        case 9561: return 752;
        case 9562: return 753;
        case 9563: return 754;
        case 9564: return 755;
        case 9565: return 756;
        case 9566: return 757;
        case 9567: return 758;
        case 9568: return 759;
        case 9569: return 760;
        case 9570: return 761;
        case 9571: return 762;
        case 9572: return 763;
        case 9573: return 764;
        case 9574: return 765;
        case 9575: return 766;
        case 9576: return 767;
        case 9577: return 768;
        case 9578: return 769;
        case 9579: return 770;
        case 9580: return 771;
        case 9581: return 772;
        case 9582: return 773;
        case 9583: return 774;
        case 9584: return 775;
        case 9585: return 776;
        case 9586: return 777;
        case 9587: return 778;
        case 9588: return 779;
        case 9589: return 780;
        case 9590: return 781;
        case 9591: return 782;
        case 9592: return 783;
        case 9593: return 784;
        case 9594: return 785;
        case 9595: return 786;
        case 9596: return 787;
        case 9597: return 788;
        case 9598: return 789;
        case 9599: return 790;
        case 9600: return 791;
        case 9601: return 792;
        case 9602: return 793;
        case 9603: return 794;
        case 9604: return 795;
        case 9605: return 796;
        case 9606: return 797;
        case 9607: return 798;
        case 9608: return 799;
        case 9609: return 800;
        case 9610: return 801;
        case 9611: return 802;
        case 9612: return 803;
        case 9613: return 804;
        case 9614: return 805;
        case 9615: return 806;
        case 9616: return 807;
        case 9617: return 808;
        case 9618: return 809;
        case 9619: return 810;
        case 9620: return 811;
        case 9621: return 812;
        case 9622: return 813;
        case 9623: return 814;
        case 9624: return 815;
        case 9625: return 816;
        case 9626: return 817;
        case 9627: return 818;
        case 9628: return 819;
        case 9629: return 820;
        case 9630: return 821;
        case 9631: return 822;
        case 9632: return 823;
        case 9633: return 824;
        case 9634: return 825;
        case 9635: return 826;
        case 9636: return 827;
        case 9637: return 828;
        case 9638: return 829;
        case 9639: return 830;
        case 9640: return 831;
        case 9641: return 832;
        case 9642: return 833;
        case 9643: return 834;
        case 9644: return 835;
        case 9645: return 836;
        case 9646: return 837;
        case 9647: return 838;
        case 9648: return 839;
        case 9649: return 840;
        case 9650: return 841;
        case 9651: return 842;
        case 9652: return 843;
        case 9653: return 844;
        case 9658: return 845;
        case 9660: return 846;
        case 9668: return 847;
        case 9670: return 848;
        case 9674: return 849;
        case 9675: return 850;
        case 9679: return 851;
        case 9688: return 852;
        case 9689: return 853;
        case 9698: return 854;
        case 9699: return 855;
        case 9700: return 856;
        case 9701: return 857;
        case 9702: return 858;
        case 9744: return 859;
        case 9786: return 860;
        case 9787: return 861;
        case 9788: return 862;
        case 9792: return 863;
        case 9794: return 864;
        case 9824: return 865;
        case 9827: return 866;
        case 9829: return 867;
        case 9830: return 868;
        case 9834: return 869;
        case 9835: return 870;
        case 10003: return 871;
        case 64257: return 872;
        case 64258: return 873;
        case 65533: return 874;
        default: return 63;
    }
}
