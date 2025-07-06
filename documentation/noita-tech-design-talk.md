# Exploring the Tech and Design of Noita

## Introduction

We're going to be talking about exploring the tech and design of Noita. Because the game is not out yet, I'm just going to show you a trailer, and hopefully it'll explain a little bit about the game.

We had some trouble getting the audio working, so it's not... again, this is the curse of mine.

### Team Introduction

This is essentially us - there's three of us, and then this is our audio department here.

### Talk Structure

The talk is going to be divided into two parts:
1. **Tech stuff** - First part
2. **Design stuff** - Second part

I'm just going to say right now that the design part - there's probably not going to be enough time to go into too much detail about it, so it's going to be a bit more rambly.

## Part 1: The Tech

### Custom Engine: Falling Everything

For the tech, we have a custom engine that's called "Falling Everything."

As you can see from the trailer, we can do:
- Sand
- Liquids of all kinds
- Gases
- Rigid bodies

Essentially, I'm going to talk about how we've done all of that and how that is being accomplished.

### The Basics: Journey Back to the 90s

We're going to start at the basics by traveling back into a much better time period - into the 90s, when you could type into a text editor and characters would appear immediately.

#### Quick Basic Origins

Back in the 90s, I got started in Quick Basic. I discovered in Quick Basic that you can do this thing where you can:
- Put a pixel on the screen by `PSET`
- Ask for a color of a pixel by `POINT`

With this amazing technology, I was able to essentially write a sand simulation.

#### Sand Simulation Algorithm

What's happening here is:
1. It's testing if there's a sand pixel here
2. It's looking at the line below it
3. If that's empty, then move down
4. If that's occupied, it looks to the left and right (down-left and down-right) and moves in that direction

With that, you essentially get a rudimentary sand simulation. **This is like 95% of our tech.**

#### Water Simulation

I quickly discovered that you can also do water. It's essentially the same algorithm, but at the very end, you also check to the left and to the right, and if they're empty, you move in that direction.

So with that, all of it settles down, and now you've got water and sand.

#### Gas Simulation

Gases are just the inverse of this - it's not that complicated!

### Evolution: Bloody Zombies (2008)

Travel forward in time about 12 years. I'm using Visual C++ 6.0 (and I do not know how good my life is).

I ended up making this game called **Bloody Zombies**:
- Made in 2008
- Made for Gamma 256 (a competition where you had to make a game that used resolution 256×256 or lower)

While working on this, I was essentially using the same algorithm as in the Quick Basic one, but I discovered you can make it more liquid-like.

#### Particle System Enhancement

You can make it more liquid-like by adding another simulation - a particle simulation.

When the player jumps into blood and it splatters all around:
1. Take one of those pixels out of the falling sand simulation
2. Put it into a separate particle simulation
3. Track its velocity and gravity there
4. It travels until it hits another pixel in the world
5. Then it gets put back into the falling sand simulation

With this, you can make things a lot more liquid-like and not just like this blobby thing that's falling down.

**We're still using this technique to this day in Noita.**

### Adding Rigid Bodies

After Bloody Zombies, I made Crayon Physics. Then I was interested in: can I add rigid bodies to the simulation?

I was using Visual Studio C++ 2008, and it turns out you can add rigid bodies to this, but it's a bit more complicated.

#### Rigid Body Integration Process

I'm using Box2D for the rigid body stuff, and it's integrated with this falling sand simulation.

The way it works:
1. **Pixel Management**: All pixels that belong to a rigid body know their material and that they want to be one rigid body
2. **Marching Squares**: Apply a marching squares algorithm to produce an outline of where all the pixels are
3. **Simplification**: That's a lot of vertices, so give it a Douglas-Peucker algorithm to smooth that out
4. **Triangulation**: Once you have the smoothed mesh, give it to a triangulation algorithm to get triangles
5. **Box2D Integration**: Give those triangles to Box2D

