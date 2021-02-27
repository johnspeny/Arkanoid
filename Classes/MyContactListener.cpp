//
// Created by okker on 2/27/2021.
//

#include "MyContactListener.h"

MyContactListener::MyContactListener() {

}

MyContactListener::~MyContactListener() {

}

void MyContactListener::BeginContact(b2Contact *contact) {
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    _contacts.push_back(myContact);
}

void MyContactListener::EndContact(b2Contact *contact) {
    MyContact myContact = { contact->GetFixtureA(), contact->GetFixtureB() };
    std::vector<MyContact>::iterator pos;
    pos = std::find(_contacts.begin(), _contacts.end(), myContact);
    if (pos != _contacts.end()) {
        _contacts.erase(pos);
    }
}

void MyContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold) {
}

void MyContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) {
}

