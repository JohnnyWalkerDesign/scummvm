/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef RIVEN_STACK_H
#define RIVEN_STACK_H

#include "common/keyboard.h"
#include "common/hash-str.h"
#include "common/ptr.h"
#include "common/rect.h"
#include "common/str-array.h"

namespace Mohawk {

class MohawkEngine_Riven;

// NAME Resource ID's
enum RivenNameResource {
	kCardNames = 1,
	kHotspotNames = 2,
	kExternalCommandNames = 3,
	kVariableNames = 4,
	kStackNames = 5
};

/**
 * Name lists provide bidirectional association between an object's name and its id
 */
class RivenNameList {
public:
	RivenNameList();
	RivenNameList(MohawkEngine_Riven *vm, uint16 id);
	~RivenNameList();

	/** Get the name of an object using its id */
	Common::String getName(uint16 nameID) const;

	/**
	 * Get the id of an object using its name
	 *
	 * This query is case insensitive.
	 */
	int16 getNameId(const Common::String &name) const;

private:
	void loadResource(MohawkEngine_Riven *vm, uint16 id);

	Common::StringArray _names;
	Common::Array<uint16> _index;
};

/**
 * A game level
 *
 * The names Card and Stack are legacy from the HyperCard engine used in
 * the original mac version of Myst.
 *
 * Stacks contain behaviors and data that are specific to a game level.
 */
class RivenStack {
public:
	RivenStack(MohawkEngine_Riven *vm, uint16 id);
	virtual ~RivenStack();

	typedef Common::Functor0<void> TimerProc;

	/** Get the id of the stack */
	uint16 getId() const;

	/** Get the name of a resource using its id */
	Common::String getName(RivenNameResource nameResource, uint16 nameId) const;

	/**
	 * Get the id of a resource using its name
	 *
	 * The search is case insensitive.
	 */
	int16 getIdFromName(RivenNameResource nameResource, const Common::String &name) const;

	/** Get the id of a card in the card from its global identifier */
	uint16 getCardStackId(uint32 globalId) const;

	/** Get the global id of the currently active card */
	uint32 getCurrentCardGlobalId() const;

	/** Get the global id of a card in the stack */
	uint32 getCardGlobalId(uint16 cardId) const;

	/** Run an external command with the specified parameters */
	void runCommand(uint16 argc, uint16 *argv);

	/** Write all of the stack's data including its cards to standard output */
	void dump() const;

	/** Install a timer for the current card if one is defined */
	virtual void installCardTimer();

	/** Clear any currently installed timer */
	void removeTimer();

	/** Handle a mouse down event */
	void onMouseDown(const Common::Point &mouse);

	/** Handle a mouse up event */
	void onMouseUp(const Common::Point &mouse);

	/** Handle a mouse move event */
	void onMouseMove(const Common::Point &mouse);

	/** Frame update handler */
	void onFrame();

	/** Is the left mouse button currently pressed? */
	bool mouseIsDown() const;

	/** Get the current mouse position */
	Common::Point getMousePosition() const;

	/** Get the mouse position when dragging started */
	Common::Point getMouseDragStartPosition() const;

	/** Force the left mouse button to be considered unpressed until the next mouse click */
	void mouseForceUp();

	/** Handle a key press event */
	void onKeyPressed(const Common::KeyCode keyCode);

	/** Get the pressed keyboard key if any */
	Common::KeyCode keyGetPressed() const;

	/** Force the keyboard to be considered unpressed until the next key press */
	void keyForceUp();

	// Common external commands
	void xflies(uint16 argc, uint16 *argv); // Start the "flies" effect

	// TODO: Misc stuff move elsewhere
	uint16 getComboDigit(uint32 correctCombo, uint32 digit);
	void runDemoBoundaryDialog();
	void runEndGame(uint16 videoCode, uint32 delay);
	void runCredits(uint16 video, uint32 delay);

protected:
	typedef Common::Functor2<uint16, uint16 *, void> ExternalCommand;

	MohawkEngine_Riven *_vm;

	/** Register an external command for use by the scripts */
	void registerCommand(const Common::String &name, ExternalCommand *command);

	/** Register a proc for planned execution */
	void installTimer(TimerProc *proc, uint32 time);

private:
	typedef Common::HashMap<Common::String, Common::SharedPtr<ExternalCommand>, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo> CommandsMap;

#define REGISTER_COMMAND(cls, method) \
		registerCommand( \
			#method, new Common::Functor2Mem<uint16, uint16 *, void, cls>(this, &cls::method) \
		)

#define TIMER(cls, method) \
		new Common::Functor0Mem<void, cls>(this, &cls::method)

	void loadResourceNames();
	void loadCardIdMap();
	void setCurrentStackVariable();
	void checkTimer();

	uint16 _id;

	// Stack resource names
	RivenNameList _varNames;
	RivenNameList _externalCommandNames;
	RivenNameList _hotspotNames;
	RivenNameList _cardNames;
	RivenNameList _stackNames;

	Common::Array<uint32> _cardIdMap;

	CommandsMap _commands;

	Common::KeyCode _keyPressed;

	bool _mouseIsDown;
	Common::Point _mousePosition;
	Common::Point _mouseDragStartPosition;

	// Timer
	Common::SharedPtr<TimerProc> _timerProc;
	uint32 _timerTime;
};

namespace RivenStacks {
	/** Get a stack name from an id */
	const char *getName(uint16 stackId);

	/** Get a stack id from a name */
	uint16 getId(const char *stackName);
}

} // End of namespace Mohawk

#endif