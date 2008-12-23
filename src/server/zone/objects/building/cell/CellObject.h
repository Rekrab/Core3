/*
 *	server/zone/objects/building/cell/CellObject.h generated by engine3 IDL compiler 0.55
 */

#ifndef CELLOBJECT_H_
#define CELLOBJECT_H_

#include "engine/orb/DistributedObjectBroker.h"

class Zone;

#include "engine/service/Message.h"

class SceneObject;

class Player;

class BuildingObject;

#include "../../scene/SceneObject.h"

class CellObject : public SceneObject {
public:
	CellObject(unsigned long long oid, BuildingObject* buio);

	CellObject(unsigned long long oid, BuildingObject* buio, int number);

	int getChildrenSize();

	void addChild(SceneObject* object, bool doLock = true);

	void removeChild(SceneObject* object, bool doLock = true);

	SceneObject* getChild(int idx);

	void setCellNumber(int i);

	int getCellNumber();

	void setAttributes(string& attributestring);

	string& getAttributes();

	void parseItemAttributes();

	string& getTemplateName();

protected:
	CellObject(DummyConstructorParameter* param);

	virtual ~CellObject();

	string _return_getAttributes;
	string _return_getTemplateName;

	friend class CellObjectHelper;
};

class CellObjectImplementation;

class CellObjectAdapter : public SceneObjectAdapter {
public:
	CellObjectAdapter(CellObjectImplementation* impl);

	Packet* invokeMethod(sys::uint32 methid, DistributedMethod* method);

	int getChildrenSize();

	void addChild(SceneObject* object, bool doLock);

	void removeChild(SceneObject* object, bool doLock);

	SceneObject* getChild(int idx);

	void setCellNumber(int i);

	int getCellNumber();

	void setAttributes(string& attributestring);

	string& getAttributes();

	void parseItemAttributes();

	string& getTemplateName();

protected:
	string _param0_setAttributes__string_;
};

class CellObjectHelper : public DistributedObjectClassHelper, public Singleton<CellObjectHelper> {
	static CellObjectHelper* staticInitializer;

public:
	CellObjectHelper();

	void finalizeHelper();

	DistributedObject* instantiateObject();

	DistributedObjectAdapter* createAdapter(DistributedObjectStub* obj);

	friend class SingletonWrapper<CellObjectHelper>;
};

#include "../../scene/SceneObjectImplementation.h"

class CellObjectServant : public SceneObjectImplementation {
public:
	CellObject* _this;

public:
	CellObjectServant(unsigned long long oid, int type);
	virtual ~CellObjectServant();

	void _setStub(DistributedObjectStub* stub);
	DistributedObjectStub* _getStub();

};

#endif /*CELLOBJECT_H_*/
