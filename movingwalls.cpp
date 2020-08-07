#include "movingwalls.hpp"

/*****************************************************************************/
//Snakewall
//A wall that moves like the snake in snake (but also diagonally)
/*****************************************************************************/

    snakeWall::snakeWall(  float newX, float newY, Color newTint, float newScale, int newSnakeLength, int newSnakeHead, int newTicksPerMove, bool newLoop, int newForwardChannel, int newReverseChannel, string newDisplay) :
        entity(newX, newY, newTint, newScale),
        loop(newLoop),
        forwardChannel(newForwardChannel),
        reverseChannel(newReverseChannel),
        snakeLength(newSnakeLength),
        snakeHead(newSnakeHead),
        ticksPerMove(newTicksPerMove),
        display(newDisplay) {}

    void snakeWall::addPoint(int pointX, int pointY) {
        if (points.size() > 0) {
            int lastX = points[points.size() - 1].x;
            int lastY = points[points.size() - 1].y;
            float interPointCount = max(abs(pointX - lastX), abs(pointY - lastY));
            float dX = (pointX - lastX) / interPointCount;
            float dY = (pointY - lastY) / interPointCount;
            for (int i = 1; i <= interPointCount; i++) {
                points.push_back(intVector2(lastX + dX * i, lastY + dY * i));
            }
        }
        else {
            points.push_back(intVector2(pointX, pointY));
        }
    }

    void snakeWall::finish() {
        snakeHead = snakeHead % points.size();
        if (snakeLength > points.size()) {
            cout << "WARNING: Snake has more segments than space!\n";
        }
        for (int i = snakeHead; i != (snakeHead - snakeLength + points.size()) % points.size(); i = (i - 1 + points.size()) % points.size()) {
            world -> setSolid(points[i].x, points[i].y, 's');
        }
    }

    unsigned int snakeWall::type() {
        return SNAKEWALLTYPE;
    }

    bool snakeWall::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        if ((tickCounter + 1) % ticksPerMove == 0 && newDirection != 0) {
            if (newDirection == 1) {
                pushPoint = cMod(snakeHead + 1, points.size());
            }
            else {
                pushPoint = cMod(snakeHead - snakeLength, points.size());
            }
            if (pushPoint - newDirection >= 0 && pushPoint - newDirection < points.size()) {
                directionPoint = pushPoint - newDirection;
                swapDirectionPoint = false;
            }
            else {
                directionPoint = pushPoint + newDirection;
                swapDirectionPoint = true;
            }
            Vector2 pushTile = points[pushPoint].getVector2();
            return (otherX >= pushTile.x - 0.8 && otherX <= pushTile.x + 0.8 && otherY >= pushTile.y - 0.8 && otherY <= pushTile.y + 0.8);
        }
        return false;
    }

    collision snakeWall::getCollision(float otherX, float otherY, int otherType, unsigned int otherID) {
        Vector2 push;
        if (swapDirectionPoint) {
            push = Vector2Subtract(points[directionPoint].getVector2(), points[pushPoint].getVector2());
        }
        else {
            push = Vector2Subtract(points[pushPoint].getVector2(), points[directionPoint].getVector2());
        }
        push = Vector2Scale(Vector2Normalize(push), (1 / float(ticksPerMove)));
        return collision(SNAKEWALLTYPE, id, 0, push.x, push.y);
    }

    bool snakeWall::stopColliding() {
        return false;
    }

    void snakeWall::tickSet() {
        if (oldDirection != 0 && tickCounter % ticksPerMove == 0) {
            int front, back;
            if (oldDirection == 1) {
                front = snakeHead + 1;
                back = snakeHead - snakeLength + 1;
            }
            else { //if (direction == -1)
                front = snakeHead - snakeLength;
                back = snakeHead;
            }
            if (!world -> isSolid(points[front].x, points[front].y) && (loop || (min(front, back) >= 0 && max(front, back) < points.size()))) {
                front = cMod(front, points.size());
                back = cMod(back, points.size());
                world -> setSolid(points[front].x, points[front].y, 's');
                world -> setSolid(points[back].x, points[back].y, '.');
                snakeHead = cMod(snakeHead + oldDirection, points.size());
            }
        }
    }

    void snakeWall::tickGet() {
        oldDirection = newDirection;
        if (world -> getChannel(forwardChannel)) {
            if (!world -> getChannel(reverseChannel)) {
                newDirection = 1;
            }
        }
        else if (world -> getChannel(reverseChannel)) {
            newDirection = -1;
        }
        else {
            newDirection = 0;
        }
    }

    bool snakeWall::finalize() {
        return false;
    }

    void snakeWall::print() {
        for (int i = snakeHead - snakeLength + 1; i <= snakeHead; i++) {
            theScreen -> draw(points[cMod(i, points.size())].x, points[cMod(i, points.size())].y, tint, scale, display, doLighting);
        }
    }

