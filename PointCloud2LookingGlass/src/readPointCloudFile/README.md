# PLY file format
Normal text. Each line ending with LF (0x0A). The first line contains `ply` indicates the file format.

The second line indicates the type (ascii or binary) and version.

Ignore lines (comment, info, etc.) before `element`

`element vertex 6782430` means there are 6782430 vertexes in the file. Each vertex may have:
1. `property float x` means coordinate x has float data type (4 bytes)
2. `property float y` means the next 4 bytes indicates coordinate y
3. `property float z` means the next 4 bytes indicates coordinate z
4. `property uchar red` means the next char (1 byte) indicate color Red
5. `property uchar green` means the next char (1 byte) indicate color Green
6. `property uchar blue` means the next char (1 byte) indicate color Blue

and other properties...

The next 15 bytes (as example shown above) would indicate the next element (vertex). There is no `<LF>` between each element. 

The last line `end_header<LF>` indicates the end of the header. 
