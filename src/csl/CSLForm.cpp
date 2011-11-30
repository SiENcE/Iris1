/*****
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****/

#include "csl/CSLHandler.h"

#include <stdlib.h>
#include <stdio.h>
#include <cassert>

#include "Logger.h"

#include <csl/ZExcept.hpp>

#include "Game.h"
#include "SDLEvent.h"
#include "gui/Image.h"
#include "gui/Border.h"
#include "gui/ImageArt.h"
#include "gui/Button.h"
#include "gui/InputField.h"
#include "gui/Label.h"
#include "gui/Container.h"
#include "gui/ItemContainer.h"
#include "gui/GUIHandler.h"
#include "gui/Textbox.h"
#include "gui/Paperdoll.h"
#include "gui/TextManager.h"

#include "renderer/DynamicObjects.h"

#include "loaders/GumpLoader.h"
#include "loaders/SkillLoader.h"

#include "net/Client.h"

#include "Config.h"
#include "include.h"

#include "xml.h"




extern GUIHandler pUOGUI;

// in xml.cpp

// That is not the best way, but I only applied a patch

int on_clickhandler(Control * control);
int on_closehandler(Control * control);
int on_mousedownhandler(Control * control);
int on_mouseuphandler(Control * control);
int on_keypressedhandler (Control * control, unsigned short key);
extern void api_addcontrol (Control * control);
extern int act_container_id;

inline unsigned int checksum32(std::string s)
{ 
	unsigned char c1=0,c2=0,c3=0,c4=0;

	for (unsigned int i=0; i < s.length(); i++)
	{
		c1+=s[i];
		c2+=c1;
		c3+=c2;
		c4+=c3;

	}

	return (c1|(c2<<8)|(c3<<16)|(c4<<24));
}

inline unsigned int checksum16(std::string s)
{
	unsigned char c1=0,c2=0;

	for (unsigned int i=0; i < s.length(); i++)
	{
		c1+=s[i];
		c2+=c1;
	}

	return (c1|(c2<<8));
}

static std::map<Uint32,Uint32> controlid_lookup;

static void clear_controlid_list()
{
	controlid_lookup.clear();
}

static void add_controlid(std::string name, Uint32 id)
{
	controlid_lookup.insert(std::make_pair(checksum32(name), id));
}

static Uint32 get_controlid(std::string name)
{
	std::map<Uint32,Uint32>::iterator iter = controlid_lookup.find(checksum32(name));
	return iter != controlid_lookup.end() ? iter->second : 0;
}

ZString api_control_getid(ZCsl* aCsl)
{

	ZString name = aCsl->get("name");

	return get_controlid(name.buffer());
}

