#include "movingwalls.hpp"
#include <iomanip>

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
            if (loop || (min(front, back) >= 0 && max(front, back) < points.size())) {
                front = cMod(front, points.size());
                back = cMod(back, points.size());
                if (!world -> isSolid(points[front].x, points[front].y)) {
                    world -> setSolid(points[front].x, points[front].y, 's');
                    world -> setSolid(points[back].x, points[back].y, '.');
                    snakeHead = cMod(snakeHead + oldDirection, points.size());
                }
            }
        }
        //If snakeWall should push objects out of the way
/*        if (newDirection != 0 && (tickCounter + 1) % ticksPerMove == 0) {
            int front;
            if (newDirection == 1) {
                front = cMod(snakeHead + 1, points.size());
            }
            else { //if (direction == -1)
                front = cMod(snakeHead - snakeLength, points.size());
            }
            int previous = cMod(front - newDirection, points.size());
            if (front == previous + 1 || front == previous - 1) {
                movingRectangle newRect(points[previous].x, points[previous].y + 0.0001, points[previous].x + 1, points[previous].y + 1, points[front].x, points[front].y + 0.0001, points[front].x + 1, points[front].y + 1, id);
                world -> addRectangle(newRect);
            }
        }*/
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
            theScreen -> draw(points[cMod(i, points.size())].x, points[cMod(i, points.size())].y, tint, scale, display, doLighting, doHighlight);
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
                string oneDisplay = utf8(newDisplay);
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
                theScreen -> draw(x, y + i, tint, scale, display, doLighting, doHighlight);
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
        movingRectangle newRect(x, y, x + width, y + height, x + move.x, y + move.y, x + width + move.x, y + height + move.y, id);
        world -> addRectangle(newRect);
    }

    void elevator::tickGet() {
        x += move.x;
        y += move.y;
        gameLayer::tickGet();
    }

    bool elevator::finalize() {
        return false;
    }

    void elevator::print() {
        gameLayer::print();
    }

/*****************************************************************************/
//physicalBlock
//A solid block which has physical attributes (gravity, momentum)
/*****************************************************************************/

    physicalBlock::physicalBlock(float newX, float newY, Color newTint, float newScale, float newWidth, float newHeight, float newElasticity, float newXMomentum, float newYMomentum, float newMaxSpeed, float newGravity, float newFriction, gameLayer& newLayer) :
        entity(newX, newY, newTint, newScale),
        physicalEntity(newX, newY, newTint, newScale, newWidth, newHeight, newElasticity, newXMomentum, newYMomentum, newMaxSpeed, newGravity, newFriction),
        gameLayer(newLayer) {
        oldX = old2X = x;
        oldY = old2Y = y;
    }

    unsigned int physicalBlock::type() {
        return ELEVATORTYPE;
    }

    void physicalBlock::tickSet() {
        movingRectangle newRect(old2X, old2Y, old2X + width, old2Y + height, oldX, oldY, oldX + width, oldY + height, entity::id);
        world -> addRectangle(newRect);
        old2X = oldX;
        old2Y = oldY;
        oldX = x;
        oldY = y;
    }

    void physicalBlock::tickGet() {
        physicalEntity::tickGet();
        gameLayer::tickGet();
        if (hitY && abs(yMomentum) < 0.05) {
            yMomentum = 0;
        }
        collisions.clear();
    }

    bool physicalBlock::stopColliding() {
        return finalize();
    }

    bool physicalBlock::finalize() {
        return y > world -> getRows() + 100;
    }

    void physicalBlock::print() {
        float newX = x;
        float newY = y;
        x = oldX;
        y = oldY;
        gameLayer::print();
        x = newX;
        y = newY;
    }
