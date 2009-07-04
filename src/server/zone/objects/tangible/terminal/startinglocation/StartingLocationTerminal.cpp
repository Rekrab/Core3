/*
 *	server/zone/objects/tangible/terminal/startinglocation/StartingLocationTerminal.cpp generated by engine3 IDL compiler 0.55
 */

#include "StartingLocationTerminal.h"

#include "StartingLocationTerminalImplementation.h"

#include "../../../player/Player.h"

#include "../Terminal.h"

/*
 *	StartingLocationTerminalStub
 */

StartingLocationTerminal::StartingLocationTerminal(unsigned long long objid, float x, float z, float y) : Terminal(DummyConstructorParameter::instance()) {
	_impl = new StartingLocationTerminalImplementation(objid, x, z, y);
	_impl->_setStub(this);
}

StartingLocationTerminal::StartingLocationTerminal(DummyConstructorParameter* param) : Terminal(param) {
}

StartingLocationTerminal::~StartingLocationTerminal() {
}

int StartingLocationTerminal::useObject(Player* player) {
	if (_impl == NULL) {
		if (!deployed)
			throw ObjectNotDeployedException(this);

		DistributedMethod method(this, 6);
		method.addObjectParameter(player);

		return method.executeWithSignedIntReturn();
	} else
		return ((StartingLocationTerminalImplementation*) _impl)->useObject(player);
}

/*
 *	StartingLocationTerminalAdapter
 */

StartingLocationTerminalAdapter::StartingLocationTerminalAdapter(StartingLocationTerminalImplementation* obj) : TerminalAdapter(obj) {
}

Packet* StartingLocationTerminalAdapter::invokeMethod(uint32 methid, DistributedMethod* inv) {
	Packet* resp = new MethodReturnMessage(0);

	switch (methid) {
	case 6:
		resp->insertSignedInt(useObject((Player*) inv->getObjectParameter()));
		break;
	default:
		return NULL;
	}

	return resp;
}

int StartingLocationTerminalAdapter::useObject(Player* player) {
	return ((StartingLocationTerminalImplementation*) impl)->useObject(player);
}

/*
 *	StartingLocationTerminalHelper
 */

StartingLocationTerminalHelper* StartingLocationTerminalHelper::staticInitializer = StartingLocationTerminalHelper::instance();

StartingLocationTerminalHelper::StartingLocationTerminalHelper() {
	className = "StartingLocationTerminal";

	DistributedObjectBroker::instance()->registerClass(className, this);
}

void StartingLocationTerminalHelper::finalizeHelper() {
	StartingLocationTerminalHelper::finalize();
}

DistributedObject* StartingLocationTerminalHelper::instantiateObject() {
	return new StartingLocationTerminal(DummyConstructorParameter::instance());
}

DistributedObjectAdapter* StartingLocationTerminalHelper::createAdapter(DistributedObjectStub* obj) {
	DistributedObjectAdapter* adapter = new StartingLocationTerminalAdapter((StartingLocationTerminalImplementation*) obj->_getImplementation());

	obj->_setClassName(className);
	obj->_setClassHelper(this);

	adapter->setStub(obj);

	return adapter;
}

/*
 *	StartingLocationTerminalServant
 */

StartingLocationTerminalServant::StartingLocationTerminalServant(unsigned int objCRC, unsigned long long objid, const UnicodeString& n, const String& tempn, float x, float z, float y, unsigned char terminaltype) : TerminalImplementation(objCRC, objid, n, tempn, x, z, y, terminaltype) {
	_classHelper = StartingLocationTerminalHelper::instance();
}

StartingLocationTerminalServant::~StartingLocationTerminalServant() {
}

void StartingLocationTerminalServant::_setStub(DistributedObjectStub* stub) {
	_this = (StartingLocationTerminal*) stub;
	TerminalServant::_setStub(stub);
}

DistributedObjectStub* StartingLocationTerminalServant::_getStub() {
	return _this;
}