Every pixel knows its UV coordinates inside of that triangle and which body it belongs to, so it can figure out its new position.

#### Combining the Simulations

To put these two simulations together:
1. At the beginning of the frame, take your rigid body pixels out of the world
2. Run your falling sand simulation one step
3. Run your Box2D one step
4. Put your rigid body pixels back into the world with updated positions from Box2D

If there's a pixel now in the way of a rigid body, take that pixel out and put it into the Bloody Zombies particle system - throw it into the air! This creates splashes when bodies hit water.

#### Additional Considerations

- If pixels get destroyed, you have to recalculate all the marching squares and Box2D body stuff
- For static stuff (nothing is really static in our game), use the first two steps of the algorithm (marching squares and Douglas-Peucker)
- Make hollow bodies of the world only where there are rigid bodies or where they're going to be

### Scaling Up: The Big World Challenge

Moving to Visual Studio 2013 (with an 8-minute boot time video that I'm making you suffer through because I had to suffer through it).

The last step was figuring out how to make a really big world where everything is simulated. The problem: it was taking too much CPU time to test all the pixels.

#### Solution: Multi-threading and Chunks

**Chunk System**:
- World divided into 64×64 chunks
- Each chunk keeps a dirty rect of things that need updating
- This alone removes many pixels that need testing (world often ends up in semi-stable state)

**Multi-threading Challenge**:
- Using the same buffer (no double buffering)
- Must ensure another thread isn't updating the same pixel
- If two threads try to update the same pixel, "all hell breaks loose"

**Checker Pattern Solution**:
1. Do four update rounds of the world
2. Gather all things that need updating
3. Pick every fourth chunk (in a checker pattern)
4. This guarantees any pixel being updated can go 32 pixels in any direction without another thread updating it
5. Throw chunks into thread pool, wait for completion
6. Repeat for all four sets

### World Streaming

For our continuous big world:
- Yellow rectangle represents the screen
- Other rectangles are 512×512 areas of the world
- Procedural generation creates new worlds as you travel
- Keep 12 areas in memory at once
- Write furthest one to disk
- Read back if you return there

This creates the feeling that the whole world is being simulated all the time, even though we're cleverly figuring out what needs updating.

## Part 2: The Design

The tech part has been the easier thing to work on. Game design has been more complicated and difficult.

### Why Design is Harder

For tech stuff, you can ask falsifiable questions:
- Does this look like water?
- Is it running at 60 FPS?
- Yes? It's done.

For game design:
- Is this game fun?
- Well, depends who's asking...
- I enjoy kicking ragdolls around for 8 hours a day, but I don't know if anyone else will enjoy that.

### The Core Question

**Does the physics simulation produce interesting gameplay?**

The answer is complicated.

#### The Naive Answer

My initial position: "Of course this is going to produce all this interesting gameplay! It's going to work as a perpetual motion machine of emergent gameplay!"

I was kind of right - there's a lot of emergent stuff that happens. But what I didn't realize...

#### The Two Colors of Emergent Gameplay

1. **The Cool and Interesting** - What everyone gets hyped about
2. **The Game-Breaking** - Can completely devastate your game

### Early Prototype Problems

The early prototype had a 2D building, but if you simulate everything, it ends up as a pile of rubble at the bottom of the screen. This is cool in a way, but how do you build a game when all your level design ends up as rubble?

Building a game on top of such a system is a nightmare.

### The Development Process

The process of making Noita has been:
- Trial and error
- Trying to tame this "emergent chaos beast" that is our physics engine
- Finding the game that maximizes good emergence while minimizing bad emergence

We've approached this by testing many different games and mechanics, because it's hard to know if something will be the coolest thing or break the game.

### Local Maxima Problem

Often we've made changes and ended at a "local maxima" - where the game was kind of fun, but we felt something much better was out there, and we didn't know how to get there.

#### Example: The Drill Tool

Earlier version had a drill tool that could dig through everything:
- **Good**: Kind of fun and interesting to explore the world
- **Bad**: Broke all combat gameplay (drill hole in wall, shoot enemies through tiny hole)

Solution:
1. Removed the drill completely
2. Game got worse temporarily (procedural generation was built assuming you could go through everything)
3. Fixed everything else
4. Ended up in a better position than we started

### The Roguelike Revolution

One major breakthrough was making it a roguelike (permadeath + procedural generation).

This helped us shift dealing with the emergent chaos:
1. **To the player** - The world's out to kill you, so you must pay attention
2. **To the random nature** - Can't build strategies around one tool

#### Benefits of Roguelike Design

- The drill made a comeback! Since the game is random, players can't build all strategies around finding this one tool
- Finding the drill and figuring out how to "break" combat becomes an exciting discovery moment
- Players think they're clever finding something designers "don't know about"

#### Permadeath Solutions

- Get a new world every time you play
- Previously (like Terraria), death meant stuff remained in world
- Problem: In our dynamic world, you could permanently block your progress (e.g., fill portal area with lava)
- With permadeath: If you block your progress, that's on you - get a new chance when you die

### Challenge and Attention

Making it challenging means players must pay attention to the world and physics because they're out to kill you.

#### Counter-example: God Mode Prototype

We tested playing as a god-like entity:
- Spam lightning everywhere
- Really fun for 20 minutes
- Problem: Physics simulation became just a visual effect rather than something to pay attention to

The roguelike approach gives us both:
- Early game: Pay careful attention to physics
- Late game: Become super powerful and destroy everything

### Why Not Roguelike from the Start?

Good reason: We thought it would be super annoying to die from physics glitches or unexplained physics interactions.

Our fixes:
1. Fix as many glitches as possible
2. If we can't fix a glitch, make it not kill the player
3. Example: Rigid bodies damage enemies but not the player (because they can get wobbly with huge forces)

### Communication is Key

The difference between players hating your permadeath game vs. blaming themselves is **communication of emergent systems**.

#### Example: Lava Death

**Bad**: Lava drops out of nowhere → Player thinks it's a glitch → Super pissed

**Good**: Player notices wooden plank with lava on top → Walks underneath → Plank catches fire → Lava drops → Player thinks "I wasn't careful enough"

#### Learning from NetHack

NetHack is excellent at communication - the game stops and displays what's happening in the world.

### The Power of UI Communication

When you add communication, players attribute more to the emergent system than might actually be there.

#### Liquid Staining System

Evolution:
1. Started purely visual (covered in blood = red)
2. Added mechanics (covered in oil = more likely to ignite)
3. Added UI icons showing which liquid you're covered in

Result: Players started attributing things that weren't implemented:
- "I'm wet, so I'll take more damage from electricity" (wasn't implemented initially)
- Shows the power of communication

#### Anecdote: The Broken Feature

- Being wet was supposed to make you less likely to ignite
- Feature broke and didn't work for 2 months
- Everyone assumed it was still working because the UI communicated it

Some games (Dwarf Fortress, The Sims) reach a level where players attribute more to emergent systems than actually exists.

## Questions & Answers

### Q: Why single-buffered simulation instead of double-buffered?

**A**: Originally, that's just how it started. Later realized with double buffering, you'd have to update everything unless you double-buffer each chunk separately. But then you maintain two buffers. For falling sand simulations, single buffering works better because pixels can only occupy one place.

### Q: How does procedural level generation work?

**A**: We've been working on this for 6-7 years and tried many things. The main approach uses Sean Barrett's Herringbone Wang tileset.

Unlike Spelunky (which uses 4×4 squares), Herringbone Wang uses brick shapes that interlock with certain connection rules. The benefit: you don't see seams as easily because there's no grid seam running through the whole world.

## Closing

Thank you for attending! If there are more questions, I can answer them outside.