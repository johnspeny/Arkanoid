//
// Created by okker on 4/8/20.
//

#ifndef PROJ_ANDROID_B2DSPRITE_H
#define PROJ_ANDROID_B2DSPRITE_H

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "b2WorldNode.h"

USING_NS_CC;

class b2WorldNode;

class b2dSprite : public cocos2d::Sprite {
public:
    // INSTANTIATION
    b2dSprite();
    ~b2dSprite();

    bool init() override;
    bool initBody();
    bool initBody(b2BodyType type);
    bool initBody(float32 friction, float32 restitution);
    bool initBody(b2BodyType type, float32 friction, float32 restitution);

    static b2dSprite* create();
    static b2dSprite* create(const std::string& filename);
    static b2dSprite* create(const std::string& filename, b2BodyType type);
    static b2dSprite* create(const std::string& filename, b2BodyType type, float32 friction, float32 restitution);
    static b2dSprite* create(const std::string& filename, const Rect& rect);
    static b2dSprite* create(const std::string& filename, const Rect& rect, b2BodyType type);
    static b2dSprite* create(const std::string& filename, const Rect& rect, b2BodyType type, float32 friction, float32 restitution);

    // SET/ GET
    virtual void setPosition(const Vec2& pos) override;
    virtual void setPosition(float x, float y) override;
    void setB2BodyPosition(float x, float y);

    virtual void setRotation(float rotation) override;
    void setB2BodyRotation(float rotation);

    void setWorldNode(b2WorldNode* world);
    b2WorldNode* getWorldNode();

    virtual void setBody(b2Body* body, b2WorldNode* world);
    b2Body* getBody();
    b2BodyDef* getBodyDef();

protected:
    b2Body* _B2Body;
    b2BodyDef _B2BodyDef;
    b2Fixture* _B2Fixture;
    b2FixtureDef _B2FixtureDef;
    b2PolygonShape _B2Shape;

private:
    b2WorldNode* _parentWorldNode;
};


#endif //PROJ_ANDROID_B2DSPRITE_H
