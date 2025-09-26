# CrossForge - Development plan (Transition to ECS)

# Basic ECS proof of concept
- [x] Define basic ECS classes and test with simple prototypes 
- [x] Try to implement AssetIO as ECS, e.g. build an ECS system that features plug-in importer/exporter (works with import/export of duck object including materials/textures)

# Proof-of-concept for windows system and basic OpenGL rendering
- [ ] create system that takes window entities and manages them, i.e. create the window and handled messages
- [ ] create system that performs basic OpenGL rendering, i.e. rendering operations of the respective windows


# Start reimplementation of crossforge, i.e. create repository libcrossforge
- [ ] transfer created ECS classes so far to libcrossforge and integrate it into the main CrossForge repository as library
- [ ] Continue planning to move to new system