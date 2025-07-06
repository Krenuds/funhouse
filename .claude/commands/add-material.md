# Add Material Command

Add a new material type named $ARGUMENTS to the falling sand engine:

1. Create material definition in materials module
2. Define physical properties:
   - Density
   - State (solid/liquid/gas/powder)
   - Flow behavior
   - Interaction rules with other materials
3. Implement update rules in simulation
4. Add rendering properties (color, texture)
5. Create basic tests for the material
6. Update material documentation

Ensure the new material follows existing patterns and integrates properly with the chunk system.