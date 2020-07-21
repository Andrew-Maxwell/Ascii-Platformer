#include "menu.hpp"

    bool menu::onAnyGamepad(bool (*gamepadProperty)(int, int), int button) {
        int padID = 0;
        while (IsGamepadAvailable(padID)) {
            if ((*gamepadProperty)(padID, button)) {
                return true;
            }
            padID++;
        }
        return false;
    }

    bool menu::axisOnAnyGamepad(int axis1, int invert1, int axis2, int invert2) {
        int padID = 0;
        while (IsGamepadAvailable(padID)) {
            if (GetGamepadAxisMovement(padID, axis1) * invert1 > 0.7) {
                return true;
            }
            if (GetGamepadAxisMovement(padID, axis2) * invert2 > 0.7) {
                return true;
            }
            padID++;
        }
        return false;
    }

    void menu::handleKey (int keyOne, int keyTwo, int button1, int axis1, int axis2, int invertAxes, input in, int& keyDown, int& select, int itemCount, int direction) {
        if (IsKeyPressed(keyOne) || IsKeyPressed(keyTwo) || onAnyGamepad(IsGamepadButtonPressed, button1) || (axisOnAnyGamepad(axis1, invertAxes, axis2, invertAxes) && keyDown == 0) || in.isPressed() || (keyDown > 12 && keyDown % 4 == 0)) {
            select = (itemCount + select + direction) % itemCount;
        }
        if (IsKeyDown(keyOne) || IsKeyDown(keyTwo) || onAnyGamepad(IsGamepadButtonPressed, button1) || axisOnAnyGamepad(axis1, invertAxes, axis2, invertAxes) || in.isDown()) {
            keyDown++;
        }
        else {
            keyDown = 0;
        }
    }

    bool menu::goBack() {
        return (!locked && tickNo < 0 && (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_DELETE) || onAnyGamepad(IsGamepadButtonPressed, GAMEPAD_BUTTON_MIDDLE_LEFT) || onAnyGamepad(IsGamepadButtonPressed, GAMEPAD_BUTTON_MIDDLE) || onAnyGamepad(IsGamepadButtonPressed, GAMEPAD_BUTTON_MIDDLE_RIGHT)));
    }

    void menu::init(int newXCount, int newYCount) {
        tickNo = 2;
        xCount = newXCount;
        yCount = newYCount;
        ySelect = 0;
        scroll = 0;
        pageRows = 2 * yCount + 1;
    }

    void menu::handleInput(bool doScroll) {
        inputMap in(-1);
        handleInput(in, doScroll);
    }

    void menu::handleInput(inputMap& in, bool doScroll) {
        if (!locked) {
            if (myGetKeyPressed() || getAnyGamepadInput()) {
                mouseMode = false;
            }
            else {
                Vector2 mouse = GetMousePosition();
                if (oldMouse.x != mouse.x || oldMouse.y != mouse.y || IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || GetMouseWheelMove()) {
                    mouseMode = true;
                }
                oldMouse = mouse;
            }
            if (!mouseMode) {
                handleKey (KEY_UP,    KEY_W, GAMEPAD_BUTTON_LEFT_FACE_UP, GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_RIGHT_Y, -1, in.up, upDown, ySelect, yCount, -1);
                handleKey (KEY_DOWN,  KEY_S, GAMEPAD_BUTTON_LEFT_FACE_DOWN, GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_RIGHT_Y, 1, in.down, downDown,  ySelect, yCount, 1);
                handleKey (KEY_LEFT,  KEY_A, GAMEPAD_BUTTON_LEFT_FACE_LEFT, GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_RIGHT_X, -1, in.left, leftDown,  xSelect, xCount, -1);
                handleKey (KEY_RIGHT, KEY_D, GAMEPAD_BUTTON_LEFT_FACE_RIGHT, GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_RIGHT_X, 1, in.right, rightDown, xSelect, xCount, 1);
            }
            tickNo--;
            if (doScroll) {
                scrollBar();
            }
            firstCallToReleased = true;
        }
    }

    bool menu::button(string text, int buttonCol, int buttonRow, string inBrackets, int x, int y) {
        if (y == -1) {
            y = 2 * (buttonRow + 1) - scroll;
        }
        if (y <= 0) {
            return false;
        }
        if (locked) {
            theScreen -> drawHud(x, y, UIFOREGROUND, text);
            if (inBrackets != "") {
                theScreen -> drawHudWithBrackets(x + text.size() + 1, y, inBrackets, false);
            }
            return false;
        }
        else {
            int hudFontSize = theScreen -> getHudFontSize();
            Vector2 mouse = GetMousePosition();
            bool toReturn = false;
            if (mouseMode) {
                if (mouse.x > x * hudFontSize && mouse.x < (x + text.size() + (inBrackets == "" ? 0 : 3 + inBrackets.size())) * hudFontSize && mouse.y > y * hudFontSize && mouse.y < (y + 1) * hudFontSize) {
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                        theScreen -> drawHudWithBackground(x, y, DARKFOREGROUND, DARKBACKGROUND, text);
                    }
                    else {
                        theScreen -> drawHudWithBackground(x, y, LIGHTFOREGROUND, LIGHTBACKGROUND, text);
                    }
                    if (inBrackets != "") {
                        theScreen -> drawHudWithBrackets(x + text.size() + 1, y, inBrackets, true);
                    }
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && firstCallToReleased) {
                        firstCallToReleased = false;
                        toReturn = true;
                    }
                }
                else {
                    theScreen -> drawHudWithBackground(x, y, UIFOREGROUND, UIBACKGROUND, text);
                    if (inBrackets != "") {
                        theScreen -> drawHudWithBrackets(x + text.size() + 1, y, inBrackets, false);
                    }
                }
            }
            else {
                if (xSelect == buttonCol && ySelect == buttonRow) {
                    if (IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_SPACE) || onAnyGamepad(IsGamepadButtonDown, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || onAnyGamepad(IsGamepadButtonDown, GAMEPAD_BUTTON_LEFT_TRIGGER_1) || onAnyGamepad(IsGamepadButtonDown, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
                        theScreen -> drawHudWithBackground(x, y, DARKFOREGROUND, DARKBACKGROUND, text);
                    }
                    else {
                        theScreen -> drawHudWithBackground(x, y, LIGHTFOREGROUND, LIGHTBACKGROUND, text);
                    }
                    if (inBrackets != "") {
                        theScreen -> drawHudWithBrackets(x + text.size() + 1, y, inBrackets, true);
                    }
                    if (tickNo < 0 && (IsKeyReleased(KEY_ENTER) || IsKeyReleased(KEY_SPACE) || onAnyGamepad(IsGamepadButtonReleased, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || onAnyGamepad(IsGamepadButtonReleased, GAMEPAD_BUTTON_LEFT_TRIGGER_1) || onAnyGamepad(IsGamepadButtonReleased, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))) {
                        firstCallToReleased = false;
                        toReturn = true;
                    }
                }
                else {
                    theScreen -> drawHudWithBackground(x, y, UIFOREGROUND, UIBACKGROUND, text);
                    if (inBrackets != "") {
                        theScreen -> drawHudWithBrackets(x + text.size() + 1, y, inBrackets, false);
                    }
                }
            }
            if (tickNo > 0) {
                return false;
            }
            return toReturn;
        }
    }

    void menu::drawTitle(string title) {
        theScreen -> drawHudBarRight(0, 0, UIFOREGROUND, theScreen -> getHudCols() + 1);
        theScreen -> drawHud((theScreen -> getHudCols() - title.size()) / 2, 0, LIGHTFOREGROUND, title);
    }

    void menu::scrollBar() {
        int fontSize = theScreen -> getHudFontSize();
        int screenRows = theScreen -> getHudRows() - 1;
        if (screenRows < pageRows) {
            float handleLength = (screenRows - 2) * (screenRows / float(pageRows));
            float handlePosition = 2 + scroll / float(pageRows) * (screenRows - 2);
            bool mouseOnBar, mouseOnHandle;
            if (!locked) {
                //Determine scroll value
                Vector2 newMouse = GetMousePosition();
                mouseOnBar = newMouse.x > fontSize * (theScreen -> getHudCols() - 1);
                mouseOnHandle = mouseOnBar && newMouse.y > handlePosition * fontSize && newMouse.y < (handlePosition + handleLength) * fontSize;
                if (mouseMode) {
                    scroll -= 2 * GetMouseWheelMove();
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (mouseOnHandle) {
                            scrollClicked = true;
                            scrollMouse = GetMousePosition();
                            oldScroll = scroll;     //Save where the handle was when we started
                        }
                        else if (mouseOnBar) {  //Jump to position clicked on bar
                            scroll = (((newMouse.y / float(fontSize)) - 2) / float(theScreen -> getHudRows() - 3)) * pageRows;
                            scrollMouse = GetMousePosition();
                            oldScroll = scroll;
                        }
                    }
                    //Move to new scroll relative to old scroll
                    else if (scrollClicked && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                        scroll = oldScroll + (newMouse.y - scrollMouse.y) / float(fontSize * theScreen -> getHudRows()) * pageRows;
                    }
                    else {
                        scrollClicked = false;
                    }
                    //While scrolling mouse mode, move keyboard mode selection to stay on-screen
                    if (2 * ySelect + 1 - scroll > screenRows) {
                        ySelect = (scroll - 1 + screenRows) / 2;
                    }
                    if (2 * ySelect + 3 - scroll < 0) {
                        ySelect = (scroll + 1) / 2;
                    }
                }
                //While in keyboard mode, scroll to keep up with keyboard mode selection.
                else {
                    if (2 * ySelect + 3 - scroll > screenRows) {
                        scroll = 2 * ySelect + 3 - screenRows;
                    }
                    if (2 * ySelect + 1 - scroll < 0) {
                        scroll = 2 * ySelect + 1;
                    }
                }
                scroll = min(scroll, pageRows - screenRows);
                scroll = max(0, scroll);
                if (scroll % 2 == 1) {
                    scroll--;
                }
            }
            else {
                mouseOnBar = mouseOnHandle = false;
            }

            //Draw bar background
            theScreen -> drawHud(theScreen -> getHudCols() - 1, 1, UIFOREGROUND, "\xE2\x95\xA5");
            for (int i = 2; i < theScreen -> getHudRows() - 1; i++) {
                theScreen -> drawHud(theScreen -> getHudCols() - 1, i, UIFOREGROUND,  "\xE2\x95\x91");
            }
            theScreen -> drawHud(theScreen -> getHudCols() - 1, theScreen -> getHudRows() - 1, UIFOREGROUND, "\xE2\x95\xA8");
            //Draw bar handle
            if (mouseMode && (scrollClicked || mouseOnHandle)) {
                if (scrollClicked) {
                    theScreen -> drawHudBarDown(theScreen -> getHudCols() - 1, handlePosition, DARKFOREGROUND, handleLength);
                }
                else if (mouseOnHandle) {
                    theScreen -> drawHudBarDown(theScreen -> getHudCols() - 1, handlePosition, LIGHTFOREGROUND, handleLength);
                }
            }
            else {
                theScreen -> drawHudBarDown(theScreen -> getHudCols() - 1, handlePosition, UIFOREGROUND, handleLength);
            }
        }
    }

