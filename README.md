# Flowfield Research Project
Research project about Flowfields. Part of first semester Exam Assignment for Gameplay Programming, a second year course at Howest - [Digital Arts and Entertainment](https://digitalartsandentertainment.be/) 



## Program overview
Program starts with a blank 500x500 world. Spawnpoints, obstacles and goals can be places onto the world. The agents will either spawn at a random location in the world when no spawn points are present or spawn at spawnpoints over time. Agents will collide with obstalces and with other agents. Every agent will go to a random goal when spawned into the world. There has to be atleast one goal present in the world. If only one goal is present in the world and an agent reaches this goal, they despawn. If more than one goals are present in the world the agents can go to a next, random, goal unless the "remove at goal" checkbox is checked, they'll despawn if this is checked. A world can either be made by manually clicking on tiles or by loading in one of the saved file in the "Environments" folder, created worlds can be saved to a new file in this folder.
### Making the world
You can load in a pre made world by clicking "use from file" and writing a correct file name in the console. (all pre made world are listed below).  
To make your own world, use LMB to "place" the environment. Clicking a placed tile will remove it. Tile types can be switched by pressing 1 to 3 on the keyboard.
- 1: Goal* 
- 2: Obstacle
- 3: Spawner**
- 4: empty tile  
\*One goal is required.  
\*\*No spawner are  required, agents will spawn at a random location if there are no spawners.

For this project I used the Elite Engine framework, authors of this framework are Matthieu Delaere and Thomas Goussaert. 





