function parms = read_vars()

data = load('setprob.data');

parms.example = data(1);
parms.gravity = data(2);
parms.mapping = data(3);
parms.omega = data(7:9);
parms.latitude = data(13:14);
parms.longitude = data(15:16);

end