/*****************************************************************************/
//Input box
//A window which can be instantiated and drawn on top of other menus
/*****************************************************************************/

    inputBox::inputBox() {
        init(2, 2);
    }

    void inputBox::handleInput() {
        if (!locked) {
            if (myGetKeyPressed() || getAnyGamepadInput()) {
                mouseMode = false;
            }
            else {
                Vector2 mouse = GetMousePosition();
                if (oldMouse.x != mouse.x || oldMouse.y != mouse.y || IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || GetMouseWheelMove()) {
                    mouseMode = true;
                }
                oldMouse = mouse;
            }
            input dummyIn;
            if (!mouseMode) {
                handleKey(KEY_UP,    0, GAMEPAD_BUTTON_LEFT_FACE_UP, GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_RIGHT_Y, -1, dummyIn, upDown, ySelect, yCount, -1);
                handleKey(KEY_DOWN,  0, GAMEPAD_BUTTON_LEFT_FACE_DOWN, GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_RIGHT_Y, 1, dummyIn, downDown,  ySelect, yCount, 1);
                handleKey(KEY_LEFT,  0, GAMEPAD_BUTTON_LEFT_FACE_LEFT, GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_RIGHT_X, -1, dummyIn, leftDown,  xSelect, xCount, -1);
                handleKey(KEY_RIGHT, 0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT, GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_RIGHT_X, 1, dummyIn, rightDown, xSelect, xCount, 1);
                handleKey(KEY_BACKSPACE, KEY_DELETE, -1, -1, -1, -1, dummyIn, delDown, del, 1000, 1);
            }
            tickNo--;
            scrollBar();
            firstCallToReleased = true;
        }
    }

    void inputBox::display(string title, string& input) {
        width = 48, height = 7, x = (theScreen -> getHudCols() - width) / 2, y = (theScreen -> getHudRows() - height) / 2;
        handleInput();
        char nextChar = GetKeyPressed();
        if (ySelect == 0) {
            while (nextChar && input.size() < width - 6) {
                if (('a' <= nextChar && nextChar <= 'z') || ('A' <= nextChar && nextChar <= 'Z') || ('0' <= nextChar && nextChar <= '9') || nextChar == '-' || nextChar == '_') {
                    input += nextChar;
                }
                else if (nextChar == ' ') {
                    input += '_';
                }
                nextChar = GetKeyPressed();
            }
        }
        if (del && input.size() > 0) {
            del = 0;
            input.pop_back();
        }
        theScreen -> drawHudBox(x, y, width, height, UIFOREGROUND);
        theScreen -> drawHud((theScreen -> getHudCols() - title.size()) / 2, y + 1, UIBACKGROUND, title);
        theScreen -> drawHudWithBrackets(x + 2, y + 3, input, (ySelect == 0));
    }

    bool inputBox::okay() {
        return button("Okay", 0, 1, "", x + width * 3 / 8 - 2, y + 5) || (tickNo < 0 && IsKeyReleased(KEY_ENTER));
    }

    bool inputBox::cancel() {
        return button("Cancel", 1, 1, "", x + width * 5 / 8 - 3, y + 5) || goBack();
    }
