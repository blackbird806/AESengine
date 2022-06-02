Ce projet contient les structures d'accélération spatiale linear octree et BSP tree (dans le dossier src/spatial).
Ces structures ont étés testés avec un rendu sommaire visibles dans les fichiers test/tes_bsp.cpp et test/test_octree.cpp.


Certaines fonctions de collisions ont été implémentées en SIMD dans src/core/geometry.cpp, (voir égalment core/simd.hpp).

Un jobSystem et une queue circulaire thread safe sont implémentées dans src/core/jobSystem et src/core/circularQueue.hpp.

En bonus un BVH naif pour raytracer CPU : https://github.com/blackbird806/tinyraytracer/blob/acceleration-structure/BVH.cpp
