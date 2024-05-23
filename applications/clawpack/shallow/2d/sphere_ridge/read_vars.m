function parms = read_vars()

% (base) (bash) ~/.../shallow/2d/sphere (app-swe-sphere) % more setprob.data
% 0                          % example
% 1.0000000000000000         % gravity
% 1                          % mapping
% 1                          % initial_condition
% 0.2000000000000000         % hin
% 0.1000000000000000         % hout
% 0.0000000000000000         % ring-inner
% 0.4000000000000000         % ring-outer
% 0                          % ring_units
% -60.0000000000000000       % latitude
% 60.0000000000000000        % latitude
% -60.0000000000000000       % longitude
% 60.0000000000000000        % longitude


data = load('setprob_2d.data');

parms.example = data(1);
parms.mapping = data(3);
parms.initial_condition = data(4);
parms.hin = data(5);
parms.hout = data(6);
parms.latitude = data(10:11);
parms.longitude = data(12:13);
parms.Px = data(14);
parms.Py = data(15);
parms.Pz = data(16);
parms.theta_wave = data(17);
parms.theta_ridge = data(18);
parms.ampl = data(19);
parms.alpha = data(20);
parms.speed = data(21);
parms.bathy = data(22:23);
parms.refine_threshold = data(24);
parms.coarsen_threshold = data(25);
end