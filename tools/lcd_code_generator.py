#!/usr/bin/python
# -*- coding: UTF-8 -*-
#    MIT License
#
#    Copyright (c) 2019, Alexey Dynda
#
#    Permission is hereby granted, free of charge, to any person obtaining a copy
#    of this software and associated documentation files (the "Software"), to deal
#    in the Software without restriction, including without limitation the rights
#    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#    copies of the Software, and to permit persons to whom the Software is
#    furnished to do so, subject to the following conditions:
#
#    The above copyright notice and this permission notice shall be included in all
#    copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#    SOFTWARE.
#
###################################################################################

import re
import sys
import os
import shutil
import json
from collections import OrderedDict

def print_help_and_exit():
    print("Usage: lcd_code_generator.py [args]")
    print("args:")
    print("      -c name   controller name")
    print("      -b bits   bits per pixel")
    print("      -B        Do not add bits to screen class name")
    print("      -r WxH    resolution")
    print("      -j json   Use json data from file")
    print("      -t template path source templates, templates by default")
    print("")
    print("exammples:")
    print("      ./lcd_code_generator.py -c all")
    exit(1)

if len(sys.argv) < 2:
    print_help_and_exit()

g_voc={}

controller=""
templates="templates"
resolution_db = []
g_voc = {
          "options":
          {
              "no_bits_in_name": False,
              "args_in_cmd_mode": False,
              "rowcol_bits": 8,
              "col_cmd": "0x15",
              "row_cmd": "0x75",
              "use_paging": False,
          },
          "interfaces": {},
          "bits": {},
          "CONTROLLER": "",
          "controller": "",
          "resolution": "",
          "exbits": "",
          "_bits": "",
          "width": "",
          "height": "",
          "init_data": "" }

# parse args
idx = 1
while idx < len(sys.argv):
    opt = sys.argv[idx]
    if opt == "-c":
        idx += 1
        controller = sys.argv[idx]
    elif opt == "-b":
        idx += 1
        bits = int(sys.argv[idx])
        g_voc["bits"].append({"bits": zip( resolution_db, [{}] * len(resolution_db) )})
        resolution_db = []
    elif opt == "-t":
        idx += 1
        templates = sys.argv[idx]
    elif opt == "-j":
        idx += 1
    elif opt == "-r":
        idx += 1
        resolution = sys.argv[idx]
        resolution_db.append( resolution )
        for t in g_voc["bits"]:
            t.append( { resolution: {} } )
    else:
        print("Unknown option: ", opt)
        print_help_and_exit()
    idx += 1

def get_val_by_path(path, default):
    nodes = path.split('/')
    data = g_voc;
    for i in range(0, len(nodes) - 1):
        data = data.get(nodes[i],{})
    # check if file exists
    ctl_path = templates + "/lcd/" + g_voc["controller"] + "/" + path
    base_path = templates + path
    if len( data ) == 0 and os.path.exists(ctl_path) and os.path.isfile(ctl_path):
        with open(ctl_path, 'r') as myfile:
            data = myfile.read().splitlines()
    elif len( data ) == 0 and os.path.exists(base_path) and os.path.isfile(base_path):
        with open(base_path, 'r') as myfile:
            data = myfile.read().splitlines()
    else:
        data = data.get( nodes[-1], default )
    return data

def read_template(fname):
    if not os.path.isfile(templates + fname):
        return ""
    with open(templates + fname, 'r') as myfile:
        data=myfile.read()
    return data

def fill_template(temp):
    temp = temp.replace('~FUNCS_DECL~', get_val_by_path("FUNCS_DECL", ""))
    temp = temp.replace('~FIELDS_DECL~', get_val_by_path("FIELDS_DECL", ""))
    temp = temp.replace('~SERIAL_INTERFACE_ARGS~', get_val_by_path("serial_interface_args", ""))
    temp = temp.replace('~CUSTOM_SERIAL_INTERFACE_ARGS~', get_val_by_path("custom_serial_interface_args", ""))
    temp = temp.replace('~CUSTOM_INTERFACE_ARGS~', get_val_by_path("custom_interface_args", ""))
    temp = temp.replace('~CONTROLLER~', get_val_by_path("CONTROLLER",""))
    temp = temp.replace('~controller~', get_val_by_path("controller",""))
    temp = temp.replace('~RESOLUTION~', get_val_by_path("resolution",""))
    temp = temp.replace('~EXBITS~', get_val_by_path("exbits",""))
    temp = temp.replace('~BITS~', get_val_by_path("_bits",""))
    temp = temp.replace('~WIDTH~', get_val_by_path("width",""))
    temp = temp.replace('~HEIGHT~',get_val_by_path("height",""))
    temp = temp.replace('~INIT~', get_val_by_path("init_data",""))
    temp = temp.replace('~OPTIONAL_CONFIG~', get_val_by_path("optional_config",""))
    temp = temp.replace('~CONFIG_FUNC~', get_val_by_path("options/config_func","_configureSpiDisplay"))
    temp = temp.replace('~SET_BLOCK~', get_val_by_path("_set_block",""))
    temp = temp.replace('~END_BLOCK~', get_val_by_path("_end_block", "    this->stop();"))
    temp = temp.replace('~FREQUENCY~', get_val_by_path("_frequency", "4400000"))
    temp = temp.replace('~I2C_ADDR~', get_val_by_path("interfaces/i2c/addr", "0x3C"))
    temp = temp.replace('~FUNCS_DEF~', get_val_by_path("FUNCS_DEF", ""))
    temp = temp.replace('~RESET_DURATION~', str(get_val_by_path("options/reset_duration", 20)))
    temp = temp.replace('~RESET_DELAY~', str(get_val_by_path("options/reset_delay", 100)))
    return temp

