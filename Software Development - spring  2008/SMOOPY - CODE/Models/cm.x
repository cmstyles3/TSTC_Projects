xof 0303txt 0032
template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshNormals {
 <f6f23f43-7686-11cf-8f52-0040333594a3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template Coords2d {
 <f6f23f44-7686-11cf-8f52-0040333594a3>
 FLOAT u;
 FLOAT v;
}

template MeshTextureCoords {
 <f6f23f40-7686-11cf-8f52-0040333594a3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}


Mesh {
 309;
 3937.49939;2624.99917;2142.31539;,
 2273.31584;4546.6320;1181.49965;,
 1312.49971;4546.6320;2142.31539;,
 2273.31584;2624.99917;3806.49933;,
 0.000101;4546.6320;2493.99936;,
 0.000101;2624.99917;4415.6328;,
 -1312.49946;4546.6320;2142.31539;,
 -2273.31584;2624.99917;3806.49933;,
 -2273.31584;4546.6320;1181.49965;,
 -3937.49939;2624.99917;2142.31539;,
 4546.6320;0.000000;2493.99936;,
 2624.99917;0.000000;4415.6328;,
 0.000101;0.000000;5118.9984;,
 -2624.99917;0.000000;4415.6328;,
 -4546.6320;0.000000;2493.99936;,
 3937.49939;2624.99917;2142.31539;,
 1312.49971;4546.6320;2142.31539;,
 2273.31584;2624.99917;3806.49933;,
 1312.49971;4546.6320;2142.31539;,
 2273.31584;2624.99917;3806.49933;,
 0.000101;4546.6320;2493.99936;,
 0.000101;2624.99917;4415.6328;,
 0.000101;4546.6320;2493.99936;,
 0.000101;2624.99917;4415.6328;,
 -1312.49946;4546.6320;2142.31539;,
 -2273.31584;2624.99917;3806.49933;,
 -1312.49946;4546.6320;2142.31539;,
 -2273.31584;2624.99917;3806.49933;,
 -2273.31584;4546.6320;1181.49965;,
 2273.31584;2624.99917;3806.49933;,
 3937.49939;2624.99917;2142.31539;,
 2273.31584;2624.99917;3806.49933;,
 2624.99917;0.000000;4415.6328;,
 0.000101;2624.99917;4415.6328;,
 2273.31584;2624.99917;3806.49933;,
 0.000101;0.000000;5118.9984;,
 -2273.31584;2624.99917;3806.49933;,
 0.000101;2624.99917;4415.6328;,
 -2624.99917;0.000000;4415.6328;,
 -3937.49939;2624.99917;2142.31539;,
 -2273.31584;2624.99917;3806.49933;,
 -4546.6320;0.000000;2493.99936;,
 -2273.31584;-4546.6320;1181.49965;,
 -2624.99917;-4546.6320;-130.999987;,
 -2273.31584;-4546.6320;-1443.49965;,
 -1312.49946;-4546.6320;-2404.31667;,
 0.000101;-4546.6320;-2755.99923;,
 0.000101;0.000000;-5380.9984;,
 -5249.9992;0.000000;-130.999987;,
 -4546.6320;0.000000;-2755.99923;,
 -2624.99917;0.000000;-4677.6320;,
 0.000101;-2624.99917;-4677.6320;,
 -3937.49939;-2624.99917;2142.31539;,
 -4546.6320;-2624.99917;-130.999987;,
 -3937.49939;-2624.99917;-2404.31667;,
 -2273.31584;-2624.99917;-4068.49920;,
 -4546.6320;-2624.99917;-130.999987;,
 -5249.9992;0.000000;-130.999987;,
 -3937.49939;-2624.99917;-2404.31667;,
 -4546.6320;0.000000;-2755.99923;,
 -3937.49939;-2624.99917;-2404.31667;,
 -2273.31584;-2624.99917;-4068.49920;,
 -2624.99917;0.000000;-4677.6320;,
 -4546.6320;-2624.99917;-130.999987;,
 -3937.49939;-2624.99917;2142.31539;,
 -2273.31584;-4546.6320;1181.49965;,
 -4546.6320;-2624.99917;-130.999987;,
 -2624.99917;-4546.6320;-130.999987;,
 -3937.49939;-2624.99917;-2404.31667;,
 -4546.6320;-2624.99917;-130.999987;,
 -2624.99917;-4546.6320;-130.999987;,
 -3937.49939;-2624.99917;-2404.31667;,
 -2273.31584;-4546.6320;-1443.49965;,
 -2273.31584;-2624.99917;-4068.49920;,
 -3937.49939;-2624.99917;-2404.31667;,
 -2273.31584;-4546.6320;-1443.49965;,
 -1312.49946;-4546.6320;-2404.31667;,
 0.000101;-2624.99917;-4677.6320;,
 -2273.31584;-2624.99917;-4068.49920;,
 0.000101;-2624.99917;-4677.6320;,
 3937.49939;2624.99917;2142.31539;,
 2273.31584;4546.6320;1181.49965;,
 4546.6320;0.000000;2493.99936;,
 4546.6320;2624.99917;-130.999987;,
 2624.99917;4546.6320;-130.999987;,
 0.000101;2624.99917;-4677.6320;,
 0.000101;4546.6320;-2755.99923;,
 1312.49971;4546.6320;-2404.31667;,
 2273.31584;2624.99917;-4068.49920;,
 2273.31584;4546.6320;-1443.49965;,
 3937.49939;2624.99917;-2404.31667;,
 5249.9984;0.000000;-130.999987;,
 0.000101;0.000000;-5380.9984;,
 2624.99917;0.000000;-4677.6320;,
 4546.6320;0.000000;-2755.99923;,
 4546.6320;2624.99917;-130.999987;,
 2273.31584;4546.6320;1181.49965;,
 2273.31584;2624.99917;-4068.49920;,
 1312.49971;4546.6320;-2404.31667;,
 2273.31584;2624.99917;-4068.49920;,
 2273.31584;4546.6320;-1443.49965;,
 3937.49939;2624.99917;-2404.31667;,
 2624.99917;4546.6320;-130.999987;,
 2273.31584;4546.6320;-1443.49965;,
 3937.49939;2624.99917;-2404.31667;,
 4546.6320;2624.99917;-130.999987;,
 2624.99917;4546.6320;-130.999987;,
 3937.49939;2624.99917;2142.31539;,
 4546.6320;2624.99917;-130.999987;,
 2273.31584;2624.99917;-4068.49920;,
 0.000101;2624.99917;-4677.6320;,
 2624.99917;0.000000;-4677.6320;,
 3937.49939;2624.99917;-2404.31667;,
 2273.31584;2624.99917;-4068.49920;,
 4546.6320;0.000000;-2755.99923;,
 4546.6320;2624.99917;-130.999987;,
 3937.49939;2624.99917;-2404.31667;,
 5249.9984;0.000000;-130.999987;,
 -2273.31584;4546.6320;1181.49965;,
 -3937.49939;2624.99917;2142.31539;,
 -4546.6320;0.000000;2493.99936;,
 0.000101;2624.99917;-4677.6320;,
 0.000101;4546.6320;-2755.99923;,
 0.000101;0.000000;-5380.9984;,
 -2624.99917;4546.6320;-130.999987;,
 -4546.6320;2624.99917;-130.999987;,
 -2273.31584;4546.6320;-1443.49965;,
 -3937.49939;2624.99917;-2404.31667;,
 -1312.49946;4546.6320;-2404.31667;,
 -2273.31584;2624.99917;-4068.49920;,
 -5249.9992;0.000000;-130.999987;,
 -4546.6320;0.000000;-2755.99923;,
 -2624.99917;0.000000;-4677.6320;,
 -3937.49939;2624.99917;2142.31539;,
 -2624.99917;4546.6320;-130.999987;,
 -4546.6320;2624.99917;-130.999987;,
 -2624.99917;4546.6320;-130.999987;,
 -4546.6320;2624.99917;-130.999987;,
 -2273.31584;4546.6320;-1443.49965;,
 -3937.49939;2624.99917;-2404.31667;,
 -2273.31584;4546.6320;-1443.49965;,
 -3937.49939;2624.99917;-2404.31667;,
 -1312.49946;4546.6320;-2404.31667;,
 -2273.31584;2624.99917;-4068.49920;,
 -1312.49946;4546.6320;-2404.31667;,
 -2273.31584;2624.99917;-4068.49920;,
 0.000101;4546.6320;-2755.99923;,
 -4546.6320;2624.99917;-130.999987;,
 -3937.49939;2624.99917;2142.31539;,
 -5249.9992;0.000000;-130.999987;,
 -3937.49939;2624.99917;-2404.31667;,
 -4546.6320;2624.99917;-130.999987;,
 -4546.6320;0.000000;-2755.99923;,
 -2273.31584;2624.99917;-4068.49920;,
 -3937.49939;2624.99917;-2404.31667;,
 -2624.99917;0.000000;-4677.6320;,
 0.000101;2624.99917;-4677.6320;,
 -2273.31584;2624.99917;-4068.49920;,
 4546.6320;0.000000;2493.99936;,
 2624.99917;-4546.6320;-130.999987;,
 2273.31584;-4546.6320;1181.49965;,
 0.000101;-4546.6320;-2755.99923;,
 1312.49971;-4546.6320;-2404.31667;,
 2273.31584;-4546.6320;-1443.49965;,
 5249.9984;0.000000;-130.999987;,
 0.000101;0.000000;-5380.9984;,
 2624.99917;0.000000;-4677.6320;,
 4546.6320;0.000000;-2755.99923;,
 4546.6320;-2624.99917;-130.999987;,
 3937.49939;-2624.99917;2142.31539;,
 0.000101;-2624.99917;-4677.6320;,
 2273.31584;-2624.99917;-4068.49920;,
 3937.49939;-2624.99917;-2404.31667;,
 2273.31584;-2624.99917;-4068.49920;,
 2624.99917;0.000000;-4677.6320;,
 3937.49939;-2624.99917;-2404.31667;,
 5249.9984;0.000000;-130.999987;,
 4546.6320;0.000000;-2755.99923;,
 4546.6320;-2624.99917;-130.999987;,
 3937.49939;-2624.99917;2142.31539;,
 4546.6320;-2624.99917;-130.999987;,
 2624.99917;-4546.6320;-130.999987;,
 3937.49939;-2624.99917;2142.31539;,
 2273.31584;-2624.99917;-4068.49920;,
 0.000101;-2624.99917;-4677.6320;,
 2273.31584;-2624.99917;-4068.49920;,
 1312.49971;-4546.6320;-2404.31667;,
 3937.49939;-2624.99917;-2404.31667;,
 2273.31584;-2624.99917;-4068.49920;,
 2273.31584;-4546.6320;-1443.49965;,
 4546.6320;-2624.99917;-130.999987;,
 3937.49939;-2624.99917;-2404.31667;,
 2273.31584;-4546.6320;-1443.49965;,
 2624.99917;-4546.6320;-130.999987;,
 4546.6320;-2624.99917;-130.999987;,
 4546.6320;0.000000;2493.99936;,
 2624.99917;0.000000;4415.6328;,
 0.000101;0.000000;5118.9984;,
 -2624.99917;0.000000;4415.6328;,
 -4546.6320;0.000000;2493.99936;,
 2273.31584;-4546.6320;1181.49965;,
 1312.49971;-4546.6320;2142.31539;,
 0.000101;-4546.6320;2493.99936;,
 -1312.49946;-4546.6320;2142.31539;,
 -2273.31584;-4546.6320;1181.49965;,
 3937.49939;-2624.99917;2142.31539;,
 2273.31584;-2624.99917;3806.49933;,
 0.000101;-2624.99917;4415.6328;,
 -2273.31584;-2624.99917;3806.49933;,
 -3937.49939;-2624.99917;2142.31539;,
 3937.49939;-2624.99917;2142.31539;,
 2624.99917;0.000000;4415.6328;,
 2273.31584;-2624.99917;3806.49933;,
 2624.99917;0.000000;4415.6328;,
 0.000101;-2624.99917;4415.6328;,
 0.000101;0.000000;5118.9984;,
 -2273.31584;-2624.99917;3806.49933;,
 -2624.99917;0.000000;4415.6328;,
 2273.31584;-2624.99917;3806.49933;,
 3937.49939;-2624.99917;2142.31539;,
 2273.31584;-4546.6320;1181.49965;,
 2273.31584;-2624.99917;3806.49933;,
 1312.49971;-4546.6320;2142.31539;,
 0.000101;-2624.99917;4415.6328;,
 2273.31584;-2624.99917;3806.49933;,
 1312.49971;-4546.6320;2142.31539;,
 0.000101;-2624.99917;4415.6328;,
 0.000101;-4546.6320;2493.99936;,
 -2273.31584;-2624.99917;3806.49933;,
 0.000101;-2624.99917;4415.6328;,
 0.000101;-4546.6320;2493.99936;,
 -2273.31584;-2624.99917;3806.49933;,
 -1312.49946;-4546.6320;2142.31539;,
 -3937.49939;-2624.99917;2142.31539;,
 -2273.31584;-2624.99917;3806.49933;,
 -1312.49946;-4546.6320;2142.31539;,
 -3937.49939;-2624.99917;2142.31539;,
 0.000101;-5249.9984;-130.999987;,
 2624.99917;-4546.6320;-130.999987;,
 2273.31584;-4546.6320;1181.49965;,
 1312.49971;-4546.6320;2142.31539;,
 0.000101;-4546.6320;2493.99936;,
 -1312.49946;-4546.6320;2142.31539;,
 -2273.31584;-4546.6320;1181.49965;,
 -2624.99917;-4546.6320;-130.999987;,
 -2273.31584;-4546.6320;-1443.49965;,
 -1312.49946;-4546.6320;-2404.31667;,
 0.000101;-4546.6320;-2755.99923;,
 1312.49971;-4546.6320;-2404.31667;,
 2273.31584;-4546.6320;-1443.49965;,
 0.000101;-5249.9984;-130.999987;,
 2273.31584;-4546.6320;1181.49965;,
 0.000101;-5249.9984;-130.999987;,
 1312.49971;-4546.6320;2142.31539;,
 0.000101;-5249.9984;-130.999987;,
 0.000101;-4546.6320;2493.99936;,
 0.000101;-5249.9984;-130.999987;,
 -1312.49946;-4546.6320;2142.31539;,
 0.000101;-5249.9984;-130.999987;,
 -2273.31584;-4546.6320;1181.49965;,
 0.000101;-5249.9984;-130.999987;,
 -2624.99917;-4546.6320;-130.999987;,
 0.000101;-5249.9984;-130.999987;,
 -2273.31584;-4546.6320;-1443.49965;,
 0.000101;-5249.9984;-130.999987;,
 -1312.49946;-4546.6320;-2404.31667;,
 0.000101;-5249.9984;-130.999987;,
 0.000101;-4546.6320;-2755.99923;,
 0.000101;-5249.9984;-130.999987;,
 1312.49971;-4546.6320;-2404.31667;,
 0.000101;-5249.9984;-130.999987;,
 2624.99917;-4546.6320;-130.999987;,
 2273.31584;-4546.6320;-1443.49965;,
 2273.31584;4546.6320;1181.49965;,
 1312.49971;4546.6320;2142.31539;,
 0.000101;4546.6320;2493.99936;,
 -1312.49946;4546.6320;2142.31539;,
 -2273.31584;4546.6320;1181.49965;,
 2624.99917;4546.6320;-130.999987;,
 0.000101;4546.6320;-2755.99923;,
 1312.49971;4546.6320;-2404.31667;,
 2273.31584;4546.6320;-1443.49965;,
 -2624.99917;4546.6320;-130.999987;,
 -2273.31584;4546.6320;-1443.49965;,
 -1312.49946;4546.6320;-2404.31667;,
 0.000101;5249.9984;-130.999987;,
 0.000101;5249.9984;-130.999987;,
 2273.31584;4546.6320;1181.49965;,
 0.000101;5249.9984;-130.999987;,
 1312.49971;4546.6320;2142.31539;,
 0.000101;5249.9984;-130.999987;,
 0.000101;4546.6320;2493.99936;,
 0.000101;5249.9984;-130.999987;,
 -1312.49946;4546.6320;2142.31539;,
 0.000101;5249.9984;-130.999987;,
 -2273.31584;4546.6320;1181.49965;,
 0.000101;5249.9984;-130.999987;,
 -2624.99917;4546.6320;-130.999987;,
 0.000101;5249.9984;-130.999987;,
 -2273.31584;4546.6320;-1443.49965;,
 0.000101;5249.9984;-130.999987;,
 -1312.49946;4546.6320;-2404.31667;,
 0.000101;5249.9984;-130.999987;,
 0.000101;4546.6320;-2755.99923;,
 0.000101;5249.9984;-130.999987;,
 1312.49971;4546.6320;-2404.31667;,
 0.000101;5249.9984;-130.999987;,
 2273.31584;4546.6320;-1443.49965;,
 2624.99917;4546.6320;-130.999987;;
 120;
 3;0,2,1;,
 3;15,3,16;,
 3;17,4,18;,
 3;19,5,20;,
 3;21,6,22;,
 3;23,7,24;,
 3;25,8,26;,
 3;27,9,28;,
 3;10,29,30;,
 3;10,11,31;,
 3;32,33,34;,
 3;32,12,33;,
 3;35,36,37;,
 3;35,13,36;,
 3;38,39,40;,
 3;38,14,39;,
 3;52,48,41;,
 3;52,53,48;,
 3;56,49,57;,
 3;56,54,49;,
 3;58,50,59;,
 3;60,55,50;,
 3;61,47,62;,
 3;61,51,47;,
 3;42,63,64;,
 3;65,43,66;,
 3;67,68,69;,
 3;70,44,71;,
 3;72,73,74;,
 3;75,45,73;,
 3;76,77,78;,
 3;76,46,79;,
 3;83,81,84;,
 3;95,80,96;,
 3;85,87,86;,
 3;85,88,87;,
 3;97,89,98;,
 3;99,90,100;,
 3;101,102,103;,
 3;104,105,106;,
 3;91,107,108;,
 3;91,82,107;,
 3;92,109,110;,
 3;92,93,109;,
 3;111,112,113;,
 3;111,94,112;,
 3;114,115,116;,
 3;114,117,115;,
 3;119,124,118;,
 3;133,125,134;,
 3;135,126,136;,
 3;137,127,138;,
 3;139,128,140;,
 3;141,129,142;,
 3;143,122,144;,
 3;145,121,146;,
 3;120,147,148;,
 3;120,130,147;,
 3;149,150,151;,
 3;149,131,150;,
 3;152,153,154;,
 3;152,132,153;,
 3;155,156,157;,
 3;155,123,156;,
 3;168,158,164;,
 3;168,169,158;,
 3;170,166,165;,
 3;170,171,166;,
 3;173,167,174;,
 3;173,172,167;,
 3;175,176,177;,
 3;175,178,176;,
 3;159,179,180;,
 3;181,160,182;,
 3;161,183,184;,
 3;161,162,185;,
 3;186,187,188;,
 3;186,163,187;,
 3;189,190,191;,
 3;192,193,194;,
 3;205,196,195;,
 3;210,206,211;,
 3;212,197,213;,
 3;212,207,197;,
 3;214,198,215;,
 3;214,208,198;,
 3;216,199,217;,
 3;216,209,199;,
 3;200,218,219;,
 3;220,201,221;,
 3;222,223,224;,
 3;225,202,226;,
 3;227,228,229;,
 3;230,203,231;,
 3;232,233,234;,
 3;235,204,236;,
 3;237,239,238;,
 3;250,240,251;,
 3;252,241,253;,
 3;254,242,255;,
 3;256,243,257;,
 3;258,244,259;,
 3;260,245,261;,
 3;262,246,263;,
 3;264,247,265;,
 3;266,248,267;,
 3;268,249,269;,
 3;270,271,272;,
 3;285,278,273;,
 3;286,287,274;,
 3;288,289,275;,
 3;290,291,276;,
 3;292,293,277;,
 3;294,295,282;,
 3;296,297,283;,
 3;298,299,284;,
 3;300,301,279;,
 3;302,303,280;,
 3;304,305,281;,
 3;306,307,308;;

 MeshNormals {
  309;
  -0.508590;-0.694746;-0.508590;,
  -0.508590;-0.694746;-0.508590;,
  -0.508590;-0.694746;-0.508590;,
  -0.508590;-0.694747;-0.508590;,
  -0.186157;-0.694747;-0.694746;,
  -0.186157;-0.694747;-0.694747;,
  0.186157;-0.694747;-0.694747;,
  0.186157;-0.694747;-0.694746;,
  0.508590;-0.694747;-0.508590;,
  0.508590;-0.694746;-0.508590;,
  -0.684550;-0.250563;-0.684550;,
  -0.684550;-0.250563;-0.684550;,
  -0.250563;-0.250563;-0.935113;,
  0.250563;-0.250563;-0.935113;,
  0.684550;-0.250563;-0.684550;,
  -0.508590;-0.694747;-0.508590;,
  -0.508590;-0.694747;-0.508590;,
  -0.186157;-0.694747;-0.694746;,
  -0.186157;-0.694747;-0.694746;,
  -0.186157;-0.694747;-0.694747;,
  -0.186157;-0.694747;-0.694747;,
  0.186157;-0.694747;-0.694747;,
  0.186157;-0.694747;-0.694747;,
  0.186157;-0.694747;-0.694746;,
  0.186157;-0.694747;-0.694746;,
  0.508590;-0.694747;-0.508590;,
  0.508590;-0.694747;-0.508590;,
  0.508590;-0.694746;-0.508590;,
  0.508590;-0.694746;-0.508590;,
  -0.684551;-0.250563;-0.684550;,
  -0.684551;-0.250563;-0.684550;,
  -0.684550;-0.250563;-0.684550;,
  -0.250563;-0.250563;-0.935113;,
  -0.250563;-0.250563;-0.935113;,
  -0.250563;-0.250563;-0.935113;,
  0.250563;-0.250563;-0.935113;,
  0.250563;-0.250563;-0.935113;,
  0.250563;-0.250563;-0.935113;,
  0.684550;-0.250563;-0.684550;,
  0.684550;-0.250563;-0.684550;,
  0.684550;-0.250563;-0.684550;,
  0.935113;0.250563;-0.250563;,
  0.694747;0.694747;-0.186157;,
  0.694747;0.694746;-0.186157;,
  0.694747;0.694747;0.186157;,
  0.508590;0.694746;0.508590;,
  0.186157;0.694747;0.694747;,
  0.250563;0.250563;0.935113;,
  0.935113;0.250563;-0.250563;,
  0.935113;0.250563;0.250563;,
  0.684550;0.250563;0.684550;,
  0.250563;0.250563;0.935113;,
  0.935113;0.250563;-0.250563;,
  0.935113;0.250563;-0.250563;,
  0.935113;0.250563;0.250563;,
  0.684550;0.250563;0.684550;,
  0.935113;0.250563;0.250563;,
  0.935113;0.250563;0.250563;,
  0.684551;0.250563;0.684550;,
  0.684550;0.250563;0.684550;,
  0.684550;0.250563;0.684550;,
  0.250563;0.250563;0.935113;,
  0.250563;0.250563;0.935113;,
  0.694747;0.694747;-0.186157;,
  0.694747;0.694747;-0.186157;,
  0.694747;0.694746;-0.186157;,
  0.694747;0.694746;-0.186157;,
  0.694747;0.694746;0.186157;,
  0.694747;0.694746;0.186157;,
  0.694747;0.694746;0.186157;,
  0.694747;0.694747;0.186157;,
  0.694747;0.694747;0.186157;,
  0.508590;0.694747;0.508590;,
  0.508590;0.694747;0.508590;,
  0.508590;0.694747;0.508590;,
  0.508590;0.694746;0.508590;,
  0.186157;0.694747;0.694747;,
  0.186157;0.694746;0.694747;,
  0.186157;0.694747;0.694747;,
  0.186157;0.694747;0.694747;,
  -0.694747;-0.694746;-0.186157;,
  -0.694747;-0.694747;-0.186157;,
  -0.935113;-0.250563;-0.250563;,
  -0.694747;-0.694747;-0.186156;,
  -0.694747;-0.694747;-0.186157;,
  -0.186157;-0.694747;0.694747;,
  -0.186157;-0.694747;0.694747;,
  -0.186157;-0.694747;0.694747;,
  -0.186157;-0.694747;0.694747;,
  -0.508590;-0.694747;0.508590;,
  -0.508590;-0.694746;0.508590;,
  -0.935113;-0.250563;-0.250563;,
  -0.250563;-0.250563;0.935113;,
  -0.250563;-0.250563;0.935113;,
  -0.684550;-0.250563;0.684550;,
  -0.694747;-0.694746;-0.186157;,
  -0.694747;-0.694746;-0.186157;,
  -0.508590;-0.694747;0.508590;,
  -0.508590;-0.694747;0.508590;,
  -0.508590;-0.694746;0.508590;,
  -0.508590;-0.694746;0.508590;,
  -0.694747;-0.694746;0.186157;,
  -0.694747;-0.694746;0.186157;,
  -0.694747;-0.694746;0.186157;,
  -0.694747;-0.694747;0.186157;,
  -0.694747;-0.694747;0.186157;,
  -0.694747;-0.694747;0.186157;,
  -0.935113;-0.250563;-0.250563;,
  -0.935113;-0.250563;-0.250563;,
  -0.250563;-0.250563;0.935113;,
  -0.250563;-0.250563;0.935113;,
  -0.684550;-0.250563;0.684550;,
  -0.684550;-0.250563;0.684550;,
  -0.684550;-0.250563;0.684550;,
  -0.935113;-0.250563;0.250563;,
  -0.935113;-0.250563;0.250563;,
  -0.935113;-0.250563;0.250563;,
  -0.935113;-0.250563;0.250563;,
  0.694747;-0.694747;-0.186157;,
  0.694747;-0.694747;-0.186157;,
  0.935113;-0.250563;-0.250563;,
  0.186157;-0.694747;0.694747;,
  0.186157;-0.694746;0.694747;,
  0.250563;-0.250563;0.935113;,
  0.694747;-0.694747;-0.186157;,
  0.694747;-0.694746;-0.186157;,
  0.694747;-0.694746;0.186157;,
  0.694747;-0.694747;0.186157;,
  0.508590;-0.694747;0.508590;,
  0.508590;-0.694746;0.508590;,
  0.935113;-0.250563;-0.250563;,
  0.935113;-0.250563;0.250563;,
  0.684550;-0.250563;0.684550;,
  0.694747;-0.694746;-0.186157;,
  0.694747;-0.694746;-0.186157;,
  0.694747;-0.694746;0.186157;,
  0.694747;-0.694746;0.186157;,
  0.694747;-0.694747;0.186157;,
  0.694747;-0.694747;0.186157;,
  0.508590;-0.694747;0.508590;,
  0.508590;-0.694747;0.508590;,
  0.508590;-0.694746;0.508590;,
  0.508590;-0.694746;0.508590;,
  0.186157;-0.694746;0.694747;,
  0.186157;-0.694746;0.694747;,
  0.186157;-0.694747;0.694747;,
  0.186157;-0.694747;0.694747;,
  0.935113;-0.250563;-0.250563;,
  0.935113;-0.250563;-0.250563;,
  0.935113;-0.250563;0.250563;,
  0.935113;-0.250563;0.250563;,
  0.935113;-0.250563;0.250563;,
  0.684550;-0.250563;0.684550;,
  0.684550;-0.250563;0.684550;,
  0.684550;-0.250563;0.684550;,
  0.250563;-0.250563;0.935113;,
  0.250563;-0.250563;0.935113;,
  0.250563;-0.250563;0.935113;,
  -0.935113;0.250563;-0.250563;,
  -0.694746;0.694747;-0.186157;,
  -0.694747;0.694746;-0.186157;,
  -0.186157;0.694747;0.694747;,
  -0.186157;0.694747;0.694747;,
  -0.508590;0.694747;0.508590;,
  -0.935113;0.250563;-0.250563;,
  -0.250563;0.250563;0.935113;,
  -0.250563;0.250563;0.935113;,
  -0.684550;0.250563;0.684550;,
  -0.935113;0.250563;-0.250563;,
  -0.935113;0.250563;-0.250563;,
  -0.250563;0.250563;0.935113;,
  -0.250563;0.250563;0.935113;,
  -0.684550;0.250563;0.684550;,
  -0.684550;0.250563;0.684550;,
  -0.684550;0.250563;0.684550;,
  -0.935113;0.250563;0.250563;,
  -0.935113;0.250563;0.250563;,
  -0.935113;0.250563;0.250563;,
  -0.935113;0.250563;0.250563;,
  -0.694746;0.694747;-0.186157;,
  -0.694746;0.694747;-0.186157;,
  -0.694747;0.694746;-0.186157;,
  -0.694747;0.694746;-0.186157;,
  -0.186157;0.694747;0.694747;,
  -0.186157;0.694747;0.694747;,
  -0.186157;0.694746;0.694747;,
  -0.508590;0.694747;0.508590;,
  -0.508590;0.694747;0.508590;,
  -0.508590;0.694746;0.508590;,
  -0.694747;0.694746;0.186157;,
  -0.694747;0.694746;0.186157;,
  -0.694747;0.694746;0.186157;,
  -0.694747;0.694747;0.186157;,
  -0.694747;0.694747;0.186157;,
  -0.694747;0.694747;0.186157;,
  -0.684550;0.250563;-0.684550;,
  -0.684550;0.250563;-0.684550;,
  -0.250563;0.250563;-0.935113;,
  0.250563;0.250563;-0.935113;,
  0.684550;0.250563;-0.684550;,
  -0.508590;0.694746;-0.508590;,
  -0.508590;0.694747;-0.508590;,
  -0.186157;0.694746;-0.694747;,
  0.186157;0.694747;-0.694746;,
  0.508590;0.694746;-0.508590;,
  -0.684550;0.250563;-0.684550;,
  -0.684551;0.250563;-0.684550;,
  -0.250563;0.250563;-0.935113;,
  0.250563;0.250563;-0.935113;,
  0.684550;0.250563;-0.684550;,
  -0.684551;0.250563;-0.684550;,
  -0.684551;0.250563;-0.684550;,
  -0.250563;0.250563;-0.935113;,
  -0.250563;0.250563;-0.935113;,
  0.250563;0.250563;-0.935113;,
  0.250563;0.250563;-0.935113;,
  0.684550;0.250563;-0.684550;,
  0.684550;0.250563;-0.684550;,
  -0.508590;0.694746;-0.508590;,
  -0.508590;0.694746;-0.508590;,
  -0.508590;0.694747;-0.508590;,
  -0.508590;0.694747;-0.508590;,
  -0.186157;0.694747;-0.694746;,
  -0.186157;0.694747;-0.694746;,
  -0.186157;0.694747;-0.694747;,
  -0.186157;0.694747;-0.694747;,
  -0.186157;0.694746;-0.694747;,
  0.186157;0.694747;-0.694747;,
  0.186157;0.694747;-0.694747;,
  0.186157;0.694747;-0.694747;,
  0.186157;0.694747;-0.694746;,
  0.186157;0.694747;-0.694746;,
  0.508590;0.694747;-0.508590;,
  0.508590;0.694747;-0.508590;,
  0.508590;0.694747;-0.508590;,
  0.508590;0.694746;-0.508590;,
  0.508590;0.694746;-0.508590;,
  -0.258199;0.963611;-0.069184;,
  -0.258199;0.963611;-0.069184;,
  -0.258199;0.963611;-0.069184;,
  -0.189015;0.963611;-0.189015;,
  -0.069184;0.963611;-0.258199;,
  0.069184;0.963611;-0.258199;,
  0.189015;0.963611;-0.189015;,
  0.258199;0.963611;-0.069184;,
  0.258199;0.963611;0.069184;,
  0.189015;0.963611;0.189015;,
  0.069184;0.963611;0.258199;,
  -0.069184;0.963611;0.258199;,
  -0.189015;0.963611;0.189015;,
  -0.189015;0.963611;-0.189015;,
  -0.189015;0.963611;-0.189015;,
  -0.069184;0.963611;-0.258199;,
  -0.069184;0.963611;-0.258199;,
  0.069184;0.963611;-0.258199;,
  0.069184;0.963611;-0.258199;,
  0.189015;0.963611;-0.189015;,
  0.189015;0.963611;-0.189015;,
  0.258199;0.963611;-0.069184;,
  0.258199;0.963611;-0.069184;,
  0.258199;0.963611;0.069184;,
  0.258199;0.963611;0.069184;,
  0.189015;0.963611;0.189015;,
  0.189015;0.963611;0.189015;,
  0.069184;0.963611;0.258199;,
  0.069184;0.963611;0.258199;,
  -0.069184;0.963611;0.258199;,
  -0.069184;0.963611;0.258199;,
  -0.189015;0.963611;0.189015;,
  -0.189015;0.963611;0.189015;,
  -0.258199;0.963611;0.069184;,
  -0.258199;0.963611;0.069184;,
  -0.258199;0.963611;0.069184;,
  -0.258199;-0.963611;-0.069184;,
  -0.189015;-0.963611;-0.189015;,
  -0.069184;-0.963611;-0.258199;,
  0.069184;-0.963611;-0.258199;,
  0.189015;-0.963611;-0.189015;,
  -0.258199;-0.963611;-0.069184;,
  0.069184;-0.963611;0.258199;,
  -0.069184;-0.963611;0.258199;,
  -0.189015;-0.963611;0.189015;,
  0.258199;-0.963611;-0.069184;,
  0.258199;-0.963611;0.069184;,
  0.189015;-0.963611;0.189015;,
  -0.258199;-0.963611;-0.069184;,
  -0.189015;-0.963611;-0.189015;,
  -0.189015;-0.963611;-0.189015;,
  -0.069184;-0.963611;-0.258199;,
  -0.069184;-0.963611;-0.258199;,
  0.069184;-0.963611;-0.258199;,
  0.069184;-0.963611;-0.258199;,
  0.189015;-0.963611;-0.189015;,
  0.189015;-0.963611;-0.189015;,
  0.258199;-0.963611;-0.069184;,
  0.258199;-0.963611;-0.069184;,
  0.258199;-0.963611;0.069184;,
  0.258199;-0.963611;0.069184;,
  0.189015;-0.963611;0.189015;,
  0.189015;-0.963611;0.189015;,
  0.069184;-0.963611;0.258199;,
  0.069184;-0.963611;0.258199;,
  -0.069184;-0.963611;0.258199;,
  -0.069184;-0.963611;0.258199;,
  -0.189015;-0.963611;0.189015;,
  -0.189015;-0.963611;0.189015;,
  -0.258199;-0.963611;0.069184;,
  -0.258199;-0.963611;0.069184;,
  -0.258199;-0.963611;0.069184;;
  120;
  3;0,2,1;,
  3;15,3,16;,
  3;17,4,18;,
  3;19,5,20;,
  3;21,6,22;,
  3;23,7,24;,
  3;25,8,26;,
  3;27,9,28;,
  3;10,29,30;,
  3;10,11,31;,
  3;32,33,34;,
  3;32,12,33;,
  3;35,36,37;,
  3;35,13,36;,
  3;38,39,40;,
  3;38,14,39;,
  3;52,48,41;,
  3;52,53,48;,
  3;56,49,57;,
  3;56,54,49;,
  3;58,50,59;,
  3;60,55,50;,
  3;61,47,62;,
  3;61,51,47;,
  3;42,63,64;,
  3;65,43,66;,
  3;67,68,69;,
  3;70,44,71;,
  3;72,73,74;,
  3;75,45,73;,
  3;76,77,78;,
  3;76,46,79;,
  3;83,81,84;,
  3;95,80,96;,
  3;85,87,86;,
  3;85,88,87;,
  3;97,89,98;,
  3;99,90,100;,
  3;101,102,103;,
  3;104,105,106;,
  3;91,107,108;,
  3;91,82,107;,
  3;92,109,110;,
  3;92,93,109;,
  3;111,112,113;,
  3;111,94,112;,
  3;114,115,116;,
  3;114,117,115;,
  3;119,124,118;,
  3;133,125,134;,
  3;135,126,136;,
  3;137,127,138;,
  3;139,128,140;,
  3;141,129,142;,
  3;143,122,144;,
  3;145,121,146;,
  3;120,147,148;,
  3;120,130,147;,
  3;149,150,151;,
  3;149,131,150;,
  3;152,153,154;,
  3;152,132,153;,
  3;155,156,157;,
  3;155,123,156;,
  3;168,158,164;,
  3;168,169,158;,
  3;170,166,165;,
  3;170,171,166;,
  3;173,167,174;,
  3;173,172,167;,
  3;175,176,177;,
  3;175,178,176;,
  3;159,179,180;,
  3;181,160,182;,
  3;161,183,184;,
  3;161,162,185;,
  3;186,187,188;,
  3;186,163,187;,
  3;189,190,191;,
  3;192,193,194;,
  3;205,196,195;,
  3;210,206,211;,
  3;212,197,213;,
  3;212,207,197;,
  3;214,198,215;,
  3;214,208,198;,
  3;216,199,217;,
  3;216,209,199;,
  3;200,218,219;,
  3;220,201,221;,
  3;222,223,224;,
  3;225,202,226;,
  3;227,228,229;,
  3;230,203,231;,
  3;232,233,234;,
  3;235,204,236;,
  3;237,239,238;,
  3;250,240,251;,
  3;252,241,253;,
  3;254,242,255;,
  3;256,243,257;,
  3;258,244,259;,
  3;260,245,261;,
  3;262,246,263;,
  3;264,247,265;,
  3;266,248,267;,
  3;268,249,269;,
  3;270,271,272;,
  3;285,278,273;,
  3;286,287,274;,
  3;288,289,275;,
  3;290,291,276;,
  3;292,293,277;,
  3;294,295,282;,
  3;296,297,283;,
  3;298,299,284;,
  3;300,301,279;,
  3;302,303,280;,
  3;304,305,281;,
  3;306,307,308;;
 }

 MeshTextureCoords {
  309;
  0.999887;0.500000;,
  1.000000;0.000499;,
  0.750000;0.000000;,
  0.750000;0.500000;,
  0.500000;0.000499;,
  0.500000;0.500000;,
  0.250000;0.000499;,
  0.250000;0.500000;,
  0.000000;0.000499;,
  0.000000;0.500000;,
  0.999500;0.999501;,
  0.750000;0.999501;,
  0.500000;0.999501;,
  0.250000;0.999501;,
  0.000499;0.999501;,
  0.999887;0.500000;,
  0.750000;0.000000;,
  0.750000;0.500000;,
  0.750000;0.000000;,
  0.750000;0.500000;,
  0.500000;0.000499;,
  0.500000;0.500000;,
  0.500000;0.000499;,
  0.500000;0.500000;,
  0.250000;0.000499;,
  0.250000;0.500000;,
  0.250000;0.000499;,
  0.250000;0.500000;,
  0.000000;0.000499;,
  0.750000;0.500000;,
  0.999887;0.500000;,
  0.750000;0.500000;,
  0.750000;0.999501;,
  0.500000;0.500000;,
  0.750000;0.500000;,
  0.500000;0.999501;,
  0.250000;0.500000;,
  0.500000;0.500000;,
  0.250000;0.999501;,
  0.000000;0.500000;,
  0.250000;0.500000;,
  1.000000;0.000499;,
  0.999500;0.999501;,
  0.750000;0.999501;,
  0.500000;0.999501;,
  0.250000;0.999501;,
  0.000500;0.999501;,
  0.000000;0.000499;,
  0.750000;0.000000;,
  0.500000;0.000499;,
  0.250000;0.000499;,
  0.000000;0.500000;,
  0.999887;0.500000;,
  0.750000;0.500000;,
  0.500000;0.500000;,
  0.250000;0.500000;,
  0.750000;0.500000;,
  0.750000;0.000000;,
  0.500000;0.500000;,
  0.500000;0.000499;,
  0.500000;0.500000;,
  0.250000;0.500000;,
  0.250000;0.000499;,
  0.750000;0.500000;,
  0.999887;0.500000;,
  0.999500;0.999501;,
  0.750000;0.500000;,
  0.750000;0.999501;,
  0.500000;0.500000;,
  0.750000;0.500000;,
  0.750000;0.999501;,
  0.500000;0.500000;,
  0.500000;0.999501;,
  0.250000;0.500000;,
  0.500000;0.500000;,
  0.500000;0.999501;,
  0.250000;0.999501;,
  0.000000;0.500000;,
  0.250000;0.500000;,
  0.000000;0.500000;,
  0.000000;0.500000;,
  0.000000;0.000499;,
  0.000000;0.999500;,
  0.250000;0.500000;,
  0.250000;0.000499;,
  1.000000;0.500000;,
  1.000000;0.000000;,
  0.750000;0.000500;,
  0.750000;0.500000;,
  0.500000;0.000499;,
  0.500000;0.500000;,
  0.250000;1.000000;,
  1.000000;0.999501;,
  0.750000;0.999501;,
  0.500000;0.999501;,
  0.250000;0.500000;,
  0.000000;0.000499;,
  0.750000;0.500000;,
  0.750000;0.000500;,
  0.750000;0.500000;,
  0.500000;0.000499;,
  0.500000;0.500000;,
  0.250000;0.000499;,
  0.500000;0.000499;,
  0.500000;0.500000;,
  0.250000;0.500000;,
  0.250000;0.000499;,
  0.000000;0.500000;,
  0.250000;0.500000;,
  0.750000;0.500000;,
  1.000000;0.500000;,
  0.750000;0.999501;,
  0.500000;0.500000;,
  0.750000;0.500000;,
  0.500000;0.999501;,
  0.250000;0.500000;,
  0.500000;0.500000;,
  0.250000;1.000000;,
  1.000000;0.000499;,
  0.999887;0.500000;,
  0.999500;0.999501;,
  0.000000;0.500000;,
  0.000000;0.000499;,
  0.000500;0.999501;,
  0.750000;0.000000;,
  0.750000;0.500000;,
  0.500000;0.000499;,
  0.500000;0.500000;,
  0.250000;0.000499;,
  0.250000;0.500000;,
  0.750000;0.999501;,
  0.500000;0.999501;,
  0.250000;0.999501;,
  0.999887;0.500000;,
  0.750000;0.000000;,
  0.750000;0.500000;,
  0.750000;0.000000;,
  0.750000;0.500000;,
  0.500000;0.000499;,
  0.500000;0.500000;,
  0.500000;0.000499;,
  0.500000;0.500000;,
  0.250000;0.000499;,
  0.250000;0.500000;,
  0.250000;0.000499;,
  0.250000;0.500000;,
  0.000000;0.000499;,
  0.750000;0.500000;,
  0.999887;0.500000;,
  0.750000;0.999501;,
  0.500000;0.500000;,
  0.750000;0.500000;,
  0.500000;0.999501;,
  0.250000;0.500000;,
  0.500000;0.500000;,
  0.250000;0.999501;,
  0.000000;0.500000;,
  0.250000;0.500000;,
  0.000000;0.000499;,
  0.250000;1.000000;,
  0.000000;0.999500;,
  1.000000;0.999501;,
  0.750000;0.999501;,
  0.500000;0.999501;,
  0.250000;0.000499;,
  1.000000;0.000000;,
  0.750000;0.000500;,
  0.500000;0.000499;,
  0.250000;0.500000;,
  0.000000;0.500000;,
  1.000000;0.500000;,
  0.750000;0.500000;,
  0.500000;0.500000;,
  0.750000;0.500000;,
  0.750000;0.000500;,
  0.500000;0.500000;,
  0.250000;0.000499;,
  0.500000;0.000499;,
  0.250000;0.500000;,
  0.000000;0.500000;,
  0.250000;0.500000;,
  0.250000;1.000000;,
  0.000000;0.500000;,
  0.750000;0.500000;,
  1.000000;0.500000;,
  0.750000;0.500000;,
  0.750000;0.999501;,
  0.500000;0.500000;,
  0.750000;0.500000;,
  0.500000;0.999501;,
  0.250000;0.500000;,
  0.500000;0.500000;,
  0.500000;0.999501;,
  0.250000;1.000000;,
  0.250000;0.500000;,
  1.000000;0.000499;,
  0.750000;0.000000;,
  0.500000;0.000499;,
  0.250000;0.000499;,
  0.000000;0.000499;,
  0.999500;0.999501;,
  0.750000;0.999501;,
  0.500000;0.999501;,
  0.250000;0.999501;,
  0.000500;0.999501;,
  0.999887;0.500000;,
  0.750000;0.500000;,
  0.500000;0.500000;,
  0.250000;0.500000;,
  0.000000;0.500000;,
  0.999887;0.500000;,
  0.750000;0.000000;,
  0.750000;0.500000;,
  0.750000;0.000000;,
  0.500000;0.500000;,
  0.500000;0.000499;,
  0.250000;0.500000;,
  0.250000;0.000499;,
  0.750000;0.500000;,
  0.999887;0.500000;,
  0.999500;0.999501;,
  0.750000;0.500000;,
  0.750000;0.999501;,
  0.500000;0.500000;,
  0.750000;0.500000;,
  0.750000;0.999501;,
  0.500000;0.500000;,
  0.500000;0.999501;,
  0.250000;0.500000;,
  0.500000;0.500000;,
  0.500000;0.999501;,
  0.250000;0.500000;,
  0.250000;0.999501;,
  0.000000;0.500000;,
  0.250000;0.500000;,
  0.250000;0.999501;,
  0.000000;0.500000;,
  0.505850;0.975210;,
  0.495919;0.977705;,
  0.498686;0.982373;,
  0.503228;0.984995;,
  0.508472;0.984995;,
  0.513076;0.982424;,
  0.515781;0.977832;,
  0.515781;0.972588;,
  0.513158;0.967843;,
  0.508617;0.965221;,
  0.503041;0.965297;,
  0.498500;0.967919;,
  0.495919;0.972461;,
  0.505850;0.975210;,
  0.498686;0.982373;,
  0.505850;0.975210;,
  0.503228;0.984995;,
  0.505850;0.975210;,
  0.508472;0.984995;,
  0.505850;0.975210;,
  0.513076;0.982424;,
  0.505850;0.975210;,
  0.515781;0.977832;,
  0.505850;0.975210;,
  0.515781;0.972588;,
  0.505850;0.975210;,
  0.513158;0.967843;,
  0.505850;0.975210;,
  0.508617;0.965221;,
  0.505850;0.975210;,
  0.503041;0.965297;,
  0.505850;0.975210;,
  0.498500;0.967919;,
  0.505850;0.975210;,
  0.495919;0.977705;,
  0.495919;0.972461;,
  0.386327;0.008807;,
  0.386398;0.008767;,
  0.386482;0.008767;,
  0.386557;0.008806;,
  0.386601;0.008880;,
  0.386284;0.008882;,
  0.386403;0.009086;,
  0.386328;0.009042;,
  0.386285;0.008970;,
  0.386603;0.008965;,
  0.386561;0.009041;,
  0.386487;0.009085;,
  0.386442;0.008924;,
  0.386442;0.008924;,
  0.386327;0.008807;,
  0.386442;0.008924;,
  0.386398;0.008767;,
  0.386442;0.008924;,
  0.386482;0.008767;,
  0.386442;0.008924;,
  0.386557;0.008806;,
  0.386442;0.008924;,
  0.386601;0.008880;,
  0.386442;0.008924;,
  0.386603;0.008965;,
  0.386442;0.008924;,
  0.386561;0.009041;,
  0.386442;0.008924;,
  0.386487;0.009085;,
  0.386442;0.008924;,
  0.386403;0.009086;,
  0.386442;0.008924;,
  0.386328;0.009042;,
  0.386442;0.008924;,
  0.386285;0.008970;,
  0.386284;0.008882;;
 }

 MeshMaterialList {
  8;
  120;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  7,
  7,
  7,
  7,
  7,
  7,
  7,
  7,
  7,
  7,
  7,
  7;

  Material {
   1.0;1.0;1.0;1.0;;
   25.000000;
   0.898039;0.898039;0.898039;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "CM1.dds";
   }
  }

  Material {
   1.0;1.0;1.0;1.0;;
   25.000000;
   0.898039;0.898039;0.898039;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "CM6.dds";
   }
  }

  Material {
   1.0;1.0;1.0;1.0;;
   25.000000;
   0.898039;0.898039;0.898039;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "CM2.dds";
   }
  }

  Material {
   1.0;1.0;1.0;1.0;;
   25.000000;
   0.898039;0.898039;0.898039;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "CM3.dds";
   }
  }

  Material {
   1.0;1.0;1.0;1.0;;
   25.000000;
   0.898039;0.898039;0.898039;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "CM5.dds";
   }
  }

  Material {
   1.0;1.0;1.0;1.0;;
   25.000000;
   0.898039;0.898039;0.898039;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "CM4.dds";
   }
  }

  Material {
   1.0;1.0;1.0;1.0;;
   25.000000;
   0.898039;0.898039;0.898039;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "CM5.dds";
   }
  }

  Material {

   1.0;1.0;1.0;1.0;;
   25.000000;
   0.898039;0.898039;0.898039;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "CM3.dds";
   }
  }
 }
}