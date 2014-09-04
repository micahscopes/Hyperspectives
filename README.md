HYPERSPECTIVES
===============
This is going to be a collection of my experiments related to moebius 
transformations and interesting mappings of 3d geometry.  The goal is 
to offer unique perspectives on various aspects of what it means to be 
a creature living in living in space and to learn about math in the
process.

[Hyper Horse](http://hyperspectives.timbremill.net/moebius/hyperhorse.html)
---------------------------------------------------------------------------
### What's going on here?
The horse is inverse stereographically projected onto the hypersphere, 
rotated, and projected back again. Dragging the mouse moves the 
camera. Pressing the left & right arrow keys decrements and 
increments the rotation speed.

### How does it work?
[Daniel Piker’s blog posts][1] give a great overview of this stuff.  
Another big help is [Moebius Transformations
Revealed][2], which shows the 2 dimensional Moebius transformations.  
It really gives an idea of what's going on in a more familiar space.  
And of course Wikipedia articles: [Stereographic projection][3], [Möbius
transformation][4], [Isoclinic decomposition.][5]

  [1]: http://spacesymmetrystructure.wordpress.com/category/Inversion/
  [2]: http://www.youtube.com/watch?v=JX3VmDgiFnY
  [3]: http://wikipedia.org/wiki/Stereographic_projection
  [4]: http://wikipedia.org/wiki/Mobius_group
  [5]: http://en.wikipedia.org/wiki/Rotations_in_4-dimensional_Euclidean_space#Isoclinic_decomposition
  
### [Vertex Shader](http://hyperspectives.timbremill.net/shaders/moebiusVertex.glsl)
This GLSL code does the actual geometry work of the stereographic 
projection and 4 dimensional rotation.  You can see that it takes two 
four-dimensional vectors as input.  Those are the parameters for the 
[isoclinic rotation matrices][4], which should each have determinants 
of 1 since they are rotations.  So you have to be careful what you put 
in them!  I usually generate these from a normalized quaternion representing
the rotation of some dummy vector.

### [Fragment Shader](http://hyperspectives.timbremill.net/shaders/rgbFragment.glsl)
This basic one just shines red, green and blue lights along x, y and z respectively.

More Information
----------------
There's an amazing video series online called [Dimensions 
Math](http://www.dimensions-math.org/), check it out as soon as 
possible.