def get_file_data(fname):
    temp = read_template(fname)
    temp = fill_template(temp)
    return temp

def get_json_controller_list(fname):
    with open(templates + 'lcd/' + fname + "/" + fname + ".json") as json_file:
        data = json.load(json_file, object_pairs_hook=OrderedDict)
    return data.keys()

def load_data_from_json(fname, ctl):
    with open(templates + 'lcd/' + fname) as json_file:
        data = json.load(json_file, object_pairs_hook=OrderedDict)
    g_voc["bits"] = data[ctl]["bits"];
    g_voc["options"] = data[ctl]["options"]
    g_voc["interfaces"] = data[ctl]["interfaces"]
    g_voc["functions"] = data[ctl].get("functions", {})

def load_init_data_from_json(fname, ctl, bits, resolution):
    with open(templates + 'lcd/' + fname) as json_file:
        data = json.load(json_file, object_pairs_hook=OrderedDict)
    g_voc["init_data"] = "\n".join( data[ctl]["bits"][bits][resolution]["init"] )
    if "begin" in data[ctl]["bits"][bits][resolution]:
        g_voc["optional_config"] = "\n".join( data[ctl]["bits"][bits][resolution]["begin"] )
    else:
        g_voc["optional_config"] = ""

templates = templates + "/"

def generate_custom_decl(name, doc=["    /**", "     * DOCUMENT","     */"], type=["void",""]):
    lines = get_val_by_path("functions/" + name + "/doc", doc)
    decl = get_val_by_path("functions/" + name + "/decl", type)
    init = get_val_by_path("functions/" + name + "/init", None)
    if init is not None and len(init) > 0:
        lines.append( ' '*4 + decl[0])
        lines.append( ' '*4 + name + "(" + ', '.join(decl[1:]) +")")
        lines.append( ' '*8 + ": " + init[0])
        for i in init[1:]:
            lines.append(' '*8 + ", " + i)
        lines.extend([' '*4 + "{", ' '*4 + "}"])
    else:
        lines.append( "    " + decl[0] + " " + name + "(" + ', '.join(decl[1:]) +");")
    return fill_template('\n'.join(lines))

def generate_custom_def(name, type=["void",""], code=[]):
    init = get_val_by_path("functions/" + name + "/init", None)
    if init is not None:
        return None
    delc = get_val_by_path("functions/" + name + "/decl", type)
    lines = [ "template <class I> " + delc[0] + \
              " Interface~CONTROLLER~<I>::" + name + "(" + \
              ', '.join(delc[1:]) +")", "{" ]
    code = get_val_by_path("functions/" + name + "/code",code)
    if len(code) == 0:
        if name == "startBlock":
            code = [generate_set_block_content()]
    lines.append( '\n'.join(code) )
    lines.extend( ["}", ""] )
    return fill_template('\n'.join(lines))

def generate_set_block_content():
    lines = ["    lcduint_t rx = w ? (x + w - 1) : (m_base.width() - 1);",
             "    commandStart();" ]
    lines.append("    this->send({0});".format(get_val_by_path("options/col_cmd", "0x22")))
    if not get_val_by_path("options/args_in_cmd_mode", False):
        lines.append("    setDataMode(1); // According to datasheet all args must be passed in data mode")
    if get_val_by_path("options/rowcol_bits",8) != 8:
        lines.append("    this->send(0);")
    if get_val_by_path("options/column_div", 1) > 1:
        lines.append( "    this->send(x / {0});".format(get_val_by_path("options/column_div", 1)) )
    else:
        lines.append( "    this->send(x);" )
    if get_val_by_path("options/rowcol_bits",8) != 8:
        lines.append("    this->send(0);")
    if get_val_by_path("options/column_div", 1) > 1:
        lines.append( "    this->send( (rx < m_base.width() ? rx : (m_base.width() - 1)) / {0} );".format(
                     get_val_by_path("options/column_div", 1)) )
    else:
        lines.append( "    this->send( rx < m_base.width() ? rx : (m_base.width() - 1) );" )

    if not get_val_by_path("options/args_in_cmd_mode", False):
        lines.append("    setDataMode(0);")
    lines.append("    this->send({0});".format(get_val_by_path("options/row_cmd", "0x72")))
    if not get_val_by_path("options/args_in_cmd_mode", False):
        lines.append("    setDataMode(1); // According to datasheet all args must be passed in data mode")
    if get_val_by_path("options/rowcol_bits",8) != 8:
        lines.append("    this->send(0);")
    lines.append("    this->send(y);")
    if get_val_by_path("options/rowcol_bits",8) != 8:
        lines.append("    this->send(0);")
    lines.append("    this->send(m_base.height() - 1);")
    if not get_val_by_path("options/args_in_cmd_mode", False):
        lines.append("    setDataMode(0);")
    if get_val_by_path("options/exit_cmd_mode_command", None) is not None:
        lines.append("    this->send({0});".format( get_val_by_path("options/exit_cmd_mode_command", None) ) )
    lines.extend( [ "    if ( m_dc >= 0 )",
                    "    {",
                    "        setDataMode(1);",
                    "    }",
                    "    else",
                    "    {",
                    "        this->stop();",
                    "        this->start();",
                    "        this->send(0x40);",
                    "    }" ] )
    return "\n".join(lines)

