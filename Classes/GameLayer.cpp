//
// Created by okker on 12/21/2020.
//


#include "GameLayer.h"

USING_NS_CC;

Scene* GameLayer::createScene() {
    auto scene = Scene::create();
    auto layer = GameLayer::create();
    scene->addChild(layer);
    return scene;
}

bool GameLayer::init() {
    if (!Scene::init()){
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    log("origin ======== %f", origin.x);

    //Director instance
    Director::getInstance()->setDisplayStats(true);

    // game vars
    GameVars::initVars();


    // background
//    scrollingBgLayer = new ScrollingBgLayer(5.0f);
//    this->addChild(scrollingBgLayer, -1);


    // Initialize the physics engine
    this->initPhysics();
    // ball
    this->addNewSpriteAtPosition(Vec2(origin.x + visibleSize.width * 0.5f, visibleSize.height * 0.25f));
    // paddle
    this->addPaddleAtPosition(Vec2(origin.x + visibleSize.width * 0.5f, visibleSize.height * 0.2f));

    // paddle touch listeners
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameLayer::onTouchEnded, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // Create contact listener
    _contactListener = new MyContactListener();
    _world->getb2World()->SetContactListener(_contactListener);

    // update
    scheduleUpdate();

    return true;
}


void GameLayer::update(float dt) {
    _world->getb2World()->Step(dt, 10, 10);
    for (b2Body* b = _world->getb2World()->GetBodyList(); b; b = b->GetNext())
    {
        if (b->GetUserData() != nullptr) {
            Sprite* sprite = (Sprite*)b->GetUserData();


            // if ball is going too fast, turn on damping
            if (sprite->getTag() == 1) {
                static int maxSpeed = 10;

                b2Vec2 velocity = b->GetLinearVelocity();
                float32 speed = velocity.Length();

                if (speed > maxSpeed) {
                    b->SetLinearDamping(0.5);
                } else if (speed < maxSpeed) {
                    b->SetLinearDamping(0.0);
                }

            }

            sprite->setPosition( Vec2( b->GetPosition().x *
                                       GameVars::PTM_Ratio, b->GetPosition().y * GameVars::PTM_Ratio) );
            sprite->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
        }
    }


    //check contacts
    std::vector<b2Body *>toDestroy;
    std::vector<MyContact>::iterator pos;
    for (pos=_contactListener->_contacts.begin();
         pos != _contactListener->_contacts.end(); ++pos) {
        MyContact contact = *pos;

        if ((contact.fixtureA == _bottomFixture && contact.fixtureB == _ballFixture) ||
            (contact.fixtureA == _ballFixture && contact.fixtureB == _bottomFixture)) {
            log("Ball hit bottom!");
        }

        b2Body *bodyA = contact.fixtureA->GetBody();
        b2Body *bodyB = contact.fixtureB->GetBody();
        if (bodyA->GetUserData() != nullptr && bodyB->GetUserData() != nullptr) {
            Sprite *spriteA = (Sprite *) bodyA->GetUserData();
            Sprite *spriteB = (Sprite *) bodyB->GetUserData();

            //Sprite A = ball, Sprite B = Block
            if (spriteA->getTag() == 1 && spriteB->getTag() == 2) {
                if (std::find(toDestroy.begin(), toDestroy.end(), bodyB) == toDestroy.end()) {
                    toDestroy.push_back(bodyB);
                }
            }

                //Sprite A = block, Sprite B = ball
            else if (spriteA->getTag() == 2 && spriteB->getTag() == 1) {
                if (std::find(toDestroy.begin(), toDestroy.end(), bodyA) == toDestroy.end()) {
                    toDestroy.push_back(bodyA);
                }
            }
        }
    }

    std::vector<b2Body *>::iterator pos2;
    for (pos2 = toDestroy.begin(); pos2 != toDestroy.end(); ++pos2) {
        b2Body *body = *pos2;
        if (body->GetUserData() != NULL) {
            Sprite *sprite = (Sprite *) body->GetUserData();
            this->removeChild(sprite);
//            [self removeChild:sprite cleanup:YES];
        }
        _world->getb2World()->DestroyBody(body);
    }


}

void GameLayer::initPhysics() {
    Size s = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // gravity parameters
    b2Vec2 gravity;
    gravity.Set(0.0f, 0.0f);

    //create the world
    _world = b2WorldNode::create(gravity.x, gravity.y, GameVars::metersHeight);

    // added world here
    this->addChild(_world, 0);

    // ground object definition
    b2BodyDef groundBodyDef;

    // left position of the screen
    groundBodyDef.position.Set(origin.x / GameVars::PTM_Ratio, 0.0f / GameVars::PTM_Ratio);

    // create a edge around the entire screen
    _groundBody = _world->getb2World()->CreateBody(&groundBodyDef);

    b2EdgeShape groundBox;
    b2FixtureDef groundBoxDef;
    groundBoxDef.shape = &groundBox;

//    groundBox.Set(b2Vec2(origin.x / GameVars::PTM_Ratio,0), b2Vec2(s.width/GameVars::PTM_Ratio, 0));
//    _bottomFixture = _groundBody->CreateFixture(&groundBoxDef);

    // left
    groundBox.Set(b2Vec2(origin.x / GameVars::PTM_Ratio,origin.y / GameVars::PTM_Ratio), b2Vec2(origin.x / GameVars::PTM_Ratio, origin.y / GameVars::PTM_Ratio + s.height/GameVars::PTM_Ratio));
    _groundBody->CreateFixture(&groundBoxDef);

    // right
    groundBox.Set(b2Vec2(origin.x / GameVars::PTM_Ratio + s.width/GameVars::PTM_Ratio, origin.y / GameVars::PTM_Ratio), b2Vec2(origin.x / GameVars::PTM_Ratio + s.width/GameVars::PTM_Ratio,
                                                              s.height/GameVars::PTM_Ratio + origin.y / GameVars::PTM_Ratio));
    _groundBody->CreateFixture(&groundBoxDef);

    // up
    groundBox.Set(b2Vec2(origin.x / GameVars::PTM_Ratio, origin.y / GameVars::PTM_Ratio + s.height/GameVars::PTM_Ratio),
                  b2Vec2(origin.x / GameVars::PTM_Ratio + s.width/GameVars::PTM_Ratio, origin.y / GameVars::PTM_Ratio + s.height/GameVars::PTM_Ratio));
    _groundBody->CreateFixture(&groundBoxDef);

    // down
    groundBox.Set(b2Vec2(origin.x / GameVars::PTM_Ratio, origin.y / GameVars::PTM_Ratio),
                  b2Vec2(origin.x / GameVars::PTM_Ratio + s.width/GameVars::PTM_Ratio, origin.y / GameVars::PTM_Ratio));
    _bottomFixture = _groundBody->CreateFixture(&groundBoxDef);


}

void GameLayer::addNewSpriteAtPosition(cocos2d::Vec2 p) {

    // Create a physics engine sprite object
    ball = Sprite::create("Ball.png");
    ball->setPosition( p.x, p.y );
    ball->setTag(1);
    this->addChild(ball);

    // create ball body
    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
    ballBodyDef.position.Set(p.x/GameVars::PTM_Ratio, p.y/GameVars::PTM_Ratio);
    ballBodyDef.userData = ball;

    b2Body * ballBody = _world->getb2World()->CreateBody(&ballBodyDef);

    // Create circle shape
    b2CircleShape circle;
    circle.m_radius = 15.0/GameVars::PTM_Ratio;

    // Create a shape definition and add to body
    b2FixtureDef ballShapeDef;
    ballShapeDef.shape =&circle;
    ballShapeDef.density = 1.0f;
    ballShapeDef.friction = 0.0f;
    ballShapeDef.restitution = 1.0f;
    _ballFixture = ballBody->CreateFixture(&ballShapeDef);

    // ball force
    b2Vec2 force = b2Vec2(10, 10);
    ballBody->ApplyLinearImpulse(force, ballBodyDef.position, true);
}

void GameLayer::addPaddleAtPosition(cocos2d::Vec2 p) {

    // Create a physics engine sprite object
    paddle = Sprite::create("Paddle.png");
    paddle->setPosition( p.x, p.y );
    // paddle->setTag(2);
    this->addChild(paddle);

    // create paddle body
    b2BodyDef paddleBodyDef;
    paddleBodyDef.type = b2_dynamicBody;
    paddleBodyDef.position.Set(p.x/GameVars::PTM_Ratio, p.y/GameVars::PTM_Ratio);
    paddleBodyDef.fixedRotation = true;
    paddleBodyDef.userData = paddle;

    _paddleBody = _world->getb2World()->CreateBody(&paddleBodyDef);

    // Create paddle shape
    b2PolygonShape paddleShape;
    paddleShape.SetAsBox(paddle->getContentSize().width/2/GameVars::PTM_Ratio,
                         paddle->getContentSize().height/2/GameVars::PTM_Ratio);

    // Create a shape definition and add to body
    b2FixtureDef paddleShapeDef;
    paddleShapeDef.shape = &paddleShape;
    paddleShapeDef.density = 10.0f;
    paddleShapeDef.friction = 0.4f;
    paddleShapeDef.restitution = 0.1f;
    _paddleFixture = _paddleBody->CreateFixture(&paddleShapeDef);

    // restrict movement of paddle
    b2PrismaticJointDef jointDef;
    b2Vec2 worldAxis(1.0f, 0.0f);
    jointDef.collideConnected = true;
    jointDef.Initialize(_paddleBody, _groundBody,
                        _paddleBody->GetWorldCenter(), worldAxis);
    _world->getb2World()->CreateJoint(&jointDef);

    // some blocks added
    for(int i = 0; i < 20; i++) {

        static int padding=20;

        // Create block and add it to the layer
        Sprite *block = Sprite::create("block.png");
        int xOffset = padding+block->getContentSize().width/2+
                      ((block->getContentSize().width+padding)*i);
        block->setPosition(Vec2(xOffset, 250));
        block->setTag(2);
        this->addChild(block);

        // Create block body
        b2BodyDef blockBodyDef;
        blockBodyDef.type = b2_dynamicBody;
        blockBodyDef.position.Set(xOffset/GameVars::PTM_Ratio, 250/GameVars::PTM_Ratio);
        blockBodyDef.userData = block;
        b2Body *blockBody = _world->getb2World()->CreateBody(&blockBodyDef);

        // Create block shape
        b2PolygonShape blockShape;
        blockShape.SetAsBox(block->getContentSize().width/GameVars::PTM_Ratio/2,
                            block->getContentSize().height/GameVars::PTM_Ratio/2);

        // Create shape definition and add to body
        b2FixtureDef blockShapeDef;
        blockShapeDef.shape = &blockShape;
        blockShapeDef.density = 10.0;
        blockShapeDef.friction = 0.0;
        blockShapeDef.restitution = 0.1f;
        blockBody->CreateFixture(&blockShapeDef);

    }


}

bool GameLayer::onTouchBegan(Touch *touch, Event *event) {
//    if (_mouseJoint != nullptr) return ;

    Point touchLocation = this->convertToWorldSpace(this->convertTouchToNodeSpace(touch));

    // b2 world location
    b2Vec2 loactionWorld = b2Vec2(touchLocation.x/GameVars::PTM_Ratio, touchLocation.y/GameVars::PTM_Ratio);

    if (_paddleFixture->TestPoint(loactionWorld)){
        b2MouseJointDef md;
        md.bodyA = _groundBody;
        md.bodyB = _paddleBody;
        md.target = loactionWorld;
        md.collideConnected = true;
        md.maxForce = 1000.0f * _paddleBody->GetMass();

        _mouseJoint = (b2MouseJoint *)_world->getb2World()->CreateJoint(&md);
        _paddleBody->SetAwake(true);
    }

    return true;
}

void GameLayer::onTouchMoved(Touch *touch, Event *event) {
    if (_mouseJoint == nullptr) return;
    Point touchLocation = this->convertToWorldSpace(this->convertTouchToNodeSpace(touch));

    // b2 world location
    b2Vec2 loactionWorld = b2Vec2(touchLocation.x/GameVars::PTM_Ratio, touchLocation.y/GameVars::PTM_Ratio);
    _mouseJoint->SetTarget(loactionWorld);
}

void GameLayer::onTouchEnded(Touch *touch, Event *event) {
    if (_mouseJoint) {
        _world->getb2World()->DestroyJoint(_mouseJoint);
        _mouseJoint = nullptr;
    }
}








