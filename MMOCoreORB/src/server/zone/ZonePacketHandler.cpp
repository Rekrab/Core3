/*
Copyright (C) 2007 <SWGEmu>

This File is part of Core3.

This program is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software
Foundation; either version 2 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General
Public License along with this program; if not, write to
the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Linking Engine3 statically or dynamically with other modules
is making a combined work based on Engine3.
Thus, the terms and conditions of the GNU Lesser General Public License
cover the whole combination.

In addition, as a special exception, the copyright holders of Engine3
give you permission to combine Engine3 program with free software
programs or libraries that are released under the GNU LGPL and with
code included in the standard release of Core3 under the GNU LGPL
license (or modified versions of such code, with unchanged license).
You may copy and distribute such a system following the terms of the
GNU LGPL for Engine3 and the licenses of the other code concerned,
provided that you include the source code of that other code when
and as the GNU LGPL requires distribution of source code.

Note that people who make modified versions of Engine3 are not obligated
to grant this special exception for their modified versions;
it is their choice whether to do so. The GNU Lesser General Public License
gives permission to release a modified version without this exception;
this exception also makes it possible to release a modified version
which carries forward this exception.
*/

#include "ZonePacketHandler.h"

#include "ZoneServer.h"
#include "ZoneClientSession.h"
#include "ZoneProcessServerImplementation.h"

#include "packets/zone/ClientIDMessage.h"
#include "packets/zone/SelectCharacter.h"
#include "packets/zone/CmdSceneReady.h"

#include "packets/charcreation/ClientCreateCharacter.h"
#include "packets/charcreation/ClientRandomNameRequest.h"

#include "packets/object/ObjectControllerMessage.h"
#include "packets/object/DataTransform.h"
#include "packets/object/DataTransformWithParent.h"
#include "packets/object/CommandQueueEnqueue.h"
#include "packets/object/ObjectMenuRequest.h"
#include "packets/object/TargetUpdate.h"


#include "packets/chat/ChatRoomList.h"

ZonePacketHandler::ZonePacketHandler(const String& s, ZoneProcessServerImplementation* serv) : Logger(s) {
	processServer = serv;

	server = processServer->getZoneServer();

	setGlobalLogging("true");
	setLogging("true");

	registerMessages();
	registerObjectControllerMessages();
}

void ZonePacketHandler::registerMessages() {
	messageCallbackFactory.registerObject<ClientIDMessageCallback>(0xD5899226);
	messageCallbackFactory.registerObject<ClientCreateCharacterCallback>(0xB97F3074);
	messageCallbackFactory.registerObject<ClientRandomNameRequest>(0xD6D1B6D1);
	messageCallbackFactory.registerObject<SelectCharacterCallback>(0xB5098D76);
	messageCallbackFactory.registerObject<CmdSceneReadyCallback>(0x43FD1C22);
	messageCallbackFactory.registerObject<ObjectControllerMessageCallback>(0x80CE5E46);
	messageCallbackFactory.registerObject<ChatRequestRoomListCallback>(0x4C3D2CFA);
}

void ZonePacketHandler::registerObjectControllerMessages() {
	ObjectControllerMessageCallback::objectMessageControllerFactory = new ObjectFactory<MessageCallback* (ObjectControllerMessageCallback*), uint32>();
	ObjectFactory<MessageCallback* (ObjectControllerMessageCallback*), uint32>* objectMessageControllerFactory = ObjectControllerMessageCallback::objectMessageControllerFactory;

	objectMessageControllerFactory->registerObject<DataTransformCallback>(0x71);
	objectMessageControllerFactory->registerObject<DataTransformWithParentCallback>(0xF1);
	objectMessageControllerFactory->registerObject<CommandQueueEnqueueCallback>(0x116);
	objectMessageControllerFactory->registerObject<TargetUpdateCallback>(0x126);
	objectMessageControllerFactory->registerObject<ObjectMenuRequestCallback>(0x146);
}

void ZonePacketHandler::handleMessage(Message* pack) {
	//info("parsing " + pack->toString());

	uint16 opcount = pack->parseShort();
	uint32 opcode = pack->parseInt();

	StringBuffer buffer;
	buffer << "handleMessage: opcount: " << hex << opcount << dec << " opcode: " << hex << opcode << endl;
	info(buffer);

	ZoneClientSessionImplementation* clientimpl = (ZoneClientSessionImplementation*) pack->getClient();
	ZoneClientSession* client = (ZoneClientSession*) clientimpl->_getStub();

	MessageCallback* messageCallback = messageCallbackFactory.createObject(opcode, client, processServer);

	if (messageCallback == NULL) {
		StringBuffer msg;
		msg << "unknown opcode 0x" << hex << opcode;
		info(msg);

		return;
	}

	if (parseMessage(pack, messageCallback))
		runMessage(messageCallback);

	delete messageCallback;
}

bool ZonePacketHandler::parseMessage(Message* pack, MessageCallback* messageCallback) {
	try {

		messageCallback->parse(pack);

	} catch (Exception& e) {
		error("exception while parsing message in ZonePacketHandler");
		error(e.getMessage());
		e.printStackTrace();

		return false;
	} catch (...) {
		error("unknown exception while parsing message in ZonePacketHandler");

		return false;
	}

	return true;
}

void ZonePacketHandler::runMessage(MessageCallback* messageCallback) {
	try {

		messageCallback->run();

	} catch (Exception& e) {
		error("exception while executing message callback in ZonePacketHandler");
		error(e.getMessage());
		e.printStackTrace();
	} catch (...) {
		error("unknown exception while executing message callback in ZonePacketHandler");
	}
}

