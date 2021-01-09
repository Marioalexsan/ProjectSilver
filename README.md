# ProjectSilver
 A small game written in C++ for the OOP class of Iasi's Technical University.

# Controls

Basic

* WASD - Move around
* Left Mouse Button - Shoot current Weapon
* R - Reload current Weapon
* 1 - Switch to Pistol
* 2 - Switch to Rifle

Advanced

* Right Mouse Button - Use Shield
* E - Melee attack
* Shift - Dash

# Gameplay tips

Health system

* Your Health regenerates if you don't take damage for a while!
* Once you lose all of your Health, you will start taking Max Health damage. You die once your Max Health reaches 0.
* If your Health and Max Health are equal, and Max Health is below 0, you will regen both Health and Max Health at the same time, albeit at a much slower rate.

Shielding

* The Shield has its own Health counter, and regenerates much faster than your character's Health.
* You can use the Shield to block damage from attacks coming from the front!
* If the Shield takes too much damage, it will break. Try to avoid doing that, as it will take a while for it to repair itself!
* If you block an attack just as it hits you, it will be Perfect Guarded, and damage will be reduced by 65%. Time your Shield use!
* If your Shield breaks, you will be immune to damage for a very short moment. Use that moment to get to safety!
* You can interrupt most of your animations (with th exception of Melee Attacks and Weapon Switches) by Shielding.

The Pistol

* The Pistol has decent damage, and a 12 round magazine.
* The ammo pool for the Pistol is infinite, so you don't need to worry about running out of it!
* You move slower while reloading. Try to stay behind cover while doing so!

The Assault Rifle
* The Assault Rifle has a 30 round magazine, a very fast fire rate, and deals slightly more damage than the Pistol.
* Unlike the Pistol, you have a limited Ammo Pool for the Assault Rifle. Use it wisely!
* You can get Rifle Ammo from Gun Turrets! It drops an Ammo Pack if it deactivates at a Round End, or two if destroyed by damage before that happens.
* You can have at most a total of 60 Rifle Bullets. Remember to use it if you're at max capacity!

The Axe
* You can do a Melee Attack with your Axe by pressing E. The Axe deals a LOT of damage, but it is risky to use.
* If you move while doing a Melee Attack, your character will try to do a lunge attack. This should help get close and personal with enemies!
* You can interrupt most of your animations (with th exception of Weapon Switches) with a Melee Attack.

Dashing

* Pressing Shift will cause you to Dash in your current movement direction.
* You can Dash at most two times in quick succession. The Dash's Charges ("Boost" counter) regenerate over time.
* You can dash during animations! It might help to avoid a bullet or two while reloading, or get to a better position for attacking!
