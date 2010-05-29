/*
 Copyright (C) 2010 <SWGEmu>

 This File is part of Core3.

 This program is free software; you can redistribute
 it and/or modify it under the terms of the GNU Lesser
 General Public License as published by the Free Software
 Foundation; either version 3 of the License,
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

#include "ResourceSpawn.h"
#include "../terrain/PlanetNames.h"
#include "server/zone/ZoneProcessServerImplementation.h"
#include "server/zone/Zone.h"
#include "server/zone/objects/resource/ResourceContainer.h"
#include "server/zone/managers/object/ObjectManager.h"

void ResourceSpawnImplementation::fillAttributeList(AttributeListMessage* alm,
		PlayerCreature* object) {

		alm->insertAttribute("resource_class", getFinalClass());

		for (int i = 0; i < spawnAttributes.size(); ++i) {
			String attrib;
			int value = getAttributeAndValue(attrib, i);
			alm->insertAttribute(attrib, value);
		}

		alm->insertInt(0);
}

bool ResourceSpawnImplementation::inShift() {
	return despawned > time(0);
}

int ResourceSpawnImplementation::getAttributeAndValue(String& attribute,
		int index) {
	if (index < spawnAttributes.size()) {
		attribute = spawnAttributes.elementAt(index).getKey();
		return spawnAttributes.get(index);
	} else {
		return 0;
	}
}

bool ResourceSpawnImplementation::isUnknownType() {
	for (int i = 0; i < stfSpawnClasses.size(); ++i) {
		if (stfSpawnClasses.get(i).indexOf("unknown") != -1)
			return true;
	}
	return false;
}

String ResourceSpawnImplementation::getFamilyName() {
   	int offset = 2;

   	if(isUnknownType())
   		offset = 1;

   	if(spawnClasses.size() > offset)
   		return spawnClasses.get(spawnClasses.size() - offset);
   	else
   		return "";
}

void ResourceSpawnImplementation::createSpawnMaps(bool jtl,
		int zonerestriction, Vector<uint32>& activeZones) {

	int concentration = getConcentration(jtl);
	Vector<uint32> zoneids = getSpawnZones(jtl, zonerestriction, activeZones);

	for (int i = 0; i < zoneids.size(); ++i) {

		Zone* zone = server->getZoneServer()->getZone(zoneids.get(i));

		SpawnDensityMap newMap(isType("ore"), concentration, zone->getMinX(),
				zone->getMaxX(), zone->getMinY(), zone->getMaxY());

		spawnMaps.put((uint32) zoneids.get(i), newMap);
	}
}

int ResourceSpawnImplementation::getConcentration(bool jtl) {
	/**
	 * Here we are using defined rules to set the max
	 * density of this specific spawn
	 */

	if (jtl || isType("chemical") || isType("gas_inert"))
		return SpawnDensityMap::HIGHDENSITY;

	else if (isType("ore") || isType("water") || isType("energy_renewable_unlimited_solar") || isType("energy_renewable_unlimited_wind"))
		return SpawnDensityMap::LOWDENSITY;

	else
		return SpawnDensityMap::MEDIUMDENSITY;
}

Vector<uint32> ResourceSpawnImplementation::getSpawnZones(bool jtl,
		int zonerestriction, Vector<uint32>& activeZones) {

	/**
	 * Here we are using defined rules to set the number
	 * of zones and specific zones of this specific spawn
	 */

	Vector<uint32> zoneids;
	int zonecount = 0;

	/// If resource is zone restricted, add only the restricted zone
	if (zonerestriction != -1)
		zoneids.add((uint32) zonerestriction);

	/// If resource is JTL, it spawns on 1 random planet
	else if (jtl)
		zoneids.add((uint32) System::random(9));

	/// If resource is the types below, it spawns on 1-3 planets
	else if (isUnknownType() || isType("iron") || isType("ore_intrusive") || isType(
			"ore_extrusive") /*|| isType("iron")  why is iron twice? should be ore? */|| isType("fuel_petrochem_solid") || isType(
			"fuel_petrochem_liquid"))

		zonecount = System::random(2) + 1;

	/// All other resources spawn on 8 planets
	else
		zonecount = 8;

	/// If there are no more zones to add exit function
	if (zonecount == 0)
		return zoneids;

	/// Randomly remove entries until the Vector contains
	/// a number of elements equal to zonecount
	while (activeZones.size() > zonecount)
		activeZones.remove(System::random(activeZones.size() - 1));

	/// Add all the remaining items in activeZones to the
	/// zoneids vector
	while (activeZones.size() > 0)
		zoneids.add(activeZones.remove(0));

	return zoneids;
}

float ResourceSpawnImplementation::getDensityAt(int zoneid, float x, float y) {
	if (!spawnMaps.contains((uint32) zoneid))
		return 0;

	SpawnDensityMap map = spawnMaps.get((uint32) zoneid);

	return map.getDensityAt(x, y);
}

int ResourceSpawnImplementation::getSpawnMapZone(int i) {
	if (spawnMaps.size() > i)
		return spawnMaps.elementAt(i).getKey();
	else
		return -1;
}

ResourceContainer* ResourceSpawnImplementation::extractResource(int zoneid, int units) {
	unitsInCirculation += units;

	return createResource(units);
}

ResourceContainer* ResourceSpawnImplementation::createResource(int units) {
   	ResourceContainer* newResource = NULL;

   	newResource = (ResourceContainer*) getZoneServer()->createObject(containerCRC, 2);
   	newResource->setSpawnObject(_this);
   	newResource->setQuantity(units);
   	StringId customString;
   	customString.setCustomString(getFamilyName());
   	newResource->setObjectName(customString);

   	return newResource;
}

void ResourceSpawnImplementation::print() {
	info("**** Resource Data ****\n");
	info("Class: " + getFinalClass());
	info("Name: " + spawnName);
	info("--------Classes--------");
	for (int i = 0; i < spawnClasses.size(); ++i)
		info(spawnClasses.get(i) + "(" + stfSpawnClasses.get(i) + ")");
	info("------Attributes-------");

	for (int i = 0; i < spawnAttributes.size(); ++i) {
		String attrib;
		int value = getAttributeAndValue(attrib, i);
		info(attrib + " " + value);
	}

	for (int i = 0; i < spawnMaps.size(); ++i) {
		info(Planet::getPlanetName(spawnMaps.elementAt(i).getKey()));
		spawnMaps.get(i).print();
	}

	info("***********************");
}
