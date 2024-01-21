function parms = read_vars()

% (base) (bash) ~/.../shallow/2d/sphere (app-swe-sphere) % more setprob.data
% 0                          % example
% 1.0000000000000000         % gravity
% 1                          % mapping
% 1                          % initial_condition
% 50.0000000000000000        % amp
% 0.2000000000000000         % hin
% 0.1000000000000000         % hout
% 0.0000000000000000         % ring-inner
% 0.4000000000000000         % ring-outer
% 0                          % ring_units
% -60.0000000000000000       % latitude
% 60.0000000000000000        % latitude
% -60.0000000000000000       % longitude
% 60.0000000000000000        % longitude


data = load('setprob.data');

parms.example = data(1);
parms.mapping = data(3);
parms.hin = data(6);
parms.hout = data(7);
parms.latitude = data(11:12);
parms.longitude = data(13:14);

end