ZString api_gui_addform( ZCsl *aCsl )
{
	clear_controlid_list();

	int argCount = aCsl->get("argCount").asInt();
//	int flags = GUMPFLAG_MOVABLE | GUMPFLAG_CLOSABLE | GUMPFLAG_FOCUSABLE;
	int x = aCsl->get("x").asInt();
	int y = aCsl->get("y").asInt();
	std::string gfm_file = std::string (aCsl->get("gfm_file").buffer ());
	bool is_container = (argCount > 3) ? ((int) aCsl->get("is_container").asInt()) : false;

	Container* container = NULL;
	std::string shape;
	
	XML::Parser parser;
	XML::Node* form=NULL,* document=NULL;
	
	try
	{
		parser.loadData(gfm_file);
		document = parser.parseDocument();

		int width=640,height=480, alpha=0, flags=0, fade_alpha=0, fade_time=0;
		
		std::string onclick, onclose, onmouseup, onmousedown, onkeypressed;
		
		form = document->findNode("form");
		if ( !form )
		{
			throw;
		}
		
		form->lookupAttribute("width", width);
		form->lookupAttribute("height", height);
		form->lookupAttribute("alpha", alpha);
		form->lookupAttribute("flags", flags);
		form->lookupAttribute("shape", shape);

		XML::Node* fade_node = form->findNode("fade");
		if (fade_node)
		{
			fade_node->lookupAttribute("alpha", fade_alpha);
			fade_node->lookupAttribute("time", fade_time);
		}

		XML::Node* event_node = form->findNode("event");

		if (event_node)
		{
			event_node->lookupAttribute("onclick", onclick);
			event_node->lookupAttribute("onclose", onclose);
			event_node->lookupAttribute("onmouseup", onmouseup);
			event_node->lookupAttribute("onmousedown", onmousedown);
			event_node->lookupAttribute("onkeypressed", onkeypressed);
		}	

		if (is_container)
		{
			container = new Container();
			container->SetPosition(x,y);
			container->SetSize(width, height);
			container->SetAlpha(alpha);
			if (fade_time != 0) 
				container->FadeTo(fade_alpha, fade_time);

			x = 0, y = 0;
			
			api_addcontrol( container );

			Control* control=container;
			if (!onclose.empty()) {
				control->OnClose(on_closehandler);
				control->SetScriptFunction(FUNC_ONCLOSE, (char*)onclose.c_str());
			}
			if (!onmousedown.empty()) {
				control->OnMouseDown(on_mousedownhandler);
				control->SetScriptFunction(FUNC_ONMOUSEDOWN, (char*)onmousedown.c_str());
			}
			if (!onmouseup.empty()) {
				control->OnMouseUp(on_mouseuphandler);
				control->SetScriptFunction(FUNC_ONMOUSEUP, (char*)onmouseup.c_str());
			}

			act_container_id = container->GetID();
		}
	} catch (...) {
		//delete document;
		return "0";
	}

	int idx=0;
	XML::Node* control_node = NULL;			
	
	while ((control_node = form->findNode("control", idx++))) {
		Control* control=NULL;
		XML::Node* event_node=NULL,* gump_node=NULL, * passwd_node=NULL;
		XML::Node* check_node=NULL,* group_node=NULL,* text_node=NULL;
		
		std::string type, name;
		std::string onclick, onclose, onmouseup, onmousedown, onkeypressed;

		int left=0,top=0,width=0,height=0,freezed=0,visible=1;			
		int alpha=0,flags=0;

		if (!control_node->lookupAttribute("class", type)) continue;

		control_node->lookupAttribute("name", name);
		control_node->lookupAttribute("left", left);
		control_node->lookupAttribute("top", top);
		control_node->lookupAttribute("width", width);
		control_node->lookupAttribute("height", height);
		control_node->lookupAttribute("freezed", freezed);
		control_node->lookupAttribute("visible", visible);
		control_node->lookupAttribute("alpha", alpha);
		control_node->lookupAttribute("flags", flags);

		left+=x;
		top+=y;

		if ((event_node = control_node->findNode("event"))) {
			event_node->lookupAttribute("onclick", onclick);
			event_node->lookupAttribute("onclose", onclose);
			event_node->lookupAttribute("onmouseup", onmouseup);
			event_node->lookupAttribute("onmousedown", onmousedown);
			event_node->lookupAttribute("onkeypressed", onkeypressed);
		}	
		
		if (type=="picture" || type=="border" || type=="paperdoll") {			
			int gump=0;
			if ((gump_node = control_node->findNode("gump"))) 
				gump = gump_node->asInteger();

			//int flags = GUMPFLAG_MOVABLE | GUMPFLAG_CLOSABLE | GUMPFLAG_FOCUSABLE;
			
			if (type=="picture") {
				control = new Image(left, top, gump, flags);
			} else if (type=="border") {
				control = new Border(left,top, gump, flags);
			} else {
				control = new Paperdoll(left, top, 0);
			}
			control->SetSize(width, height);
			
		} else if (type=="button") {
			int normal=-1, over=-1, pressed=-1;
			if ((gump_node = control_node->findNode("gump"))) { 
				gump_node->lookupAttribute("normal", normal);
				gump_node->lookupAttribute("over", over);
				gump_node->lookupAttribute("pressed", pressed);
			}

			//Button *button = new Button(left,top);
			
			
			/*if (normal >= 0) button->SetButton(BUTTONGUMP_NORMAL, normal);
			if (over >= 0) button->SetButton(BUTTONGUMP_MOUSEOVER, over);
			if (pressed >= 0) button->SetButton(BUTTONGUMP_PRESSED, pressed);*/
			control = new Button( left, top );

			if (normal >= 0)
				((Button*)control)->SetButton(BUTTONGUMP_NORMAL, normal);
			if (over >= 0)
				((Button*)control)->SetButton(BUTTONGUMP_MOUSEOVER, over);
			if (pressed >= 0)
				((Button*)control)->SetButton(BUTTONGUMP_PRESSED, pressed);

			if (!onclick.empty()) {
				/*button->OnClick(on_clickhandler);
				button->SetScriptFunction(FUNC_ONCLICK, (char*)onclick.c_str());*/
				((Button*)control)->OnClick(on_clickhandler);
				((Button*)control)->SetScriptFunction(FUNC_ONCLICK, (char*)onclick.c_str());
			}
			
			//control = button;
		} else if (type=="checkbox" || type=="radio") {
			int normal=-1, checked=-1, group=0, ischecked=0;
			gump_node = control_node->findNode("gump");
			if (gump_node) { 
				gump_node->lookupAttribute("normal", normal);
				gump_node->lookupAttribute("checked", checked);
			}
			if ((check_node = control_node->findNode("checked"))) {
				ischecked = check_node->asBool();
			}
			if ((group_node = control_node->findNode("group"))) {
				group = group_node->asInteger();
			}
		} else if (type=="label" || type=="edit") {
			std::string text, align;
			int font=-1,hue=-1,password=0, label_align=-1;
			if ((text_node = control_node->findNode("text"))) {
				text = text_node->asString();
				text_node->lookupAttribute("font", font);
				text_node->lookupAttribute("hue", hue);
				text_node->lookupAttribute("align", align);
			}

			if ((passwd_node = control_node->findNode("password"))) {
				password = passwd_node->asBool();
			}
			
			if (type == "label") {
				if (align=="center") {
					label_align=1;
					left += width / 2;
					//top  += height / 2;
				} else if (align=="right") {
					label_align=2;
					left += width;
					//top  += height;
				}

				Label* label = new Label(left, top, text.c_str(), hue, font);
/*SiENcE:
				if (font >= 0) label->setFont(font);
				if (hue >= 0) label->setHue(hue);
*/
				if (label_align >= 0) label->setAlign(label_align);
					control = label;
				}
				else
				{
					font = font < 0 ? 3 : font;
					hue = hue < 0 ? 0 : hue;
					InputField *input = new InputField(left,top,width,height, text.c_str(), hue, font, password ? 42 : 0);
					input->OnKeyPress(on_keypressedhandler);
					if (!onkeypressed.empty())
						input->SetScriptFunction(FUNC_ONKEYPRESSED, (char*)onkeypressed.c_str());
					control = input;
				}
		}

		if ( !control )
		{
			continue;
		}

		api_addcontrol(control);

		add_controlid(name,control->GetID());

		if (!onclose.empty()) {
			control->OnClose(on_closehandler);
			control->SetScriptFunction(FUNC_ONCLOSE, (char*)onclose.c_str());
		}
		if (!onmousedown.empty()) {
			control->OnMouseDown(on_mousedownhandler);
			control->SetScriptFunction(FUNC_ONMOUSEDOWN, (char*)onmousedown.c_str());
		}
		if (!onmouseup.empty()) {
			control->OnMouseUp(on_mouseuphandler);
			control->SetScriptFunction(FUNC_ONMOUSEUP, (char*)onmouseup.c_str());
		}
	}

	document = NULL;

	if ( container )
	{
		if ( !shape.empty() )
		{
			container->SetShape( get_controlid( shape ) );
		}
		act_container_id = 0;
		return ZString( container->GetID() );
	}

	return "0";
}
