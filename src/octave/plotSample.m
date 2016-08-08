#! /usr/bin/octave -qf

i = 0;
figure();

while (1 > 0)
  [t,x] = textread('temp.dat',"%f %f");
  %stem(x);
  %figure();
  plot(x);
 % axis([0 1000 -0.4 0.4], 'square');
  pause(0.3);
  %display(i++)
endwhile

pause()
