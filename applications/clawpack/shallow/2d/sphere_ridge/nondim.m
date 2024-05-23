function nondim(L,T)

% from paper
Rsphere = 6.371e6;

xbar = Rsphere;
ubar = 200;
gbar = 9.81;
tbar = 1;
Abar = 10;
bbar_0 = 40000;
bbar_1 = 20000;

% fix these
t0 = 1;
x0 = xbar;

% These have to depend on only u0,g0,h0
u0 = x0/t0;
g0 = x0/t0^2;
A0 = x0;
b0 = x0;
h0 = x0;

beta = h0*g0/u0^2;

fprintf('\n');
fprintf('Speed      : %.6e\n',ubar/u0);
fprintf('Gravity    : %.6e\n',gbar/g0);
fprintf('Amplitude  : %.6e\n',Abar/A0);
fprintf('dt         : %f\n',  2000/t0);
fprintf('t=24000    : %f\n',  24000/t0);
fprintf('t=40000    : %f\n',  40000/t0);
fprintf('Bathy (0)  : %f\n',  bbar_0/b0);
fprintf('Bathy (1)  : %f\n',  bbar_1/b0);
fprintf('g/beta     : %f\n',  gbar/beta);
