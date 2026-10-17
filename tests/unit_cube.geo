SetFactory("OpenCASCADE");
Box(1) = {0, 0, 0, 1, 1, 1};
Mesh.CharacteristicLengthMin = 0.4;
Mesh.CharacteristicLengthMax = 0.4;
Physical Surface("boundary", 13) = {1, 2, 3, 4, 5, 6};
Physical Volume("volume", 14) = {1};