def generate_controller_data(jsonfile, ctl):
    controller = ctl.lower();
    g_voc["CONTROLLER"] = controller.upper()
    g_voc["controller"] = controller.lower()

    if jsonfile is not None:
        load_data_from_json( jsonfile, ctl )
    func_list = ["Interface~CONTROLLER~", "startBlock", "nextBlock", "endBlock", "setDataMode", "commandStart"]
    for f in get_val_by_path( "functions/interface_list",[] ):
        if f not in func_list:
            func_list.append( f )
    g_voc["FUNCS_DECL"] = ""
    for f in func_list:
        g_voc["FUNCS_DECL"] += generate_custom_decl(f) + '\n\n'
    g_voc["FUNCS_DEF"] = ""
    for f in func_list:
        custom_def = generate_custom_def(f)
        if custom_def is not None:
            g_voc["FUNCS_DEF"] += custom_def + '\n'
    g_voc["FIELDS_DECL"] = '\n'.join(get_val_by_path("fields/Interface~CONTROLLER~",[]))

    location = "../src/v2/lcd/" + controller
    shutil.rmtree(location,True)
    os.mkdir(location)
    header = open(location + "/lcd_" + controller + ".h", "w" )
    inl = open(location + "/lcd_" + controller + ".inl", "w" )
    cpp = open(location + "/lcd_" + controller + ".cpp", "w" )

    header.write( get_file_data('copyright.txt') )
    inl.write( get_file_data('copyright.txt') )
    cpp.write( get_file_data('copyright.txt') )

    header.write( get_file_data('header.h') )
    inl.write( get_file_data('header.inl') )
    cpp.write( get_file_data('header.cpp') )

    header.write( get_file_data('interface_spi.h') )

    for _bits in g_voc["bits"].keys():
        g_voc["_bits"] = _bits;
        if not get_val_by_path("options/no_bits_in_name", False):
            g_voc["exbits"] = "x" + g_voc["_bits"]
        else:
            g_voc["exbits"] = ""
        header.write( get_file_data( 'display.h' ) )
        inl.write( get_file_data( 'display.inl' ) )
        for res in g_voc["bits"][_bits].keys():
            g_voc["resolution"] = res + g_voc["exbits"]
            g_voc["init_data"] = get_file_data('init_data.inl')
            if jsonfile is not None:
                load_init_data_from_json(jsonfile, ctl, _bits, res)
            g_voc["width"] = res.split('x')[0]
            g_voc["height"] = res.split('x')[1]
            header.write( get_file_data('resolution.h') )
            inl.write( get_file_data('resolution.inl') )
            for intf in g_voc["interfaces"].keys():
                g_voc["serial_interface_args"] = get_val_by_path("bits/" + _bits + "/" + res + "/serial_interface_args",\
                                          "*this, -1" if intf == "i2c" else "*this, config.dc")
                g_voc["custom_serial_interface_args"] = get_val_by_path("bits/" + _bits + "/" + res + "/custom_serial_interface_args",\
                                          "*this, -1" if intf == "i2c" else "*this, dcPin")
                g_voc["custom_interface_args"] = get_val_by_path("bits/" + _bits + "/" + res + "/custom_interface_args",\
                                          "*this, dcPin, frequency = frequency ? frequency : ~FREQUENCY~")
                g_voc["_frequency"] = str(get_val_by_path( "interfaces/" + intf + "/frequency", 4400000 ))
                header.write( get_file_data('display_' + intf + '.h') )
                cpp.write( get_file_data('display_' + intf + '.cpp') )

    header.write( get_file_data('footer.h') )

    header.close()
    inl.close()
    cpp.close()

display_list = [f for f in os.listdir(templates + "lcd") if len(f) > 3]
for display in display_list:
    _ctl_list = get_json_controller_list(display)
    for _ctl in _ctl_list:
        generate_controller_data( _ctl + "/" + _ctl + ".json", _ctl)
