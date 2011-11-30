// for symbols
//-------------------------------
log(const text){}
label_settext (const id, const text){}
label_setfont (const id, const font){}
label_sethue (const id, const hue){}
label_setalign (const id, const align){}
label_gettext (const id){}
label_getfont (const id){}
label_gethue (const id){}
label_getalign (const id){}
label_getwidth(const id){}
label_getheight(const id){}
label_crop(const id, const width){}
label_wrap(const id, const width){}

border_setgump (const id, const gump){}
border_getgump (const id){}

config_setvalue (const id, const value){}
config_getvalue (const id) api_config_getvalue){}

// Added in 0.3.1
//-------------------------------
iris_connect(){}
iris_disconnect(){}
iris_deinit(){}
iris_init(){}
iris_setlightlevel(const level){}
iris_quit(){}
iris_playmusic(const id, const format, [const volume]){}
iris_playsound(const id, const volume){}

input_settext (const id, const text){}
input_gettext (const id){}
input_getwidth (const id){}
input_getheight (const id){}
input_gethue (const id){}
input_getfont (const id){}
input_getpasswordchar (const id){}
input_setwidth (const id, const width){}
input_sethue (const id, const hue){}
input_setfont (const id, const font){}
input_setpasswordchar (const id, const passwordchar){}
input_setignorecursorkeys (const id, const ignorecursorkeys){}

button_setgump (const id, const type, const gump){}
button_getgump (const id, const type){}
button_setdestinationpage (const id, const page){}
button_getdestinationpage (const id){}

textbox_addmessage(const id, const message, const timeout, [const hue]){}

container_setcurrentpage(const id, const page){}
container_getcurrentpage(const id){}

control_fade (const id, const alpha, const time){}
control_close (const id) api_control_close){}
control_getparent (const id){}
control_settag (const id, const slot, const tag){}
control_gettag (const id, const slot){}
control_setposition (const id, const x, const y){}
control_setsize (const id, const width, const height){}
control_setx (const id, const x){}
control_sety (const id, const y){}
control_setflags (const id, const flags){}
control_setflag (const id, const flag, const value){}
control_setalpha (const id, const alpha){}
control_getx (const id) api_control_getx){}
control_gety (const id) api_control_gety){}
control_getfocus (const id){}
control_getwidth (const id){}
control_getheight (const id){}
control_getflags (const id){}
control_getflag (const id, const flag){}
control_getalpha (const id){}
control_sethandler (const id, const type, [const funcname]){}
control_gethandler (const id, const type){}
control_setpage (const id, const page){}

gui_setcontainershape(const id){}
gui_translate (const x, const y){}
gui_gumpexists(const id) api_gui_gumpexists){}
gui_setcontainer( [const id] ){}
gui_clear([const id]) api_gui_clear){}
gui_addart(const x, const y, const artid, [ const flags, const hue ]){}
gui_registergump(const filename){}
gui_unregistergump(const id){}
gui_setfocus(const id) api_gui_setfocus){}
gui_setdefaultfocus(const id){}
gui_rewind([const id]) api_gui_rewind){}
gui_findnext([const id]) api_gui_findnext){}
gui_addcontainer(const x, const y, const width, const height){}
gui_addgump(const x, const y, const gump, [ const flags ]){}
gui_additemcontainer(const x, const y, const containerid){}
gui_addborder(const x, const y, const gump, [ const flags ]){}
gui_addbutton(const x, const y, [ const normal, const mouseover, const pressed ]){}
gui_addlabel(const x, const y, const text, [ const font, const hue ]){}
gui_addinput(const x, const y, const width, const height, [ const hue, const font, const background, const passwordchar ]){}
gui_addpaperdoll(const x, const y){}
gui_addtextbox() api_gui_addtextbox){}
gui_gettextwidth(const text, const font){}
gui_gettextheight(const text, const font){}
gui_addhtmllabel(const x, const y , const width, const height, const htmltext, const scrollbar){}
gui_addform (const x, const y, const gfm_file, [const is_container]){}
control_getid (const name){}

art_setid(const id, const artid) api_art_setid){}
art_getid(const id) api_art_getid){}

net_getserverlist (const index,[ var & id ]){}
net_selectserver (const id){}
net_getcharlist (const index){}
net_selectchar (const id) api_net_selectchar){}
net_createchar (const name, const sex, const str_val, const dex_val, const int_val, const skill1, const skill1_val, const skill2, const net_deletechar (const charindex){}
net_sendspeech (const text, [ const mode ]){}
net_getplayercharacter (){}
net_statusrequest (const id, [ const mode ]){}
net_dye(const id, const model, const color){}
net_rename (const id, const name){}
net_doaction(const type, [const mode]){}
net_warmode (const mode) api_net_warmode){}
net_getmenuitem (const index, [ var & model ]){}
net_selectmenu (const dialogid, const menuid, const index, const model){}
net_getpopupentry (const id){}
net_getpopuptag (const id) api_net_getpopuptag){}
net_sendpopupchoice (const id){}
net_senddoubleclick (const id){}
net_castspell(const spellid) api_net_castspell){}
net_updatebuylist (const mode, [const itemid, const amount]){}
net_buyitems () api_net_buyitems){}
net_updateselllist (const mode, [const itemid, const amount]){}
net_sellitems (const id) api_net_sellitems){}
net_sendtradecheck (const id, const check1, const check2){}
net_sendpickup (const id, const model, const count){}
net_getaostooltip (const id, const index){}
net_attackrequest(const charid){}
net_openbackpack() api_net_openbackpack){}
net_skilllock(const skillid, const lock){}

char_gethighlighthue (const id){}
char_getstatus (const id, const property){}
char_getskill(const id, const skillid, [var & name, var & unmodified, var & skillcap, var & skilllock, var & skill_button ]){}
char_addtext (const id, const text, [ const timeout , const hue]){}

paperdoll_refresh(const id, const charid){}
skill2_val, const skill3, const skill3_val, const skin_color, const hair, const hair_color, const facial_hair, const facial_hair_color, const shirt_color, const pants_color, const starting_location, const slot) api_net_createchar){}

//Object Functions
//-------------------------------
object_getcontentcount(const id){}
object_getcontententry(const id, const index){}
object_gethue(const id) api_object_gethue){}
object_getbody(const id) api_object_getbody){}
object_getquantity(const id){}
object_getparent(const id) api_object_getparent){}
object_getposition(const id, var & x, var & y, var & z){}
font_register(const id, const filename, const size, [const defaulthue]){}
font_unregister(const id) api_font_unregister){}
utils_bit_and(const a, const b){}
api_utils_bitand){}
utils_bit_or(const a, const b){}
utils_bit_rightshift(const a, const b){}
utils_bit_leftshift(const a, const b){}

//Harkon: NEW
//-------------------------------
button_getradioindex(const id){}
button_setradioindex(const id, const index){}
button_getchecked (const id){}
button_setchecked (const id, const checked){}
scrollbar_getpos (const id){}
scrollbar_setpos (const id, const pos){}
gui_loadform (const x, const y, const gfm_file, [const is_container]){}
cliloc_getmessage(const id)


Examples:

font_register:
	font_register(0, "fonts/font7.ttf", 18, 946);