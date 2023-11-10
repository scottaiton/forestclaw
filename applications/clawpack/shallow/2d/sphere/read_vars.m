function parms = read_vars()

data = load('setprob.data');

parms.example = data(1);
parms.mapping = data(3);
parms.latitude = data(11:12);
parms.longitude = data(13:14);

end