//
// Created by okker on 4/8/20.
//

#ifndef PROJ_ANDROID_B2WORLDNODE_H
#define PROJ_ANDROID_B2WORLDNODE_H

#include "cocos2d.h"
#include "Box2dDebugDraw.h"
#include "Box2D/Box2D.h"
#include "b2dSprite.h"


class b2WorldNode : public cocos2d::Node {

public:
    b2WorldNode();
    ~b2WorldNode();
    bool init() override ;
    bool init(float gravityX, float gravityY, float metersHeight);

    static b2WorldNode* create();
    static b2WorldNode* create(float metersHeight);
    static b2WorldNode* create(float gravityX, float gravityY);
    static b2WorldNode* create(float gravityX, float gravityY, float metersHeight);

    // Children
    using Node::addChild;
    void addChild(Node* child, int localZOrder, const std::string &name) override;

    // Get, Set
    b2World* getb2World();
    float& getPTM();

    //delta functions
    void update(float dt) override;

//#if (CC_BOX2D_DEBUG)
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags) override;
//#endif

private:
    bool addChildBody(Node* child);

    void addDynamicChild(b2Body* child);

    void removeDynamicChild(b2Body* child);

    std::vector<b2Body*> _dynamicChild; // store dynamic and kinematic bodies

    b2World* _world;
    float _PTM_Ratio;

    int32 _velocityIterations; // how strongly to correct velocity
    int32 _positionIterations; // how strongly to correct position


//#if (CC_BOX2D_DEBUG)
    //    std::unique_ptr<CcxBox2dDebugDraw> _debugDrawInst = nullptr;
    CcxBox2dDebugDraw* _debugDrawInst;
//#endif

};


#endif //PROJ_ANDROID_B2WORLDNODE_H
