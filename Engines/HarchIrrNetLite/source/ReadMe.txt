IrrNetLite V2.0 - Lite Enet Framework
-------------------------------------

irrNetLite is an lite enet wrapper with a clean C++ interface.
Please look at Tutorial.cpp for a quick introduction to coding with irrNetLite.

switch(YourQuestions)
{
Why irrNetLite?:
	Enet is a portable, robust and reliable UDP implementation, but unfortunately
	it comes with an ugly C-style interface that can be rather difficult to integrate
	with a large scale application (Or to look at in general.). irrNetLite makes the
	experience a whole lot better by imploying OO-style interfaces for packet handling
	and connection callbacks and a sprinkle here and there of synctatic sugar (Operator
	overloading, etc). irrNetLite also implements some useful features not present in
	vanilla Enet (Namely compression and encryption), in the same easy to use manner.

Is there a connection to Irrlicht?:
What are all these header files?:
Do you have something against the std lib?:	irrNetLite V1.0 was originally based off irrNet. irrNet was a network library
	for integration of Enet and Irrlicht to create a persistant multi-user
	enviroment. irrNet was dependant on Irrlicht. irrNetLite however, has
	no dependancy whatsoever on Irrlicht being included or linked in. I do however
	like some of the Irrlicht typedefs and helper structures, so those are included
	amongst the header files, but by no means do you have to use them in your
	application. I do recommend using the typedefs, as it is important to maintain
	more precise typing (unsigned/signed) when sending data over a network due
	to deferring architecture defaults.
}

LICENSE
-----------

  Copyright (C) 2008 Ahmed Hilali

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgement in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be clearly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
  
