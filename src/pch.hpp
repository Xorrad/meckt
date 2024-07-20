#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "util/Ptr.hpp"
#include "util/Logger.hpp"
#include "app/Configuration.hpp"

// Custom backward::SignalHandling to print stacktrace to file.
class SignalHandler;

class App;
class Menu;
class EditingMenu;
class EditingMenu;