//
// Created by okker on 12/21/2020.
//

#ifndef PROJ_ANDROID_GAMELAYER_H
#define PROJ_ANDROID_GAMELAYER_H


#include "ScrollingBgLayer.h"
#include "cocos2d.h"
#include "b2WorldNode.h"
#include "GameVars.h"
#include "b2dSprite.h"
#include "MyContactListener.h"


class GameLayer: public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameLayer);

    b2WorldNode* _world;


    // init Physics
    void initPhysics();
    void addNewSpriteAtPosition(cocos2d::Vec2 p);
    void addPaddleAtPosition(cocos2d::Vec2 p);

    // ball and wall
    Sprite* ball;
    b2Body* _groundBody;
    b2Fixture *_bottomFixture;
    b2Fixture *_ballFixture;

    // paddle
    Sprite* paddle;
    b2Body* _paddleBody;
    b2Fixture* _paddleFixture;

    // mouse
    b2MouseJoint *_mouseJoint;

    // touch paddle to control
    void onTouchMoved(Touch *touch, Event *event);
    void onTouchEnded(Touch *touch, Event *event);
    bool onTouchBegan(Touch *touch, Event *event);

    void update(float dt);

private:
    // Add inside @interface
    MyContactListener *_contactListener;


};


#endif //PROJ_ANDROID_GAMELAYER_H