/*****************************************************************************/
//activeWall
//Appears only when triggered
/*****************************************************************************/

    activeWall::activeWall(  float newX, float newY, Color newTint, float newScale, int newWidth, int newHeight, int newChannel, int newDisplay) :
        entity(newX, newY, newTint, newScale),
        width(newWidth),
        height(newHeight),
        channel(newChannel) {
            if (newDisplay != 0) {
                string oneDisplay = TextToUtf8(&newDisplay, 1);
                for (int j = 0; j < width; j++) {
                    display += oneDisplay;
                }
            }
        }

    unsigned int activeWall::type() {
        return ACTIVEWALLTYPE;
    }

    void activeWall::tickSet() {
        if (active && !activated) {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    world -> setSolid(x + j, y + i, 's');
                }
            }
            activated = true;
        }
        if (!active && activated) {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    world -> setSolid(x + j, y + i, '.');
                }
            }
            activated = false;
        }
    }

    void activeWall::tickGet() {
        active = world -> getChannel(channel);
    }

    bool activeWall::finalize() {
        return false;
    }

    void activeWall::print() {
        if (activated && display != "") {
            for (int i = 0; i < height; i++) {
                theScreen -> draw(x, y + i, tint, scale, display, doLighting);
            }
        }
    }

/*****************************************************************************/
//elevator
//A block with fixed size and shape which can move in multiple directions
//Takes any entities on top of it with it.
/*****************************************************************************/

    elevator::elevator(float newX, float newY, Color newTint, float newScale, float newWidth, float newHeight, float newSpeed, bool newLoop, int newForwardChannel, int newReverseChannel, gameLayer& newLayer) :
        entity(newX, newY, newTint, newScale),
        gameLayer(newLayer),
        width(newWidth),
        height(newHeight),
        speed(newSpeed),
        loop(newLoop),
        forwardChannel(newForwardChannel),
        reverseChannel(newReverseChannel) {}

    void elevator::addPoint(Vector2 newPoint) {
        points.push_back(newPoint);
    }

    void elevator::finish(int startingPoint) {
        nextPoint = cMod(startingPoint, points.size());
        x = points[nextPoint].x;
        y = points[nextPoint].y;
    }

    unsigned int elevator::type() {
        return ELEVATORTYPE;
    }

    bool elevator::doesCollide(float otherX, float otherY, int otherType, unsigned int otherID) {
        return (otherX > x - 1 && otherX < x + width && otherY >= y - 1 && otherY < y + height);
    }

    collision elevator::getCollision(float otherX, float otherY, int otherType, unsigned int otherID) {
        return collision(type(), id, 0, move.x, move.y);
    }

    bool elevator::stopColliding() {
        return false;
    }

    void elevator::tickSet() {
        if (x == points[nextPoint].x && y == points[nextPoint].y) {
            if (world -> getChannel(forwardChannel) && (loop || nextPoint < points.size() - 1)) {
                nextPoint = cMod(nextPoint + 1, points.size());
                move = Vector2Scale(Vector2Normalize((Vector2){points[nextPoint].x - x, points[nextPoint].y - y}), speed);
            }
            else if (world -> getChannel(reverseChannel) && (loop || nextPoint > 0)) {
                nextPoint = cMod(nextPoint - 1, points.size());
                move = Vector2Scale(Vector2Normalize((Vector2){points[nextPoint].x - x, points[nextPoint].y - y}), speed);
            }
            else {
                move = (Vector2){0, 0};
            }
        }
        else if (abs(move.x) > abs(x - points[nextPoint].x) || abs(move.y) > abs(y - points[nextPoint].y)) {
            move = Vector2Subtract(points[nextPoint], (Vector2){x, y});
        }
        movingRectangle newRect;
        newRect.oldX1 = x;
        newRect.oldY1 = y;
        newRect.oldX2 = x + width;
        newRect.oldY2 = y + height;
        newRect.newX1 = newRect.oldX1 + move.x;
        newRect.newY1 = newRect.oldY1 + move.y;
        newRect.newX2 = newRect.oldX2 + move.x;
        newRect.newY2 = newRect.oldY2 + move.y;
        world -> addRectangle(newRect);
    }

    void elevator::tickGet() {
        x += move.x;
        y += move.y;
    }

    bool elevator::finalize() {
        return false;
    }

    void elevator::print() {
        gameLayer::print();
    }

