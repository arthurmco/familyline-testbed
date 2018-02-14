# Attack rules

These are the object attack-defend rules in Tribalia

Every object has some properties regarding object attack and defense. Those
are:

 - attack points
 - defense points
 - attack range bonus
 - unit attack bonus
 - building attack bonus
 - unit defense bonus
 - building defense bonus
 - attack decay
 
 Attack points are simply the amount of damage inflicted on every tick.

 Defense points are the protection points from the damage said above.
 
 Taking into account only those two, to calculate the damage, just:
 
 ```c
 
	 defensor.hp = (defensor.hp - (attacker.atk - defensor.def))
	 
 ```
 
## Attack range bonus ##
 
 Attack range bonus is a multiplier over the attacker atk points. When he's
 below half the maximum range, the multiplier is 1. When he's between half
 the max range and the max range itself, the multiplier is between 1 and
 the attack range bonus.
 
 A simple illustration, supposing the atk range bonus is 2
 
 ```
 atk range multiplier   1             1     1.5      2
                       ||-------------|------|------||
 distance               0             4      6       8
 ```

The atk range multiplier can be lower than 1, for attacks that lose
strength on distance

This is done to mirror the arrow shot strength, when higher the distance,
the impact is stronger

 
## Unit & Building bonuses ##

The unit and building attack bonuses are attack multipliers when tke
attacker is facing an unit or a building. 

Generally the building attack bonus is low for units and high for siege
units, and the unit attack bonus is low for siege units, medium for
buildings and high for units themselves.

The unit and building defense bonuses are defense multipliers, for when the
defender is being attacked by an unit or a building


## Attack decay ##

The attack decay is a multiplier, generally low, applied recursively on
each tick, after the attack has been done, to the attacked unit. It's used
to simulate something like fire damage.

> TODO: Explain attack decay better
