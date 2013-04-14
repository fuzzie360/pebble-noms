# Noms Pebble Watch Face
_by Fazli Sapuan_

Animation Preview:

[![Animation Video](http://i.imgur.com/3HpHOF2.png)](http://youtu.be/1eEoC9VkqDo)

New fonts:

[![ScreenShot](http://i.imgur.com/uTbNOzm.png)](http://imgur.com/uTbNOzm)

**[Download](http://d.pr/f/oqEl)**

# Build Instructions

Clone this repository in an appropriate directory (e.g. ~/Documents/ or etc. ). I strongly suggest somewhere outside the pebble-dev directory .

	git clone git@github.com:Fuzzie360/pebble-noms.git

Set up waf:

	python ~/pebble-dev/pebble-sdk-release-001/tools/create_pebble_project.py --symlink-only ~/pebble-dev/pebble-sdk-release-001/sdk/ pebble-noms
	cd pebble-noms

Configure and build

	./waf configure
	./waf build

Install pebble-noms.pbw in build directory

# License

Copyright (C) 2013  Fazli Sapuan

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

You may contact the author at fazli@sapuan.org

# Fonts Used

Big Noodle Titling from Sentinel Type, designed by James Arboghast