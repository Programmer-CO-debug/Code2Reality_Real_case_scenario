# Code2Reality

Adventure Quest: The Lost Kingdom

A single-player, menu-driven console adventure game written in C++17. The player chooses a hero class, fights through three stages of enemies, earns experience and gold, and tries to reclaim the Lost Kingdom. The project demonstrates core object-oriented programming concepts in a small, easy-to-read program.

Features
Character creation with three hero classes: Warrior, Mage, and Archer
Turn-based combat with a normal attack, a once-per-battle special ability, and a once-per-battle healing potion
Enemies that counterattack when they survive a player turn
Experience and gold rewards for each victory
Weapons found after each victory, equipped automatically when stronger
Three quest stages, with progression locked until the current quest is complete
Menus for character status, inventory, quest journal, and exit
End-of-session summary showing final status and progress
Hero Classes
Class	Health	Attack	Special Ability
Warrior	150	18	Shield Strike: 1.5x damage
Mage	90	22	Arcane Renewal: restores 30 HP
Archer	110	20	Precision Shot: 2x damage
Stages
Stage	Quest	Enemy	HP	Attack	XP	Weapon Reward
1	Forest of Whispers	Goblin Scout	60	8	40	Iron Sword (+5)
2	Marsh of Ashes	Orc Brute	100	12	80	Steel Axe (+8)
3	Throne of Shadows	Dark Knight	140	16	150	Crown Blade (+12)

Each victory also awards 25 gold.

Requirements
A C++ compiler with C++17 support: GCC, Clang, or Microsoft Visual C++
Windows, Linux, or macOS
A terminal or console window

No third-party libraries are needed. The program uses only the C++ standard library.

Building
bash
g++ -std=c++17 -Wall -Wextra -pedantic -o adventure Adventure_Quest.cpp

On Windows with MSVC:

bat
cl /std:c++17 /EHsc Adventure_Quest.cpp
Running
bash
./adventure

Enter a hero name, pick a class, and use the numbered menus to play. Choose “Fight stage enemy” from the main menu to start a battle. Once a quest is complete, choose “Advance to next stage” to continue.

Controls

All input is numeric menu selection, except the hero name. Invalid entries are rejected and the prompt is repeated.

Project Structure
Adventure_Quest.cpp    Full source code (all classes in a single file)
README.md              This file
OOP Design
Abstraction: Character is an abstract base class with pure virtual methods getClassName(), getSpecialName(), and specialAbility().
Inheritance: Warrior, Mage, and Archer derive from Character and reuse its shared data and behavior.
Polymorphism: The game controls the player through a Character pointer, and virtual methods run the correct class’s behavior at runtime.
Encapsulation: Data members are private or protected, and changes go through public methods.
Composition: Game owns the player through a unique_ptr, and manages collections of Quest and Weapon objects.
Limitations
Progress is not saved when the program closes.
Enemies, rewards, and quest stages are predefined.
Combat is deterministic, with no randomness.
The game is text-based and runs only in a terminal.
Future Enhancements
Save and load progress
More character classes
Random enemy encounters
Equipment upgrades and a map
Difficulty settings
License

Educational use. Created as part of a C++ OOP Continuous Internal Evaluation (CIE) project.
