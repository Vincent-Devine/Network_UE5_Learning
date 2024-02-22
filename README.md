# Shooter multi
3rd year exercice that aims at learning network programming on Unreal Engine 5.<br/>

**!! Warning, You need to added the plugin: AdvencedSessionsPlugin !!**
https://github.com/mordentral/AdvancedSessionsPlugin/tree/master

## Table of content ##
 - [Project description](#project-description)
 - [Quick Start](#quick-start)
 - [Technology](#technologie)
 - [Organization](#organization)
 - [Credit](#credit)

## Project description ##
The project is a 3rd-person shooter that can be played by several players in a network on Unreal Engine 5.2, more specifically a team deathmatch / horde mode: blue team versus red team.

### Instruction ###
1. Basic features
The project will offer the basic gameplay features listed (character, IA, pickups, team, cubes), functional from the
editor.
2. Lobby
The lobby will use the Unreal session system. Sessions will allow you to
    - search and list games in progress
    - select a game to join
Once a game has been joined, the lobby will function in a similar way to the previous version.
3. In-game info
You can add various information during gameplay phases

## Quick Start ##
Unreal Engine 5 project
1. Download the project from perforce
2. Open the project (*double click on .uproject*)
3. Wait shader compilation and build light
4. Start the server with double click the ``` StartDedicatedServer.bat ```
5. On the editor. Play the level ``` ClientBoot ```

## To prevent crash ##
6. Launch the first player by connecting to server and launching game
7. Connect the second player to the server will automaticaly make him join the current game 

## Technologie ##
- Engine :  Unreal Engine 5.2
- IDE : Visual Studio 2022
- Versionning : Perforce (team: Splatoon)
- Plugins:
    - Advanced Sessions
    - Substance 3D

## Organization ##
| Tasks                                           | Omaya | Vincent |
|-------------------------------------------------|:-----:|:-------:|
| Replicate basic features on network (character) | x     | x       |
| Replicate basic features on network (IA)        | x     | x       |
| Replicate basic features on network (Pickups)   | x     |         |
| Replicate basic features on network (Team)      |       | x       |
| Replicate basic features on network (Cubes)     | x     |         |
| Lobby                                           | x     | x       |

## Credit ##
Exercice done at **ISART DIGITAL** <br>
Authors : Omaya LISE, Vincent DEVINE <br>
Special thanks : Florian Wolf <br>
Project start : 03-11-2023 <br>
Project end : 28-11-2023 <br>