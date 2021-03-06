#ifndef STL_HEADER
#define STL_HEADER

// Precompiled Header


// Disable useless warnings

#pragma warning( disable : 26812 ) // Unscoped enums

// STL Stuff

#include <algorithm>

#include <string>
using std::string;

#include <map>
using std::map;

#include <unordered_map>
using std::unordered_map;

#include <stack>
using std::stack;

#include <array>
using std::array;

#include <vector>
using std::vector;

#include <iostream>
using std::cerr;
using std::endl;

#include <algorithm>

#include <queue>
using std::queue;

#include <utility>
using std::pair;

#include <cmath>

#include <fstream>

#include <functional>
using std::function;

#include <exception>

#include <cstdint>

#include <list>
using std::list;

#include <set>
using std::set;

// SDL Libraries

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

// My own utilities

#include "MiscUtility.h"
#include "LogHandler.h"

#endif
