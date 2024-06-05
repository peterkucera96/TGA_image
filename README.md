## Spustenie programu 
1. kompilacia programu pomocou Apple clang main.c
2. ./a.out --output <nazov_suboru> --width <cislo> --height <cislo>
3. viz. priklad pouzitia



## Priklad pouzitia:
./kresleni --output out.tga --width 900 --height 450
line 10,10,50,40,255,255,0,255
clear
rectangle 10,10,100,100,255,0,0,255
save
circle 20,20,40,120,120,120,128
circle 10,20,20,120,120,120,220
triangle 50,50,20,0,255,0,255
rotated-rectangle 45,80,80,50,50,255,0,255,128
exit

## Vysvetlenie pouzitia skratiek
### save
saves the image to a file
### exit
saves the image to a file and exits the program
### clear
cleans the canvas
### line
Vsetky parametre musia byt oddelene ciarkou
Draws a line from x1, y1 to x2, y2
<br><b>Order of parameters: x1,y1,x2,y2,r,g,b,a</b>/Users/peterkucera/Desktop/School/1. Rocnik/Zimny semester/UPR/Domace ulohy/5 - Projekt
### rectangle
Draws a rectangle with the upper left corner at point x, y with width width and height height
<br><b>Order of parameters: x,y,width,height,r,g,b,a</b>
### circle
Draws a circle centered at x, y with radius radius
<br><b>Parameter order: x,y,radius,r,g,b,a</b>
### triangle
Draws an equilateral triangle centered at x, y with a radius of width
<br><b>Parameter order: x,y,width,r,g,b,a</b>
### rotated-rectangle
Draws a rotated rectangle centered at x, y with width, height and rotation angle
<br><b>Order of parameters: angle,x,y,width,height,r,g,b,a</b>
## sample usage
